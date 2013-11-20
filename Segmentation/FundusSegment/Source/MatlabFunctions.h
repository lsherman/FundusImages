//
//  MatlabFunctions.h
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/8/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

// Begin Definition
#ifndef FSEG_MATLAB_FUNCTIONS
#define FSEG_MATLAB_FUNCTIONS

// Include Headers
#include "Common.h"

//Converts a 2-D 8bit one channel matrix to a double matrix(64bit) one channel
//such that all values are scaled by a factor of 1/255.s
//If the input matrix consists of any other type than integer types then
//an equivalent doulbe matrix is returned.
cv::Mat mlab_im2double(cv::Mat& mat);

//Converts a 2-D matrix to a matrix consisting of scaled numbers such that
//the minimum number is a zero while the maximum is a 1.
void mlab_imadjust(cv::Mat& mat);

//Create an average kernel to be used with a filter.
cv::Mat mlab_fspecialAverage(int width, int height);

//Apply a filter with replicated borders.
cv::Mat mlab_filterReplicate(cv::Mat src, cv::Mat kernel);

//Make a matrix with a line of length 'length' and at an angle 'angle'.
//The line is composed of 1's and the rest are 0's.
cv::Mat mlab_strelLine(int length, double angle);

//Top hat filtering
cv::Mat mlab_imtophat(cv::Mat& IM, cv::Mat& SE);

cv::Mat mlab_imopen(cv::Mat& IM, cv::Mat& SE);

//Removes from a binary image all connected components (objects)
//that have fewer than P pixels.
//BW is a matrix of type double and will be modified.
void mlab_bwareaopen(cv::Mat& BW, int P);


//Given a matrix of type double, a matrix with median filtering
//performed on A using a 2-by-2 neighborhood is returned.
cv::Mat mlab_medfilt2_2_2(cv::Mat& A);

// End Definition
#endif // FSEG_MATLAB_FUNCTIONS
