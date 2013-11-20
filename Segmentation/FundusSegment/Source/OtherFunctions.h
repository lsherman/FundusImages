//
//  OtherFunctions.h
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/8/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#ifndef __FudusSeg_IOS__OtherFunctions__
#define __FudusSeg_IOS__OtherFunctions__

#include <iostream>

#include "MatlabFunctions.h"

//Do an element-wise power of the matrix.
//Passed in matrix must be of type double.
//Modifies passed in matrix.
void cvMatElementwisePower(cv::Mat& mat, double t);

//Passed in matrix must be of type double.
//Modifies passed in matrix.
void enhancer(cv::Mat& mat, double t);

//Passed in matrix must be of type double.
//Modifies passed in matrix.
void enhancer_new(cv::Mat& mat, double t);

#endif /* defined(__FudusSeg_IOS__OtherFunctions__) */
