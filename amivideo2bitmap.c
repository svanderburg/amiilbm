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

#include "amivideo2bitmap.h"
#include <stdlib.h>
#include <stdio.h>

#include <graphics/gfx.h>
#include <graphics/displayinfo.h>
#include <intuition/intuition.h>

#include <clib/graphics_protos.h>

amiVideo_Bool AMI_ILBM_agaIsSupported()
{
    struct DisplayInfo displayInfo;

    if(GetDisplayInfoData(NULL, (UBYTE*)&displayInfo, sizeof(struct DisplayInfo), DTAG_DISP, NULL))
        return (displayInfo.RedBits == 8);
    else
    {
        fprintf(stderr, "WARNING: No display info found! Assuming we have an ECS chipset!\n");
        return FALSE;
    }
}

void AMI_ILBM_setScreenPalette(const amiVideo_Palette *palette, struct Screen *screen)
{
    if(AMI_ILBM_agaIsSupported())
    {
        amiVideo_ULong *colorSpecs = amiVideo_generateRGB32ColorSpecs(palette);
        LoadRGB32(&screen->ViewPort, (ULONG*)colorSpecs);
        free(colorSpecs);
    }
    else
    {
        amiVideo_UWord *colorSpecs = amiVideo_generateRGB4ColorSpecs(palette);
        LoadRGB4(&screen->ViewPort, (UWORD*)colorSpecs, palette->bitplaneFormat.numOfColors);
        free(colorSpecs);
    }
}

struct BitMap *AMI_ILBM_createBitMapFromScreen(amiVideo_Screen *screen)
{
    struct BitMap *bitmap = AllocBitMap(screen->width, screen->height, screen->bitplaneDepth, BMF_DISPLAYABLE, NULL);

    if(bitmap != NULL)
        amiVideo_setScreenBitplanePointers(screen, (amiVideo_UByte**)bitmap->Planes); /* Set bitplane pointers to the bitmap */

    return bitmap;
}
