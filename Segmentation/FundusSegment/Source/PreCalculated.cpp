//
//  PreCalculated.cpp
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/9/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#include "PreCalculated.h"

cv::Mat GetStrelLength9(double angle)
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