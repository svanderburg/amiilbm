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

#include "image2amivideo.h"
#include <stdlib.h>
#include <libilbm/byterun.h>
#include <libilbm/interleave.h>
#include <libamivideo/viewportmode.h>

void AMI_ILBM_initPaletteFromImage(const ILBM_Image *image, amiVideo_Palette *palette)
{
    amiVideo_initPalette(palette, image->bitMapHeader->nPlanes, 8, 0);

    if(image->colorMap == NULL)
    {
        /* If no colormap is provided by the image, use a generated grayscale one */
        ILBM_ColorMap *colorMap = ILBM_generateGrayscaleColorMap(image);
        amiVideo_setBitplanePaletteColors(palette, (amiVideo_Color*)colorMap->colorRegister, colorMap->colorRegisterLength);
        free(colorMap);
    }
    else
        amiVideo_setBitplanePaletteColors(palette, (amiVideo_Color*)image->colorMap->colorRegister, image->colorMap->colorRegisterLength);
}

amiVideo_ULong AMI_ILBM_extractViewportModeFromImage(const ILBM_Image *image)
{
    amiVideo_ULong paletteFlags, resolutionFlags;

    if(image->viewport == NULL)
        paletteFlags = 0; /* If no viewport value is set, assume 0 value */
    else
        paletteFlags = amiVideo_extractPaletteFlags(image->viewport->viewportMode); /* Only the palette flags can be considered "reliable" from a viewport mode value */

    /* Resolution flags are determined by looking at the page dimensions */
    resolutionFlags = amiVideo_autoSelectViewportMode(image->bitMapHeader->pageWidth, image->bitMapHeader->pageHeight);

    /* Return the combined settings of the previous */
    return paletteFlags | resolutionFlags;
}

void AMI_ILBM_attachImageToScreen(ILBM_Image *image, amiVideo_Screen *screen)
{
    /* Determine which viewport mode is best for displaying the image */
    IFF_Long viewportMode = AMI_ILBM_extractViewportModeFromImage(image);

    /* Initialize the screen with the image's dimensions, bitplane depth, and viewport mode */
    amiVideo_initScreen(screen, image->bitMapHeader->w, image->bitMapHeader->h, image->bitMapHeader->nPlanes, 8, viewportMode);

    /* Sets the colors of the palette */
    AMI_ILBM_initPaletteFromImage(image, &screen->palette);

    /* Decompress the image body */
    ILBM_unpackByteRun(image);

    /* Attach the appropriate pixel surface to the screen */
    if(ILBM_imageIsPBM(image))
        amiVideo_setScreenUncorrectedChunkyPixelsPointer(screen, (amiVideo_UByte*)image->body->chunkData, image->bitMapHeader->w); /* A PBM has chunky pixels in its body */
    else if(ILBM_imageIsACBM(image))
        amiVideo_setScreenBitplanes(screen, (amiVideo_UByte*)image->bitplanes->chunkData); /* Set bitplane pointers of the conversion screen */
    else if(ILBM_imageIsILBM(image))
    {
        if(ILBM_convertILBMToACBM(image)) /* Amiga ILBM image has interleaved scanlines per bitplane. We have to deinterleave it in order to be able to convert it */
            amiVideo_setScreenBitplanes(screen, (amiVideo_UByte*)image->bitplanes->chunkData); /* Set bitplane pointers of the conversion screen */
    }
}
