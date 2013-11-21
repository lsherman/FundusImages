/* ===========================================================================

	Project: Fundus Image Segmentation Library

    Performs unit testing for the image segmentation library

    Copyright (C) 2013

	Lucas Sherman, email: LucasASherman@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
    startup("./");

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
    std::ofstream of("outputImage.jpg", std::ios::binary);
    of.write((char*)outputImage, bytes);
    of.close();

    freeImage(outputImage);

    shutdown();

	return 0;
}

