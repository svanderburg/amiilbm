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

#include "set.h"
#include <stdlib.h>
#include <libilbm/ilbm.h>

amiVideo_Bool AMI_ILBM_initSetFromFilename(AMI_ILBM_Set *set, const char *filename)
{
    IFF_Chunk *chunk = ILBM_read(filename);

    set->chunk = chunk;
    set->ilbmImages = ILBM_extractImages(chunk, &set->imagesLength);

    return ILBM_checkImages(chunk, set->ilbmImages, set->imagesLength);
}

amiVideo_Bool AMI_ILBM_initImageFromSet(const AMI_ILBM_Set *set, const unsigned int index, AMI_ILBM_Image *image)
{
    if(index < set->imagesLength)
        return AMI_ILBM_initImage(image, set->ilbmImages[index]);
    else
        return FALSE;
}

void AMI_ILBM_cleanupSet(AMI_ILBM_Set *set)
{
    ILBM_freeImages(set->ilbmImages, set->imagesLength);
    ILBM_free(set->chunk);
}
