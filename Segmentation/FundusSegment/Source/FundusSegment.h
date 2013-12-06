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
#ifndef FSEG_FUNDUS_SEGMENT
#define FSEG_FUNDUS_SEGMENT

// Export configurations
#ifdef FUNDUSSEGMENT_EXPORTS
#   define FSEG_EXPORT __declspec(dllexport)
#else
#   define FSEG_EXPORT __declspec(dllimport)
#endif

extern "C" 
{
    /** 
     * Must be called once globally before the execution of segmentImage 
     *
     * @returns An error code, or 0 if successful
     */
    FSEG_EXPORT int startup(char const* directory);

    /** 
     * Returns a segmented version of the input image 
     *
     * @param data   [in]  The raw image data (jpeg, png, tiff, or bmp)
     * @param length [in]  The length of the input image data buffer
     * @param result [out] The output image data
     * @param bytes  [in]  The length of the output image data buffer
     *
     * @returns An error code, or 0 if successful
     */
    FSEG_EXPORT int segmentImage(void* data, size_t length, void** result, size_t* bytes);

    /** Frees memory allocated for the given image */
    FSEG_EXPORT void freeImage(void* data);

    /** Must be called before DLL unload */
    FSEG_EXPORT void shutdown();
}

// End Definition
#endif // FSEG_FUNDUS_SEGMENT