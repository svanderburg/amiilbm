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
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include <libilbm/byterun.h>
#include <libilbm/interleave.h>

#include <libamivideo/bitplane.h>

#include "palette.h"

struct Screen *AMI_ILBM_createScreen(const ILBM_Image *image)
{
    UWORD pens[] = { ~0 };
    
    UWORD viewportMode = AMI_ILBM_calculateViewPortMode(image);
    
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

void AMI_ILBM_setPalette(struct Screen *screen, const ILBM_Image *image)
{
    if(AMI_ILBM_agaIsSupported())
    {
        ULONG colorSpecsSize;
        ULONG *colorSpecs = AMI_ILBM_generate24BitPalette(image, &colorSpecsSize);

        LoadRGB32(&screen->ViewPort, colorSpecs);

        FreeMem(colorSpecs, colorSpecsSize);
    }
    else
    {
        ULONG colorSpecsSize;
        UWORD *colorSpecs = AMI_ILBM_generate12BitPalette(image, &colorSpecsSize);

        LoadRGB4(&screen->ViewPort, (UWORD*)colorSpecs, image->colorMap->colorRegisterLength);

        FreeMem(colorSpecs, colorSpecsSize);
    }
}

struct BitMap *AMI_ILBM_generateBitMap(ILBM_Image *image)
{
    struct BitMap *bitmap;
    IFF_UByte nPlanes = image->bitMapHeader->nPlanes;
    
    ILBM_unpackByteRun(image);
    
    bitmap = AllocBitMap(image->bitMapHeader->w, image->bitMapHeader->h, nPlanes, BMF_DISPLAYABLE, NULL);
    
    if(ILBM_imageIsPBM(image))
	amiVideo_chunkyToBitplaneMemory((amiVideo_UByte**)bitmap->Planes, (amiVideo_UByte*)image->body->chunkData, image->bitMapHeader->w, image->bitMapHeader->h, nPlanes);
    else
	ILBM_deinterleaveToBitplaneMemory(image, (IFF_UByte**)bitmap->Planes);
    
    /* Return bitmap */
    return bitmap;
}

UWORD AMI_ILBM_calculateViewPortMode(const ILBM_Image *image)
{
    UWORD viewportMode;
    
    if(image->viewport == NULL)
	viewportMode = 0; /* If there is no viewport chunk, we shouldn't do anything */
    else
	viewportMode = image->viewport->viewportMode & (HAM | EXTRA_HALFBRITE); /* Only take EHB and HAM flags from the ILBM viewport chunk */

    /* If the page width is larger than 368 (320 width + max overscan), we use the hi-res screen mode */
    if(image->bitMapHeader->pageWidth > 368)
	viewportMode |= HIRES;

    /* If the page height is larger than 290 (256 height + max overscan), we have a laced screen mode */
    if(image->bitMapHeader->pageHeight > 290)
	viewportMode |= LACE;

    return viewportMode;
}

