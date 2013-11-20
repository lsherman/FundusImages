//
//  FundusSegmenter.h
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/7/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#ifndef FudusSeg_IOS_FundusSegmenter_h
#define FudusSeg_IOS_FundusSegmenter_h

#include "MatlabFunctions.h"
#include "OtherFunctions.h"

class FundusSegmenter
{
private:
    cv::Mat g;
    cv::Mat gDouble;
    int gRows;
    int gCols;
    
    cv::Mat g0;
    cv::Mat g0Double;
    int g0Rows;
    int g0Cols;
    
public:
    FundusSegmenter();
    
    void* SegmentImage(void* data, size_t width, size_t height);
};


#endif
