//
//  MatlabFunctions.cpp
//  FudusSeg_IOS
//
//  Created by Niraj Rayalla on 11/8/13.
//  Copyright (c) 2013 Niraj Rayalla. All rights reserved.
//

#include "MatlabFunctions.h"
#include "PreCalculated.h"

cv::Mat mlab_im2double(cv::Mat& mat)
{
    int type = mat.type();
    
    //If already a double matrix return it.
    if (type == CV_64FC1)
    {
        return mat;
    }
    
    int rows = mat.rows;
    int cols = mat.cols;
    cv::Mat newMat(rows, cols, CV_64FC1);
    
    //If an 8bit unsigned interger matrix, convert to double, scale and return it.
    //Otherwise return the conversion to double- only.
    if (type == CV_8UC1)
    {
        double scaleFactor = 1.0/255.0;
        
        for(int i = 0; i < rows; ++i)
        {
            for(int j = 0; j < cols; ++j)
            {
                newMat.at<double>(i, j) = (double)mat.at<UInt8>(i, j) * scaleFactor;
            }
        }
    }
    else
    {
        for(int i = 0; i < rows; ++i)
        {
            for(int j = 0; j < cols; ++j)
            {
                mat.convertTo(newMat, type);
            }
        }
    }
    
    return newMat;
}

void mlab_imadjust(cv::Mat& mat)
{
    int rows = mat.rows;
    int cols = mat.cols;
    
    //Setup array of all matrix elements to sort.
    double* sortedArray = new double[rows*cols];
    int k = 0;
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            sortedArray[k] = mat.at<double>(i, j);
            
            ++k;
        }
    }
    
    //Sort the array.ß
    std::sort(sortedArray, sortedArray + rows*cols);
    
    int onePercentIndex = 0.01*rows*cols;
    
    double onePercentMin = sortedArray[onePercentIndex];
    double onePercentMax = sortedArray[rows*cols-1-onePercentIndex];
    
    delete [] sortedArray;
    
    double diffInv = 1.0f/(onePercentMax - onePercentMin);
    
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (mat.at<double>(i, j) <= onePercentMin)
            {
                mat.at<double>(i, j) = 0.0;
            }
            else if (mat.at<double>(i, j) >= onePercentMax)
            {
                mat.at<double>(i, j) = 1.0;
            }
            else
            {
                double newValue = (mat.at<double>(i, j) - onePercentMin) * diffInv;
                if (newValue > 1.0)
                {
                    newValue = 1.0;
                }
                if (newValue < 0.0)
                {
                    newValue = 0.0;
                }
                mat.at<double>(i, j) = newValue;
            }
        }
    }
}

cv::Mat mlab_fspecialAverage(int width, int height)
{
    double avgVal = 1.0/((double)width * (double)height);
    
    return cv::Mat(height, width, CV_64FC1, cv::Scalar(avgVal));
}

cv::Mat mlab_filterReplicate(cv::Mat src, cv::Mat kernel)
{
    cv::Point anchor( -1,-1);
    float delta = 0.0;
    cv::Mat dest;
    cv::filter2D(src, dest, CV_64FC1, kernel, anchor, delta, cv::BORDER_REPLICATE);
    
    return dest;
}

cv::Mat mlab_strelLine(int length, double angle)
{
    double effectiveAngle = fmod(angle, 360.0);
    
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

cv::Mat mlab_imtophat(cv::Mat& IM, cv::Mat& SE)
{
    return IM-mlab_imopen(IM, SE);
}

cv::Mat mlab_imopen(cv::Mat& IM, cv::Mat& SE)
{
    cv::Mat result;
    cv::erode(IM, result, SE);
    cv::dilate(result, result, SE);
    
    return result;
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
                    && BW.at<double>(currentRow-1, currentCol-1) == 1.0)
                {
                    whiteIndicesCheckNeighborRow.push_back(currentRow-1);
                    whiteIndicesCheckNeighborCol.push_back(currentCol-1);
                }
                bwSeen[(currentRow-1)*cols+(currentCol-1)] = true;
            }
            
            //Top
            if (bwSeen[(currentRow-1)*cols+(currentCol)] == false
                && BW.at<double>(currentRow-1, currentCol) == 1.0)
            {
                whiteIndicesCheckNeighborRow.push_back(currentRow-1);
                whiteIndicesCheckNeighborCol.push_back(currentCol);
            }
            bwSeen[(currentRow-1)*cols+(currentCol)] = true;
            
            if (hasRight)
            {
                //Top-Right
                if (bwSeen[(currentRow-1)*cols+(currentCol+1)] == false
                    && BW.at<double>(currentRow-1, currentCol+1) == 1.0)
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
                    && BW.at<double>(currentRow+1, currentCol-1) == 1.0)
                {
                    whiteIndicesCheckNeighborRow.push_back(currentRow+1);
                    whiteIndicesCheckNeighborCol.push_back(currentCol-1);
                }
                bwSeen[(currentRow+1)*cols+(currentCol-1)] = true;
            }
            
            //Bot
            if (bwSeen[(currentRow+1)*cols+(currentCol)] == false
                && BW.at<double>(currentRow+1, currentCol) == 1.0)
            {
                whiteIndicesCheckNeighborRow.push_back(currentRow+1);
                whiteIndicesCheckNeighborCol.push_back(currentCol);
            }
            bwSeen[(currentRow+1)*cols+(currentCol)] = true;
            
            if (hasRight)
            {
                //Bot-Right
                if (bwSeen[(currentRow+1)*cols+(currentCol+1)] == false
                    && BW.at<double>(currentRow+1, currentCol+1) == 1.0)
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
                && BW.at<double>(currentRow, currentCol-1) == 1.0)
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
                && BW.at<double>(currentRow, currentCol+1) == 1.0)
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
            
            BW.at<double>(currentRow,currentCol) = 0.0;
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
                if (BW.at<double>(i,j) == 1.0)
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

cv::Mat mlab_medfilt2_2_2(cv::Mat& A)
{
    cv::Mat medFilt = A.clone();
    
    int rows = A.rows;
    int cols = A.cols;
    
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            std::vector<double> medianFinder;
            
            //Find the values in the 2-by-2 neighboorhood
            //top-left
            medianFinder.push_back(A.at<double>(i,j));
            if (j+1 >= cols)
            {
                //top-right
                medianFinder.push_back(0.0);
                //bottom-right
                medianFinder.push_back(0.0);
            }
            else
            {
                //top-right
                medianFinder.push_back(A.at<double>(i,j+1));
                if (i+1 >= rows)
                {
                    //bottom-right
                    medianFinder.push_back(A.at<double>(i+1,j+1));
                }
                else
                {
                    //bottom-right
                    medianFinder.push_back(0.0);
                }
            }
            if (i+1 >= rows)
            {
                //bottom-left
                medianFinder.push_back(A.at<double>(i+1,j));
            }
            else
            {
                //bottom-left
                medianFinder.push_back(0.0);
            }
            
            std::sort(medianFinder.begin(), medianFinder.end());
            int size = medianFinder.size();
            if (size % 2 == 0)
            {
                medFilt.at<double>(i,j) = 0.5*(medianFinder[size/2] + medianFinder[(size/2) + 1]);
            }
            else
            {
                medFilt.at<double>(i,j) = medianFinder[size/2];
            }
        }
    }
    
    return medFilt;
}