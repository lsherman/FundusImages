//
//  FundusSegmenter.cpp
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/7/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#include "FundusSegmenter.h"

FundusSegmenter::FundusSegmenter()
{
    //Get resources
    NSString* bundlePath = [[NSBundle mainBundle] pathForResource:@"iosBundle" ofType:@"bundle"];
    
    NSString* gImageName = [[NSBundle bundleWithPath:bundlePath] pathForResource:@"DRIVE_mask_1_processed_resized" ofType:@"jpg"];
    UIImage* gImage = [[UIImage alloc] initWithContentsOfFile:gImageName];
    gImage = ScaleImage(gImage, CGSizeMake(500, 500));
    
    NSString* g0ImageName = [[NSBundle bundleWithPath:bundlePath] pathForResource:@"DRIVE_second_processed_resized" ofType:@"jpg"];
    UIImage* g0Image = [[UIImage alloc] initWithContentsOfFile:g0ImageName];
    g0Image = ScaleImage(g0Image, CGSizeMake(500, 500));
    
    //Setup matrices
    cv::Mat gTemp = cvMatFromUIImage(gImage);
    cv::vector<cv::Mat> channels(4);
    cv::split(gTemp, channels);
    g = channels[1];
    
    cv::Mat g0Temp = cvMatFromUIImage(g0Image);
    cv::vector<cv::Mat> channels2(4);
    cv::split(g0Temp, channels2);
    g0 = channels2[1];
    
    //Turn g and g0 into binary images (0 or 1)
    gRows = g.rows;
    gCols = g.cols;
    
    g0Rows = g0.rows;
    g0Cols = g0.cols;
    
    int greyCheck = 0.9*255;
    
    gDouble = cv::Mat(gRows, gCols, CV_64FC1);
    g0Double = cv::Mat(g0Rows, g0Cols, CV_64FC1);
    
    for (int i = 0; i < gRows; ++i)
    {
        for (int j = 0; j < gCols; ++j)
        {
            if (g.at<UInt8>(i, j) > greyCheck)
            {
                g.at<UInt8>(i, j) = 1;
                gDouble.at<double>(i,j) = 1.0;
            }
            else
            {
                g.at<UInt8>(i, j) = 0;
                gDouble.at<double>(i,j) = 0.0;
            }
        }
    }
    
    for (int i = 0; i < g0Rows; ++i)
    {
        for (int j = 0; j < g0Cols; ++j)
        {
            if (g0.at<UInt8>(i, j) > greyCheck)
            {
                g0.at<UInt8>(i, j) = 1;
                g0Double.at<double>(i,j) = 1.0;
            }
            else
            {
                g0.at<UInt8>(i, j) = 0;
                g0Double.at<double>(i,j) = 0.0;
            }
        }
    }
}

cv::Mat DoubleToUInt(cv::Mat& mat)
{
    cv::Mat temp(mat.rows, mat.cols, CV_8UC1);
    
    for(int i = 0; i < mat.rows; ++i)
    {
        for(int j = 0; j < mat.cols; ++j)
        {
            temp.at<UInt8>(i,j) = (UInt8)(mat.at<double>(i,j)*255.0);
            
        }
    }
    
    return temp;
}

void IsGreaterThan(cv::Mat& mat, double val)
{
    for (int i = 0; i < mat.rows; ++i)
    {
        for (int j = 0; j < mat.cols; ++j)
        {
            if (mat.at<double>(i,j) > 0.2)
            {
                mat.at<double>(i,j) = 1.0;
            }
            else
            {
                mat.at<double>(i,j) = 0.0;
            }
        }
    }
}

void* FundusSegmenter::SegmentImage(void* data, size_t width, size_t height)
{
    //reize now.
    cv::Mat I = cvMatFromRawImg(img, width, height);
    
    // "channels" is a vector of 4 Mat arrays: RGBA
    cv::vector<cv::Mat> channels(4);
    // split I into the 4 channels.
    cv::split(I, channels);
    
    cv::Mat K = channels[1];
    
    cv::Mat HH0 = mlab_im2double(K);
    //Nearest resize
    /*cv::Mat HH0;
    cv::resize(H, HH0, cv::Size_<int>(gRows, gCols));
    //cvResize(&H, &HH0, CV_INTER_NN);*/
    
    //Contrast adjustment
    mlab_imadjust(HH0);
    
    
    //High pass filter image extraction (H=I_sc3)
    cv::Mat w = mlab_fspecialAverage(10, 10);
    cv::Mat I_bg = mlab_filterReplicate(HH0, w);
    cv::Mat I_sc = HH0 - I_bg;
    
    cv::Mat I_sc1(gRows, gCols, CV_64FC1);
    for(int i = 0; i < gRows; ++i)
    {
        for(int j = 0; j < gCols; ++j)
        {
            if (I_sc.at<double>(i,j) >= 0.0)
            {
                I_sc.at<double>(i,j) = 0.0;
                I_sc1.at<double>(i,j) = 0.0;
            }
            else
            {
                //Get absolute value.
                if (I_sc.at<double>(i,j) < 0.0)
                {
                    I_sc1.at<double>(i,j) = -1.0 * I_sc.at<double>(i,j);
                }
            }
        }
    }
    cv::Mat I_sc3 = I_sc1.mul(gDouble);
    mlab_imadjust(I_sc3);
    enhancer_new(I_sc3, 1.0);
    
    //Extraction of morphologically tophap reconstructed image (T=Kn1)
    cv::Mat I_neg = (1.0-HH0).mul(gDouble);
    cv::Mat U = I_neg.clone();
    mlab_imadjust(U);
    enhancer(U, 2.0);
    
    cv::Mat images(12, gRows*gCols, CV_64FC1);
    cv::Mat topArr[12];
    for(int h = 1; h <= 12; ++h)
    {
        cv::Mat se = mlab_strelLine(9, ((double)h-1.0)*15.0);
        cv::Mat UTemp = U.clone();
        cv::Mat im_top = mlab_imtophat(UTemp, se);
        topArr[h-1] = im_top;
        //Update the current row in images
        for (int i = 0; i < gRows; ++i)
        {
        for (int j = 0; j < gCols; ++j)
        {
            images.at<double>(h-1,i*gRows+j)=im_top.at<double>(i,j);
        }
        }
    }
    //Find the maximum of the top
    cv::Mat max_top(1, gRows*gCols, CV_64FC1);
    for (int j = 0; j < gRows*gCols; ++j)
    {
        double maxTop = -10000000.0f;
        for (int i = 0; i < 12; ++i)
        {
            if (images.at<double>(i,j) > maxTop)
            {
                maxTop = images.at<double>(i,j);
            }
        }
        
        max_top.at<double>(1,j) = maxTop;
    }
    cv::Mat Kn1(gRows, gCols, CV_64FC1);
    for (int i = 0; i < gRows; ++i)
    {
        for (int j = 0; j < gCols; ++j)
        {
            Kn1.at<double>(i,j) = max_top.at<double>(1,i*gRows+j);
        }
    }
    enhancer(Kn1, 1.0);
    mlab_imadjust(Kn1);
    
    //Extraction of two binary images, (H'=Kn2, T'=I_sc4)
    cv::Mat Kn2 = Kn1.clone();
    IsGreaterThan(Kn2, 0.2);
    mlab_bwareaopen(Kn2, 30);
    cv::Mat I_sc4 = I_sc3.clone();
    IsGreaterThan(I_sc4, 0.2);
    mlab_bwareaopen(I_sc4, 30);
    
    //Finding the intersecting regions in the two binary images as the major vessels (P=V1=H' intersect T')
    cv::Mat P = Kn2.mul(I_sc4);
    cv::Mat e = mlab_medfilt2_2_2(P);
    P = e.clone();
    IsGreaterThan(P, 0.0);
    
    //Finding remaining pixels in H' and T' after removing V1 from them. i.e.,
    //H1=P1, T1=P2, C=pixel locations in H1 and T1.
    cv::Mat P1 = Kn2-P;
    IsGreaterThan(P1, 0.0);
    cv::Mat P2 = I_sc4-P;
    IsGreaterThan(P2, 0.0);
    
    return rawImgFromCVMat(DoubleToUInt(P));
}