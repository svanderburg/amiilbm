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

#include "screen.h"
#include <stdlib.h>
#include <stdio.h>

#include <graphics/gfx.h>
#include <graphics/displayinfo.h>
#include <intuition/intuition.h>

#include <clib/graphics_protos.h>

#include <libilbm/byterun.h>
#include <libilbm/interleave.h>
#include <libamivideo/screen.h>
#include <libamivideo/viewportmode.h>

void AMI_ILBM_initPaletteFromImage(const ILBM_Image *image, amiVideo_Palette *palette)
{
    amiVideo_initPalette(palette, image->bitMapHeader->nPlanes, 8, 0);
    
    if(image->colorMap == NULL)
	amiVideo_setBitplanePaletteColors(palette, NULL, 0); /* If no colormap is provided, set the palette to 0 */
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

void AMI_ILBM_setPalette(struct Screen *screen, const amiVideo_Palette *palette)
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

struct BitMap *AMI_ILBM_generateBitMap(ILBM_Image *image)
{
    IFF_UByte nPlanes = image->bitMapHeader->nPlanes;
    struct BitMap *bitmap = AllocBitMap(image->bitMapHeader->w, image->bitMapHeader->h, nPlanes, BMF_DISPLAYABLE, NULL);
    
    /* Decompress the image body */
    ILBM_unpackByteRun(image);
    
    if(ILBM_imageIsPBM(image))
    {
	amiVideo_Screen screen;
	
	/* Initialise screen width the image's dimensions, bitplane depth, and viewport mode */
	amiVideo_initScreen(&screen, image->bitMapHeader->w, image->bitMapHeader->h, image->bitMapHeader->nPlanes, 8, 0);
	
	/* A PBM has chunky pixels in its body, so set the chunky pointer to them */
	amiVideo_setScreenUncorrectedChunkyPixelsPointer(&screen, (amiVideo_UByte*)image->body->chunkData, image->bitMapHeader->w);
	
	/* Set bitplane pointers to the bitmap */
	amiVideo_setScreenBitplanePointers(&screen, (amiVideo_UByte**)bitmap->Planes);
	
	/* Do the conversion */
	amiVideo_convertScreenChunkyPixelsToBitplanes(&screen);
	
	/* Cleanup */
	amiVideo_cleanupScreen(&screen);
    }
    else
	ILBM_deinterleaveToBitplaneMemory(image, (IFF_UByte**)bitmap->Planes);
    
    /* Return bitmap */
    return bitmap;
}

struct Screen *AMI_ILBM_createScreen(const ILBM_Image *image)
{
    UWORD pens[] = { ~0 };
    
    UWORD viewportMode = AMI_ILBM_extractViewportModeFromImage(image);
    
    struct Rectangle dclip = { 0, 0, image->bitMapHeader->pageWidth, image->bitMapHeader->pageHeight };
    
    struct TagItem screenTags[] = {
        {SA_Width, image->bitMapHeader->pageWidth},
        {SA_Height, image->bitMapHeader->pageHeight},
        {SA_Depth, image->bitMapHeader->nPlanes},
        {SA_Title, "ILBM viewer"},
        {SA_Pens, pens},
        {SA_FullPalette, TRUE},
        {SA_Type, CUSTOMSCREEN},
        {SA_DisplayID, viewportMode},
        {SA_DClip, &dclip},
        {SA_AutoScroll, TRUE},
        {TAG_DONE, NULL}
    };
    
    return OpenScreenTagList(NULL, screenTags);
}

struct Window *AMI_ILBM_createWindow(ILBM_Image *image, struct Screen *screen)
{
    struct BitMap *bitmap = AMI_ILBM_generateBitMap(image);
	
    struct TagItem windowTags[] = {
        {WA_Width, image->bitMapHeader->w},
        {WA_Height, image->bitMapHeader->h},
        {WA_CustomScreen, screen},
        {WA_SuperBitMap, bitmap},
        {WA_Backdrop, TRUE},
        {WA_Borderless, TRUE},
        {WA_Activate, TRUE},
        {WA_IDCMP, IDCMP_MENUPICK},
        {TAG_DONE, NULL}
    };
    
    return OpenWindowTagList(NULL, windowTags);
}
