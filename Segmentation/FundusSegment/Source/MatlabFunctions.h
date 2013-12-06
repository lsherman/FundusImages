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

// Begin Definition
#ifndef FSEG_MATLAB_FUNCTIONS_H
#define FSEG_MATLAB_FUNCTIONS_H

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
#endif // FSEG_MATLAB_FUNCTIONS_H
