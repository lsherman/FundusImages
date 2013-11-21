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

//Passed in matrix must be of type double.
//Modifies passed in matrix.
void enhancer(cv::Mat& mat, float t);

//Passed in matrix must be of type double.
//Modifies passed in matrix.
void enhancer_new(cv::Mat& mat, float t);

#endif /* defined(__FudusSeg_IOS__OtherFunctions__) */
