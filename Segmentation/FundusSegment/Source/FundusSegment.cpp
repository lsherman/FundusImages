/* ===========================================================================

	Project: Fundus Image Segmentation Library

    Copyright (C) 2013

    Niraj Rayalla,
	Lucas Sherman, email: LucasASherman@gmail.com

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

=========================================================================== */

// Include Header
#include "FundusSegment.h"

// Include Dependencies
#include "MatlabFunctions.h"

namespace {
namespace filescope {

    // File locations on disk
    char const * MASK_IMAGE_FILE = "fovMask.jpg";
    char const * POST_IMAGE_FILE = "fovMask_2.jpg";

    // Image resolution for segmentation
    size_t RESOLUTION_X = 500;
    size_t RESOLUTION_Y = 500;

    // Features for GMM classifier
    size_t ATTRS = 6; // Number of features
    size_t VAL   = 0; // Pixel intensity
    size_t AVG   = 1; // Mean of neighborhood
    size_t VAR   = 2; // Variance of neighborhood
    size_t MAX   = 3; // Max of neighborhood
    size_t MIN   = 4; // Min of neighborhood
    size_t PCT   = 5; // Value percentile (ie. Percent of neighbors with higher intensities)

    // Segmentation parameters
    size_t MINIMUM_NEIGHBORHOOD = 30;

    // Field-of-view mask image
    cv::Mat fovMask;
    cv::Mat postMask;

    // Trained GMM classifier
    cv::EM * classifier;
    
    // --------------------------------------------------------------------
    //  Enhances the value curve of the input image using a level factor t
    // --------------------------------------------------------------------
    void enhancer(cv::Mat& mat, float t)
    {
        mat *= pow(10.0, t);
        cv::pow(mat, t, mat);
    
        cv::normalize(mat, mat, 0.0, 1.0, cv::NORM_MINMAX);
    }

    // --------------------------------------------------------------------
    //  Computes GMM classifier attributes for each unclassified pixel
    //  Takes the mask of unclassified pixels and the image as input
    //  This is an interesting problem, you apply a series of convolution 
    //  kernels to only the masked elements of a matrix. Not sure how to
    //  do this with OpenCV so we will inefficiently do it ourselves.
    // --------------------------------------------------------------------
    void computeGmmAttributes(cv::Mat & noclass, cv::Mat & image)
    {
        int K = 7; // Width of the kernel

        int M = (K - 1) / 2;
        for (int j = 0; j < noclass.cols; ++j)
        for (int i = 0; i < noclass.rows; ++i)
        {
            if (noclass.at<float>(i,j) == 0.0f) continue; // already classified

            float pix = image.at<float>(i, j); // Pixel intensity

            // Compute the bounds for the kernel mask
            int rMin = (i < M) ? 0 : i - M;
            int rMax = (i > noclass.rows-M) ? noclass.rows : i + M + 1;
            int cMin = (j < M) ? 0 : j - M;
            int cMax = (j > noclass.cols-M) ? noclass.cols : j + M + 1;

            // Compute the pixel's classification attributes
            float low  = 2.0f;
            float high = -1.0f;
            float sum  = 0.0f;
            int   pct  = 0;
            for (int c = cMin; c < cMax; c++)
            for (int r = rMin; r < rMax; r++)
            {
                auto val = image.at<float>(r, c);
                sum  += val;
                low  =  std::min(low, val);
                high =  std::max(high, val);
                if (val > pix) pct++;
            }

            // Compute the sample variance (Same way MATLAB trains it)
            float ksize = (float)(rMax - rMin) * (cMax - cMin);
            float mean  = sum / ksize;
            float stat  = 0.0f;
            for (int c = cMin; c < cMax; c++)
            for (int r = rMin; r < rMax; r++)
            {
                auto val = image.at<float>(r, c);
                val -= mean;
                stat += val*val;
            }

            // Calculate the pixels classification attributes
            float variance = stat / (ksize-1);
            cv::Mat attrs(1, 6, CV_32FC1);
            attrs.at<float>(0, filescope::VAL) = pix;
            attrs.at<float>(0, filescope::AVG) = mean;
            attrs.at<float>(0, filescope::VAR) = sqrt(variance);
            attrs.at<float>(0, filescope::MAX) = high;
            attrs.at<float>(0, filescope::MIN) = low;
            attrs.at<float>(0, filescope::PCT) = pct / ksize;
            
            // Calculate the probability of being a vessel
            //std::cout << attrs << std::endl;
            cv::Mat probs; 
            filescope::classifier->predict(attrs, probs);
            //std::cout << probs.at<double>(0) << " " << probs.at<double>(1) << std::endl;
            noclass.at<float>(i,j) = probs.at<double>(0);
        }
    }

} // filescope
} // anonymous

// --------------------------------------------------------------------
//  Initializes the FOV masking image for performing segmentation
// --------------------------------------------------------------------
int startup(char const* directory)
{
    std::string path(directory); 

    int greyCheck = 0.9*255;

    // Load the mask image from the disk and extract one of the color
    // channels to use in computing the associated binary mask 
    cv::Mat maskImage;
    auto maskFile = path + std::string(filescope::MASK_IMAGE_FILE);
    resize(cv::imread(maskFile.c_str()), maskImage, 
           cv::Size(filescope::RESOLUTION_X, filescope::RESOLUTION_Y), 
           0, 0, cv::INTER_CUBIC);
    cv::vector<cv::Mat> channels(4);
    cv::split(maskImage, channels);
    
    // Threshold the mask image to create a binary mask
    channels[1].convertTo(filescope::fovMask, CV_32FC1, 1.0);
    cv::threshold(filescope::fovMask, filescope::fovMask, greyCheck, 
        1.0, cv::THRESH_BINARY);
 
    // Load the postprocessing mask image from the disk and extract one of the color
    // channels to use in computing the associated binary mask 
    cv::Mat postImage;
    auto postFile = path + std::string(filescope::POST_IMAGE_FILE);
    resize(cv::imread(postFile.c_str()), postImage, 
           cv::Size(filescope::RESOLUTION_X, filescope::RESOLUTION_Y), 
           0, 0, cv::INTER_CUBIC);
    cv::vector<cv::Mat> postChannels(4);
    cv::split(postImage, postChannels);
    
    // Threshold the mask image to create a binary mask
    postChannels[1].convertTo(filescope::postMask, CV_32FC1, 1.0);
    cv::threshold(filescope::postMask, filescope::postMask, greyCheck, 
        1.0, cv::THRESH_BINARY);

    // Load the training data for the GMM classifier from disk
    CvMLData Data; // :TODO: Stupid issue with hard maximum memory allocation requires multiple files for large data sets
    auto file = path + "train_data1.csv";
    Data.read_csv(file.c_str());
    cv::Mat gmmSamples1 = cv::Mat(Data.get_values()).clone(); 
    gmmSamples1.convertTo(gmmSamples1, CV_64FC1, 1.0f);
    file = path + "train_data2.csv";
    Data.read_csv(file.c_str());
    cv::Mat gmmSamples2 = cv::Mat(Data.get_values()).clone(); 
    gmmSamples2.convertTo(gmmSamples2, CV_64FC1, 1.0f);
    cv::hconcat(gmmSamples1, gmmSamples2, gmmSamples1);
    file = path + "train_data3.csv";
    Data.read_csv(file.c_str());
    cv::Mat gmmSamples3 = cv::Mat(Data.get_values()).clone(); 
    gmmSamples3.convertTo(gmmSamples3, CV_64FC1, 1.0f);
    cv::hconcat(gmmSamples1, gmmSamples3, gmmSamples1);
    transpose(gmmSamples1, gmmSamples1);
    filescope::classifier = new cv::EM(2, cv::EM::COV_MAT_GENERIC); // 2-clusters, non-independent covariance matrix
    filescope::classifier->train(gmmSamples1);
    auto means   = filescope::classifier->get<cv::Mat>("means");
    auto weights = filescope::classifier->get<cv::Mat>("weights");
    auto covs    = filescope::classifier->get<std::vector<cv::Mat>>("covs");

    return 0;
}

// --------------------------------------------------------------------
//  Performs segmentation on the input image and returns the result
//  :TODO: Some of these image functions implemented manually are 
//        probably already available in openCV, would be faster
// --------------------------------------------------------------------
int segmentImage(void* data, size_t length, void** result, size_t* outLength)
{
    // ---
    //
    // Scale the input to the segment resolution and perform pre-adjustments
    //
    // ---

    // Decode the input image and scale to the segmentation resolution
    std::vector<UInt8> input(length); // :TODO: Remove this memcpy
    memcpy(&input[0], data, length);
    cv::Mat colorIn = cv::imdecode(input, CV_LOAD_IMAGE_COLOR);
    cv::Mat HH0;
    cv::vector<cv::Mat> channels(4);
    cv::split(colorIn, channels);
    HH0 = channels[1];
    //cv::cvtColor(colorIn, HH0, CV_RGB2GRAY);
    resize(HH0, HH0, cv::Size(filescope::RESOLUTION_X, filescope::RESOLUTION_Y), 
        0, 0, cv::INTER_NEAREST);
    HH0.convertTo(HH0, CV_32FC1, (float)(1.0/255.0));
    
    // Contrast adjustment (Window-Level adjusted to [1%,99%])
    mlab_imadjust(HH0);
    
    // ---
    //
    // Extract vessels using a high pass filter
    //
    // ---

    cv::Mat I_bg;
    cv::blur(HH0, I_bg, cv::Size(10,10), cv::Point(-1,-1), cv::BORDER_REPLICATE);
    cv::Mat I_sc = HH0 - I_bg;
    
    cv::threshold(I_sc, I_sc, 0.0, 0.0, cv::THRESH_TRUNC);
    cv::Mat I_sc1 = cv::abs(I_sc);
    cv::Mat I_sc3 = I_sc1.mul(filescope::fovMask);
    mlab_imadjust(I_sc3);
    
    // ---
    //
    // Extract vessels using a line feature detection
    //
    // ---

    // Extraction of morphologically tophat reconstructed image (T=Kn1)
    cv::Mat I_neg = (1.0-HH0).mul(filescope::fovMask);
    mlab_imadjust(I_neg);
    filescope::enhancer(I_neg, 2.0);
    
    // Feature detect lines at 15 degree intervals
    cv::Mat Kn1;
    cv::morphologyEx(I_neg, Kn1, cv::MORPH_TOPHAT, mlab_strelLine(9, 0), cv::Point(-1,-1), 1, cv::BORDER_CONSTANT, 0.0f);
    for(int h = 1; h < 12; ++h)
    {
        cv::Mat partialLines;
        cv::morphologyEx(I_neg, partialLines, cv::MORPH_TOPHAT, mlab_strelLine(9, h*15.0), cv::Point(-1,-1), 1, cv::BORDER_CONSTANT, 0.0f);
        cv::max(partialLines, Kn1, Kn1);
    }
    cv::normalize(Kn1, Kn1, 0.0, 1.0, cv::NORM_MINMAX);
    mlab_imadjust(Kn1);
    
    // ---
    //
    // Identify the major vessels as those in (High pass AND Line feature)
    //
    // ---

    // Extract binary mask from the high pass and line feature filtered images
    // Also, remove any dangling islands before selecting the major vessels
    cv::threshold(Kn1, Kn1, 0.2, 1.0, cv::THRESH_BINARY);
    mlab_bwareaopen(Kn1, filescope::MINIMUM_NEIGHBORHOOD);
    cv::threshold(I_sc3, I_sc3, 0.2, 1.0, cv::THRESH_BINARY);
    mlab_bwareaopen(I_sc3, filescope::MINIMUM_NEIGHBORHOOD);
    
    // Mark the intersecting regions as major vessels
    cv::Mat vessels = Kn1.mul(I_sc3);
    cv::medianBlur(vessels, vessels, 3);
    cv::threshold(vessels, vessels, 0.0, 1.0, cv::THRESH_BINARY);
    
    // ---
    //
    // Perform GMM classification on the disputed pixels (High pass XOR Line feature)
    //
    // ---

    // Finding the disputed pixels that need additional classification
    cv::Mat UC1, UC2, unclassedPix;
    cv::threshold(Kn1-vessels,   UC1, 0.0, 1.0, cv::THRESH_BINARY);
    cv::threshold(I_sc3-vessels, UC2, 0.0, 1.0, cv::THRESH_BINARY);
    unclassedPix = UC1 + UC2;

    // Create a new mask of disputed pixels with a high probability of being vessels
    double GMM_PROB_THRESH = 0.9; // 90% likelyhood threshold
    filescope::computeGmmAttributes(unclassedPix, HH0); // Scales unclassed pix to probability of being a vessel
    cv::threshold(unclassedPix, unclassedPix, GMM_PROB_THRESH, 1.0, cv::THRESH_BINARY); 
    vessels = vessels+unclassedPix;
    mlab_bwareaopen(vessels, filescope::MINIMUM_NEIGHBORHOOD);
    vessels = vessels.mul(filescope::postMask);

    // ---
    //
    // Compose the GMM pixels and the major vessels as the final vessel mask
    //
    // ---

    vessels.convertTo(vessels, CV_8UC1, 255);
    cv::Mat outputImage(vessels.rows, vessels.cols, CV_8UC4);
    int fromTo[] = { 0,0, 0,1, 0,2, 0,3 };
    cv::mixChannels(&vessels, 1, &outputImage, 1, fromTo, 4);
    std::vector<uchar> buf;
    cv::imencode(".png", outputImage, buf);

    *result    = malloc(buf.size());
    *outLength = buf.size();
    memcpy(*result, &buf[0], buf.size());

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
    if (filescope::classifier) delete filescope::classifier;
}