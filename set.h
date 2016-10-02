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

#ifndef __AMI_ILBM_SET_H
#define __AMI_ILBM_SET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AMI_ILBM_Set AMI_ILBM_Set;

#include <stdio.h>
#include <libilbm/ilbmimage.h>
#include "image.h"

/**
 * @brief An encapsulation of a set of images that originate from an IFF/ILBM file.
 */
struct AMI_ILBM_Set
{
    /** Reference to a parsed chunk originating from an IFF file */
    IFF_Chunk *chunk;
    
    /** An array extracted ILBM images from an IFF file */
    ILBM_Image **ilbmImages;
    
    /** Specifies the length of the ILBM images array */
    unsigned int imagesLength;
};

/**
 * Initializes a preallocated set by opening a file with a specified filename.
 *
 * @param set A preallocated set
 * @param filename Path to an IFF file to open
 * @return TRUE if the initialization succeeded, else FALSE
 */
int AMI_ILBM_initSetFromFilename(AMI_ILBM_Set *set, const char *filename);

/**
 * Initializes a preallocated cyclable AMI_ILBM_Image from an image in the set.
 *
 * @param set An AMI_ILBM_Set containing images
 * @param index Index of the image in the set
 * @return TRUE if the initialization succeeded, else FALSE
 */
int AMI_ILBM_initImageFromSet(const AMI_ILBM_Set *set, const unsigned int index, AMI_ILBM_Image *image);

/**
 * Clears all properties of a set from memory.
 *
 * @param set An AMI_ILBM_Set containing images
 */
void AMI_ILBM_cleanupSet(AMI_ILBM_Set *set);

#ifdef __cplusplus
}
#endif

#endif
