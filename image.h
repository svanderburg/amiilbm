/*
 * Copyright (c) 2012 Sander van der Burg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __AMI_ILBM_IMAGE_H
#define __AMI_ILBM_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AMI_ILBM_Image AMI_ILBM_Image;

#include <libilbm/ilbmimage.h>
#include <libamivideo/screen.h>
#include <exec/types.h>
#include <clib/intuition_protos.h>

#include "cycle.h"

/**
 * @brief Encapsulates the properties of a cyclable image
 */
struct AMI_ILBM_Image
{
    /** Reference to the ILBM image from which this image is generated */
    ILBM_Image *image;

    /** Reference to the screen converting the image to the desired output format */
    amiVideo_Screen screen;

    /** Bitmap encoding the pixels as bitplanes */
    struct BitMap *bitmap;

    /** Intuition screen suitable for displaying the image */
    struct Screen *intuitionScreen;

    /** Maintains the ellapsed cycle range times */
    AMI_ILBM_RangeTimes rangeTimes;
};

/**
 * Initializes a preallocated AMI_ILBM_Image from a given ILBM image in a specified
 * output format.
 *
 * @param image Preallocated AMI_ILBM_Image instance
 * @param ilbmImage ILBM image to generate the output from
 * @return TRUE if the initalisation succeeded, otherwise FALSE
 */
amiVideo_Bool AMI_ILBM_initImage(AMI_ILBM_Image *image, ILBM_Image *ilbmImage);

/**
 * Clears the properties of the provided image from memory.
 *
 * @param image An AMI_ILBM_Image instance
 */
void AMI_ILBM_destroyImage(AMI_ILBM_Image *image);

/**
 * Checks whether the range times have ellapsed and cycles the
 * corresponding colors in the palette accordingly.
 *
 * @param image An AMI_ILBM_Image instance
 */
void AMI_ILBM_cycleColors(AMI_ILBM_Image *image);

/**
 * Resets the colors in the palette back to normal.
 *
 * @param image An AMI_ILBM_Image instance
 */
void AMI_ILBM_resetColors(AMI_ILBM_Image *image);

/**
 * Constructs a window suitable for displaying a given image
 *
 * @param image An AMI_ILBM_Image instance
 */
struct Window *AMI_ILBM_createWindowFromImage(const AMI_ILBM_Image *image);

#ifdef __cplusplus
}
#endif

#endif
