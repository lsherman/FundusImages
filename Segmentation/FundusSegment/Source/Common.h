/* ===========================================================================

	Project: Fundus Image Segmentation DLL

	Includes common headers used by most classes. Also defines DLL export
    and import signals.

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

// Begin definition
#ifndef FSEG_COMMON_H
#define FSEG_COMMON_H

// Export configurations
#ifdef FundusLib_EXPORTS
#   define FSEG_EXPORT __declspec(dllexport)
#else
#   define FSEG_EXPORT __declspec(dllimport)
#endif

// VS2012 limitations
#define _VARIADIC_MAX 10

// <math.h> Include Macro
#define _USE_MATH_DEFINES

// Raw image data type
typedef unsigned char UInt8;

// OpenCV Headers
#include "opencv/cv.h"
#include "opencv/cvwimage.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

// STD Standard Includes
#include <algorithm>
#include <deque>
#include <exception>
#include <functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// End definition
#endif // FSEG_COMMON_H