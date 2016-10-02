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

#include "render.h"
#include <string.h>
#include <libilbm/interleave.h>

void AMI_ILBM_renderBitMap(const ILBM_Image *image, amiVideo_Screen *screen, struct BitMap *bitmap)
{
    if(ILBM_imageIsPBM(image))
    {
        if(image->body != NULL)
            amiVideo_convertScreenChunkyPixelsToBitplanes(screen); /* Do the conversion */
    }
    else if(ILBM_imageIsILBM(image))
        ILBM_deinterleaveToBitplaneMemory(image, (IFF_UByte**)bitmap->Planes);
    else if(ILBM_imageIsACBM(image))
    {
        if(image->bitplanes != NULL)
        {
            unsigned int bitplaneSize = ILBM_calculateRowSize(image) * image->bitMapHeader->h;
            unsigned int i, offset = 0;
        
            for(i = 0; i < screen->bitplaneDepth; i++)
            {
                memcpy(bitmap->Planes[i], image->bitplanes->chunkData + offset, bitplaneSize);
                offset += bitplaneSize;
            }
        }
    }
}
