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

#include "palette.h"
#include <exec/memory.h>
#include <graphics/displayinfo.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

int AMI_ILBM_agaIsSupported()
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

UWORD *AMI_ILBM_generate12BitPalette(const ILBM_Image *image, ULONG *colorSpecsSize)
{
    unsigned int colorRegisterLength = image->colorMap->colorRegisterLength;
    UWORD *colorSpecs;
    unsigned int i;
    
    *colorSpecsSize = colorRegisterLength * sizeof(UWORD);
    colorSpecs = (UWORD*)AllocMem(*colorSpecsSize, 0);
    
    for(i = 0; i < colorRegisterLength; i++)
    {
	ILBM_ColorRegister *colorRegister = &image->colorMap->colorRegister[i];
	colorSpecs[i] = ((colorRegister->red >> 4) << 8) | ((colorRegister->green >> 4) << 4) | (colorRegister->blue >> 4);
    }
    
    return colorSpecs;
}

ULONG *AMI_ILBM_generate24BitPalette(const ILBM_Image *image, ULONG *colorSpecsSize)
{
    unsigned int colorRegisterLength = image->colorMap->colorRegisterLength;
    ULONG *colorSpecs;
    unsigned int i;
    unsigned int index = 1;
    
    *colorSpecsSize = (colorRegisterLength * 3 + 2) * sizeof(ULONG);
    colorSpecs = (ULONG*)AllocMem(*colorSpecsSize, 0);
    
    /* First element's first word is number of colors, second word is the first color to be loaded (which is 0) */
    colorSpecs[0] = colorRegisterLength << 16;
    
    /* Remaining elements are color values */
    
    for(i = 0; i < colorRegisterLength; i++)
    {
	ILBM_ColorRegister *colorRegister = &image->colorMap->colorRegister[i];
	
	colorSpecs[index] = colorRegister->red << 24;
	index++;
	colorSpecs[index] = colorRegister->green << 24;
	index++;
	colorSpecs[index] = colorRegister->blue << 24;
	index++;
    }
    
    /* Add 0 termination at the end */
    colorSpecs[index] = 0;
    
    return colorSpecs;
}
