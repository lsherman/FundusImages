//
//  OtherFunctions.cpp
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/8/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#include "OtherFunctions.h"

void enhancer(cv::Mat& mat, float t)
{
    mat *= pow(10.0, t);
    cv::pow(mat, t, mat);
    
    cv::normalize(mat, mat, 0.0, 1.0, cv::NORM_MINMAX);
}

void enhancer_new(cv::Mat& mat, float t)
{
    cv::pow(mat, t, mat);
    
    cv::normalize(mat, mat, 0.0, 1.0, cv::NORM_MINMAX);
}