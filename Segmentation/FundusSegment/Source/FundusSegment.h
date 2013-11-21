/* ===========================================================================

	Project: Fundus Image Segmentation Library

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