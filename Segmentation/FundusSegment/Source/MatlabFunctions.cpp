//
//  MatlabFunctions.cpp
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/8/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#include "MatlabFunctions.h"
#include "PreCalculated.h"

#define DATA_TYPE float

void mlab_imadjust(cv::Mat& mat)
{
    int rows = mat.rows;
    int cols = mat.cols;
    
    //Setup array of all matrix elements to sort.
    DATA_TYPE* sortedArray = new DATA_TYPE[rows*cols];
    int k = 0;
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            sortedArray[k] = mat.at<DATA_TYPE>(i, j);
            
            ++k;
        }
    }
    
    //Sort the array.ß
    std::sort(sortedArray, sortedArray + rows*cols);
    
    int onePercentIndex = 0.01*rows*cols;
    
    DATA_TYPE onePercentMin = sortedArray[onePercentIndex];
    DATA_TYPE onePercentMax = sortedArray[rows*cols-1-onePercentIndex];
    
    delete [] sortedArray;
    
    float diffInv = 1.0f/(onePercentMax - onePercentMin);
    
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (mat.at<DATA_TYPE>(i, j) <= onePercentMin)
            {
                mat.at<DATA_TYPE>(i, j) = 0.0;
            }
            else if (mat.at<DATA_TYPE>(i, j) >= onePercentMax)
            {
                mat.at<DATA_TYPE>(i, j) = 1.0;
            }
            else
            {
                float newValue = (mat.at<DATA_TYPE>(i, j) - onePercentMin) * diffInv;
                if (newValue > 1.0)
                {
                    newValue = 1.0;
                }
                if (newValue < 0.0)
                {
                    newValue = 0.0;
                }
                mat.at<DATA_TYPE>(i, j) = newValue;
            }
        }
    }
}

cv::Mat mlab_strelLine(int length, float angle)
{
    float effectiveAngle = fmod(angle, 360.0);
    
    if (effectiveAngle == 0.0 || effectiveAngle == 180.0 || effectiveAngle == -180.0)
    {
        return cv::Mat(1, length, CV_8U, cv::Scalar(1));
    }
    else if (effectiveAngle == 90.0 || effectiveAngle == 270.0
             || effectiveAngle == -90.0 || effectiveAngle == -270.0)
    {
        return cv::Mat(length, 1, CV_8U, cv::Scalar(1));
    }
    else if (effectiveAngle == 45.0 || effectiveAngle == 225.0)
    {
        int width = 0.778 * length;
        cv::Mat retMat(width, width, CV_8U, cv::Scalar(0));
        //Write the diagonal
        for (int i = 0; i < width; ++i)
        {
            retMat.at<UInt8>(width-1-i,i) = 1;
        }
        return retMat;
    }
    else if (effectiveAngle == -45.0 || effectiveAngle == -225.0
             || effectiveAngle == 135)
    {
        int width = 0.778 * length;
        cv::Mat retMat(width, width, CV_8U, cv::Scalar(0));
        //Write the diagonal
        for (int i = 0; i < width; ++i)
        {
            retMat.at<UInt8>(i,i) = 1;
        }
        return retMat;
    }
    else
    {
        if (fmod(angle, 15.0) == 0 && angle > 0.0 && angle <= 165.0 && length == 9)
        {
            //Use pre-calculated matrices.
            return GetStrelLength9(angle);
        }
        
        return cv::Mat(length, length, CV_8U, cv::Scalar(0));
    }
}

//Called by mlab_bwareaopen when the beginning of a component
//is found. The whole component will be removed from BW if
//it consists of less than P pixels.
void bwareaopenHelper(cv::Mat& BW, int P, bool* bwSeen, int startRowIndx, int startColIndx)
{
    std::vector<int> whiteIndicesCheckNeighborRow;
    std::vector<int> whiteIndicesCheckNeighborCol;
    std::vector<int> whiteIndicesCheckedNeighborRow;
    std::vector<int> whiteIndicesCheckedNeighborCol;
    
    int rows = BW.rows;
    int cols = BW.cols;
    
    whiteIndicesCheckNeighborRow.push_back(startRowIndx);
    whiteIndicesCheckNeighborCol.push_back(startColIndx);
    
    while (whiteIndicesCheckNeighborRow.size() > 0)
    {
        int currentRow = whiteIndicesCheckNeighborRow[whiteIndicesCheckNeighborRow.size()-1];
        int currentCol = whiteIndicesCheckNeighborCol[whiteIndicesCheckNeighborCol.size()-1];
        whiteIndicesCheckNeighborRow.pop_back();
        whiteIndicesCheckNeighborCol.pop_back();
        
        whiteIndicesCheckedNeighborRow.push_back(currentRow);
        whiteIndicesCheckedNeighborCol.push_back(currentCol);
        
        bwSeen[(currentRow)*cols+(currentCol)] = true;
        
        //Check all neighbors
        bool hasTop = currentRow > 0;
        bool hasBot = currentRow < (rows - 1);
        bool hasLeft = currentCol > 0;
        bool hasRight = currentCol < (cols - 1);
        
        if (hasTop)
        {
            if (hasLeft)
            {
                //Top-Left
                if (bwSeen[(currentRow-1)*cols+(currentCol-1)] == false
                    && BW.at<DATA_TYPE>(currentRow-1, currentCol-1) == 1.0)
                {
                    whiteIndicesCheckNeighborRow.push_back(currentRow-1);
                    whiteIndicesCheckNeighborCol.push_back(currentCol-1);
                }
                bwSeen[(currentRow-1)*cols+(currentCol-1)] = true;
            }
            
            //Top
            if (bwSeen[(currentRow-1)*cols+(currentCol)] == false
                && BW.at<DATA_TYPE>(currentRow-1, currentCol) == 1.0)
            {
                whiteIndicesCheckNeighborRow.push_back(currentRow-1);
                whiteIndicesCheckNeighborCol.push_back(currentCol);
            }
            bwSeen[(currentRow-1)*cols+(currentCol)] = true;
            
            if (hasRight)
            {
                //Top-Right
                if (bwSeen[(currentRow-1)*cols+(currentCol+1)] == false
                    && BW.at<DATA_TYPE>(currentRow-1, currentCol+1) == 1.0)
                {
                    whiteIndicesCheckNeighborRow.push_back(currentRow-1);
                    whiteIndicesCheckNeighborCol.push_back(currentCol+1);
                }
                bwSeen[(currentRow-1)*cols+(currentCol+1)] = true;
            }
        }
        
        if (hasBot)
        {
            if (hasLeft)
            {
                //Bot-Left
                if (bwSeen[(currentRow+1)*cols+(currentCol-1)] == false
                    && BW.at<DATA_TYPE>(currentRow+1, currentCol-1) == 1.0)
                {
                    whiteIndicesCheckNeighborRow.push_back(currentRow+1);
                    whiteIndicesCheckNeighborCol.push_back(currentCol-1);
                }
                bwSeen[(currentRow+1)*cols+(currentCol-1)] = true;
            }
            
            //Bot
            if (bwSeen[(currentRow+1)*cols+(currentCol)] == false
                && BW.at<DATA_TYPE>(currentRow+1, currentCol) == 1.0)
            {
                whiteIndicesCheckNeighborRow.push_back(currentRow+1);
                whiteIndicesCheckNeighborCol.push_back(currentCol);
            }
            bwSeen[(currentRow+1)*cols+(currentCol)] = true;
            
            if (hasRight)
            {
                //Bot-Right
                if (bwSeen[(currentRow+1)*cols+(currentCol+1)] == false
                    && BW.at<DATA_TYPE>(currentRow+1, currentCol+1) == 1.0)
                {
                    whiteIndicesCheckNeighborRow.push_back(currentRow+1);
                    whiteIndicesCheckNeighborCol.push_back(currentCol+1);
                }
                bwSeen[(currentRow+1)*cols+(currentCol+1)] = true;
            }
        }
        
        if (hasLeft)
        {
            //Left
            if (bwSeen[(currentRow)*cols+(currentCol-1)] == false
                && BW.at<DATA_TYPE>(currentRow, currentCol-1) == 1.0)
            {
                whiteIndicesCheckNeighborRow.push_back(currentRow);
                whiteIndicesCheckNeighborCol.push_back(currentCol-1);
            }
            bwSeen[(currentRow)*cols+(currentCol-1)] = true;
        }
        
        if (hasRight)
        {
            //Right
            if (bwSeen[(currentRow)*cols+(currentCol+1)] == false
                && BW.at<DATA_TYPE>(currentRow, currentCol+1) == 1.0)
            {
                whiteIndicesCheckNeighborRow.push_back(currentRow);
                whiteIndicesCheckNeighborCol.push_back(currentCol+1);                
            }
            bwSeen[(currentRow)*cols+(currentCol+1)] = true;
        }
    }
    
    //Check how many pixels were a part of this component
    //and if fewer than P pixels, remove them from BW.
    if (whiteIndicesCheckedNeighborRow.size() < P)
    {
        while (whiteIndicesCheckedNeighborRow.size() > 0)
        {
            int currentRow = whiteIndicesCheckedNeighborRow[whiteIndicesCheckedNeighborRow.size()-1];
            int currentCol = whiteIndicesCheckedNeighborCol[whiteIndicesCheckedNeighborCol.size()-1];
            whiteIndicesCheckedNeighborRow.pop_back();
            whiteIndicesCheckedNeighborCol.pop_back();
            
            BW.at<DATA_TYPE>(currentRow,currentCol) = 0.0;
        }
    }
}

void mlab_bwareaopen(cv::Mat& BW, int P)
{
    int rows = BW.rows;
    int cols = BW.cols;
    int totalPixels = rows*cols;
    
    bool* bwSeen = new bool[totalPixels];
    for (int i = 0; i < totalPixels; ++i)
    {
        bwSeen[i] = false;
    }
    
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            int currentIndex = i*cols + j;
            if (bwSeen[currentIndex] == false)
            {
                if (BW.at<DATA_TYPE>(i,j) == 1.0)
                {
                    //Found the beginning of a component
                    bwareaopenHelper(BW, P, bwSeen, i, j);
                }
                else
                {
                    bwSeen[currentIndex] = true;
                }
            }
        }
    }
    
    delete [] bwSeen;
}