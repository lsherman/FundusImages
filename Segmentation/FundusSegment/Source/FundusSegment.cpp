//
//  FundusSegmenter.cpp
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/7/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

// Include Header
#include "FundusSegment.h"

// Include Dependencies
#include "MatlabFunctions.h"
#include "OtherFunctions.h"

namespace {
namespace filescope {

    // File locations of field-of-view masks
    char const * MASK_IMAGE_FILE = "fovMask.jpg";

    // Image resolution for segmentation
    size_t RESOLUTION_X = 500;
    size_t RESOLUTION_Y = 500;

} // filescope
} // anonymous

    cv::Mat g;

// --------------------------------------------------------------------
//  Initializes the FOV masking image for performing segmentation
// --------------------------------------------------------------------
int startup(char const* directory)
{
    auto string = std::string(directory) + filescope::MASK_IMAGE_FILE;

    // Load the mask images from the disk and extract one of the color
    // channels to use in computing the associated binary mask 
    cv::Mat maskImage;
    resize(cv::imread(string.c_str()), maskImage, 
           cv::Size(filescope::RESOLUTION_X, filescope::RESOLUTION_Y), 
           0, 0, cv::INTER_CUBIC);
    cv::vector<cv::Mat> channels(4);
    cv::split(maskImage, channels);
    
    // Threshold the mask image to create a binary mask
    int greyCheck = 0.9*255;
    channels[1].convertTo(g, CV_32FC1, 1.0);
    cv::threshold(g, g, greyCheck, 1.0, cv::THRESH_BINARY);
 
    return 0;
}

// --------------------------------------------------------------------
//  Performs segmentation on the input image and returns the result
//  :TODO: Some of these image functions implemented manually are 
//        probably already available in openCV, would be faster
// --------------------------------------------------------------------
int segmentImage(void* data, size_t length, void** result, size_t* outLength)
{
    // Decode the input image and scale to the segmentation resolution
    std::vector<UInt8> input(length); // :TODO: Can we remove this memcpy???
    memcpy(&input[0], data, length);
    cv::Mat I;
    resize(cv::imdecode(input, CV_LOAD_IMAGE_COLOR), I, 
           cv::Size(filescope::RESOLUTION_X, filescope::RESOLUTION_Y), 
           0, 0, cv::INTER_CUBIC);

    // Convert the input image (apparently its red channel for reasons 
    // unknown? :TODO:) to a floating point format for processing
    cv::vector<cv::Mat> channels(4);
    cv::split(I, channels);
    cv::Mat HH0;
    channels[1].convertTo(HH0, CV_32FC1, (float)(1.0/255.0));
    
    // Contrast adjustment (Window-Level adjusted 0.01-0.99)
    mlab_imadjust(HH0);
    
    // High pass filter image extraction (H=I_sc3)
    cv::Mat I_bg;
    cv::blur(HH0, I_bg, cv::Size(10,10), cv::Point(-1,-1), cv::BORDER_REPLICATE);
    cv::Mat I_sc = HH0 - I_bg;
    
    // :TODO: This can probably be refactored into a cv transform
    cv::Mat I_sc1(g.rows, g.cols, CV_32FC1);
    for(int i = 0; i < g.rows; ++i)
    for(int j = 0; j < g.cols; ++j)
    {
        if (I_sc.at<float>(i,j) >= 0.0)
        {
            I_sc.at<float>(i,j) = 0.0;
            I_sc1.at<float>(i,j) = 0.0;
        }
        else
        {
            //Get absolute value.
            if (I_sc.at<float>(i,j) < 0.0)
            {
                I_sc1.at<float>(i,j) = -1.0 * I_sc.at<float>(i,j);
            }
        }
    }
    cv::Mat I_sc3 = I_sc1.mul(g);
    mlab_imadjust(I_sc3);
    enhancer_new(I_sc3, 1.0);
    
    // Extraction of morphologically tophat reconstructed image (T=Kn1)
    cv::Mat I_neg = (1.0-HH0).mul(g);
    cv::Mat U = I_neg.clone();
    mlab_imadjust(U);
    enhancer(U, 2.0);
    
    cv::Mat images[12];
    for(int h = 0; h < 12; ++h)
    {
        cv::Mat se = mlab_strelLine(9, h*15.0); // Should we precalculate these and store in memory?
                                                // We can probably optimize something in this loop
        images[h] = mlab_imtophat(U, se);
        images[h] = images[h].reshape(0, 1);
    }

    // Find the maximum of the top
    cv::Mat max_top;
    cv::max(images[0], images[1], max_top);
    for (int i = 2; i < 12; i++)
    {
        cv::max(images[i], max_top, max_top);
    }
    cv::Mat Kn1 = max_top.reshape(0, g.rows);
    enhancer(Kn1, 1.0);
    mlab_imadjust(Kn1);
    
    //Extraction of two binary images, (H'=Kn1, T'=I_sc4)
    cv::threshold(Kn1, Kn1, 0.2, 1.0, cv::THRESH_BINARY);
    mlab_bwareaopen(Kn1, 30);
    cv::Mat I_sc4 = I_sc3.clone();
    cv::threshold(I_sc4, I_sc4, 0.2, 1.0, cv::THRESH_BINARY);
    mlab_bwareaopen(I_sc4, 30);
    
    //Finding the intersecting regions in the two binary images as the major vessels (P=V1=H' intersect T')
    cv::Mat P = Kn1.mul(I_sc4);
    cv::Mat e;
    cv::medianBlur(P, e, 3);
    cv::threshold(e, P, 0.0, 1.0, cv::THRESH_BINARY);
    
    // Finding remaining pixels in H' and T' after removing V1 from them. i.e.,
    // H1=P1, T1=P2, C=pixel locations in H1 and T1.
    cv::Mat P1, P2;
    cv::threshold(Kn1-P,   P1, 0.0, 1.0, cv::THRESH_BINARY);
    cv::threshold(I_sc4-P, P2, 0.0, 1.0, cv::THRESH_BINARY);
    
    // ---
    //
    // :TODO: :TODO: You are missing the GMM classifier here :TODO: TODO:
    //
    // ---

    cv::Mat segmentedImg;
    P.convertTo(segmentedImg, CV_8UC1, 255);
    segmentedImg = segmentedImg.reshape(2, 0);
    std::vector<uchar> buf;
    cv::imencode(".jpg", segmentedImg, buf);

    void* data = malloc(buf.size());
    *outLength = buf.size();
    memcpy(data, &buf[0], buf.size());
    (*result) = data;

    return 0;
}

// --------------------------------------------------------------------
//  Frees image data allocated by a call to segmentImage
// --------------------------------------------------------------------
void freeImage(void* data)
{
    free(data);
}

// --------------------------------------------------------------------
//  Performs cleanup operations before library unload
// --------------------------------------------------------------------
void shutdown()
{
}