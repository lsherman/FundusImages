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

//Converts a 2-D matrix to a matrix consisting of scaled numbers such that
//the minimum number is a zero while the maximum is a 1.
void mlab_imadjust(cv::Mat& mat);

//Make a matrix with a line of length 'length' and at an angle 'angle'.
//The line is composed of 1's and the rest are 0's.
cv::Mat mlab_strelLine(int length, float angle);

//Removes from a binary image all connected components (objects)
//that have fewer than P pixels.
//BW is a matrix of type float and will be modified.
void mlab_bwareaopen(cv::Mat& BW, int P);

// End Definition
#endif // FSEG_MATLAB_FUNCTIONS
