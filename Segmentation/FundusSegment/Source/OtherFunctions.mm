//
//  OtherFunctions.cpp
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/8/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#include "OtherFunctions.h"

void cvMatElementwisePower(cv::Mat& mat, double t)
{
    int rows = mat.rows;
    int cols = mat.cols;
    
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            mat.at<double>(i,j) = pow(mat.at<double>(i,j), t);
        }
    }
}

double matMax(cv::Mat& mat)
{
    int rows = mat.rows;
    int cols = mat.cols;
    
    double maxVal = -10000000.0;
    
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (mat.at<double>(i, j) > maxVal)
            {
                maxVal = mat.at<double>(i, j);
            }
        }
    }
    
    return maxVal;
}

double matMin(cv::Mat& mat)
{
    int rows = mat.rows;
    int cols = mat.cols;
    
    double minVal = 10000000.0;
    
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (mat.at<double>(i, j) < minVal)
            {
                minVal = mat.at<double>(i, j);
            }
        }
    }
    
    return minVal;
}

void enhancer(cv::Mat& mat, double t)
{
    mat *= pow(10.0, t);
    cvMatElementwisePower(mat, t);
    
    double min = matMin(mat);
    double max = matMax(mat);
    mat = (mat - min)/(max - min);
}

void enhancer_new(cv::Mat& mat, double t)
{
    cvMatElementwisePower(mat, t);
    
    double min = matMin(mat);
    double max = matMax(mat);
    mat = (mat - min)/(max - min);
}