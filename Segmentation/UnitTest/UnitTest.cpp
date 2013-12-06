/* ===========================================================================

	Project: Fundus Image Segmentation Library

    Performs unit testing for the image segmentation library

    Copyright (C) 2013

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

// Include Header
#include "FundusSegment.h"

// STD Standard Includes
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char* argv[])
{
    std::string directory("./");
    startup(directory.c_str());

    // Load the test image from memory
    std::ifstream is("testImage.png", std::ios_base::binary | std::ios::ate);
    auto length = is.tellg();
    is.seekg(std::ios_base::beg);
    auto image = new char[length];
    is.read(image, length);
    is.close();

    // Perform the segmentation
    void * outputImage; size_t bytes;
    auto error = segmentImage(image, length, &outputImage, &bytes);
    if (error) return 1;
    delete[] image;

    // Write the output back to disk
    std::ofstream of("outputImage.png", std::ios::binary);
    of.write((char*)outputImage, bytes);
    of.close();

    freeImage(outputImage);

    shutdown();

	return 0;
}

