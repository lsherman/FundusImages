/* ===========================================================================

	Project: Fundus Image Segmentation Library

    Copyright (C) 2013

    Niraj Rayalla,

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
#include "PreCalculated.h"

cv::Mat GetStrelLength9(float angle)
{
    if (angle == 15.0)
    {
        cv::Mat retMat(3, 9, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(2, 0) = 1;
        retMat.at<UInt8>(2, 1) = 1;
        retMat.at<UInt8>(2, 2) = 1;
        
        retMat.at<UInt8>(1, 3) = 1;
        retMat.at<UInt8>(1, 4) = 1;
        retMat.at<UInt8>(1, 5) = 1;
        
        retMat.at<UInt8>(0, 6) = 1;
        retMat.at<UInt8>(0, 7) = 1;
        retMat.at<UInt8>(0, 8) = 1;
        
        return retMat;
    }
    
    if (angle == 30.0)
    {
        cv::Mat retMat(5, 7, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(4, 0) = 1;
        retMat.at<UInt8>(3, 1) = 1;
        retMat.at<UInt8>(3, 2) = 1;
        
        retMat.at<UInt8>(2, 3) = 1;
        retMat.at<UInt8>(1, 4) = 1;
        retMat.at<UInt8>(1, 5) = 1;
        
        retMat.at<UInt8>(0, 6) = 1;
        
        return retMat;
    }
    
    if (angle == 60.0)
    {
        cv::Mat retMat(7, 5, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(6, 0) = 1;
        retMat.at<UInt8>(5, 1) = 1;
        retMat.at<UInt8>(4, 1) = 1;
        
        retMat.at<UInt8>(3, 2) = 1;
        retMat.at<UInt8>(2, 3) = 1;
        retMat.at<UInt8>(1, 3) = 1;
        
        retMat.at<UInt8>(0, 4) = 1;
        
        return retMat;
    }
    
    if (angle == 75.0)
    {
        cv::Mat retMat(9, 3, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(8, 0) = 1;
        retMat.at<UInt8>(7, 0) = 1;
        retMat.at<UInt8>(6, 0) = 1;
        
        retMat.at<UInt8>(5, 1) = 1;
        retMat.at<UInt8>(4, 1) = 1;
        retMat.at<UInt8>(3, 1) = 1;
        
        retMat.at<UInt8>(2, 2) = 1;
        retMat.at<UInt8>(1, 2) = 1;
        retMat.at<UInt8>(0, 2) = 1;
        
        return retMat;
    }
    
    
    
    
    
    
    if (angle == 105.0)
    {
        cv::Mat retMat(9, 3, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(0, 0) = 1;
        retMat.at<UInt8>(1, 0) = 1;
        retMat.at<UInt8>(2, 0) = 1;
        
        retMat.at<UInt8>(3, 1) = 1;
        retMat.at<UInt8>(4, 1) = 1;
        retMat.at<UInt8>(5, 1) = 1;
        
        retMat.at<UInt8>(6, 2) = 1;
        retMat.at<UInt8>(7, 2) = 1;
        retMat.at<UInt8>(8, 2) = 1;
        
        return retMat;
    }
    
    if (angle == 120.0)
    {
        cv::Mat retMat(7, 5, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(0, 0) = 1;
        retMat.at<UInt8>(1, 1) = 1;
        retMat.at<UInt8>(2, 1) = 1;
        
        retMat.at<UInt8>(3, 2) = 1;
        retMat.at<UInt8>(4, 3) = 1;
        retMat.at<UInt8>(5, 3) = 1;
        
        retMat.at<UInt8>(6, 4) = 1;
        
        return retMat;
    }
    
    
    
    if (angle == 150.0)
    {
        cv::Mat retMat(5, 7, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(0, 0) = 1;
        retMat.at<UInt8>(1, 1) = 1;
        retMat.at<UInt8>(1, 2) = 1;
        
        retMat.at<UInt8>(2, 3) = 1;
        retMat.at<UInt8>(3, 4) = 1;
        retMat.at<UInt8>(3, 5) = 1;
        
        retMat.at<UInt8>(4, 6) = 1;
        
        return retMat;
    }
    
    if (angle == 165.0)
    {
        cv::Mat retMat(3, 9, CV_8U, cv::Scalar(0));
        
        retMat.at<UInt8>(0, 0) = 1;
        retMat.at<UInt8>(0, 1) = 1;
        retMat.at<UInt8>(0, 2) = 1;
        
        retMat.at<UInt8>(1, 3) = 1;
        retMat.at<UInt8>(1, 4) = 1;
        retMat.at<UInt8>(1, 5) = 1;
        
        retMat.at<UInt8>(2, 6) = 1;
        retMat.at<UInt8>(2, 7) = 1;
        retMat.at<UInt8>(2, 8) = 1;
        
        return retMat;
    }
    
    
    return cv::Mat(9, 9, CV_8U, cv::Scalar(0));
}