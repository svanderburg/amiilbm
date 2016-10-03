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

#include "image.h"
#include <stdlib.h>
#include <libamivideo/viewportmode.h>
#include "image2amivideo.h"
#include "amivideo2bitmap.h"

#include <clib/graphics_protos.h>

#include "render.h"

static struct Screen *createIntuitionScreen(const ILBM_Image *image, const amiVideo_Long viewportMode)
{
    UWORD pens[] = { ~0 };
    
    struct Rectangle dclip = { 0, 0, image->bitMapHeader->pageWidth, image->bitMapHeader->pageHeight };
    
    struct TagItem screenTags[] = {
        {SA_Width, image->bitMapHeader->w},
        {SA_Height, image->bitMapHeader->h},
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

int AMI_ILBM_initImage(AMI_ILBM_Image *image, ILBM_Image *ilbmImage)
{
    /* Attach some properties to the facade */
    image->image = ilbmImage;
    
    /* Initialise the range times */
    AMI_ILBM_initRangeTimes(&image->rangeTimes, image->image);
    
    /* Attach the image to screen conversion pipeline */
    AMI_ILBM_attachImageToScreen(ilbmImage, &image->screen);
    
    /* Create output bitmap */
    image->bitmap = AMI_ILBM_createBitMapFromScreen(&image->screen);
    
    if(image->bitmap == NULL)
        return FALSE;
    
    /* Create output intuition screen */
    image->intuitionScreen = createIntuitionScreen(ilbmImage, image->screen.viewportMode);
    
    if(image->intuitionScreen == NULL)
        return FALSE;
    
    /* Set the screen palette */
    AMI_ILBM_setScreenPalette(&image->screen.palette, image->intuitionScreen);
    
    /* Render the output */
    AMI_ILBM_renderBitMap(ilbmImage, &image->screen, image->bitmap);
    
    return TRUE;
}

void AMI_ILBM_destroyImage(AMI_ILBM_Image *image)
{
    CloseScreen(image->intuitionScreen);
    FreeBitMap(image->bitmap);
    amiVideo_cleanupScreen(&image->screen);
    AMI_ILBM_cleanupRangeTimes(&image->rangeTimes);
}

void AMI_ILBM_cycleColors(AMI_ILBM_Image *image)
{
    AMI_ILBM_shiftActiveRanges(&image->rangeTimes, image->image, &image->screen.palette);
    AMI_ILBM_setScreenPalette(&image->screen.palette, image->intuitionScreen);
}

void AMI_ILBM_resetColors(AMI_ILBM_Image *image)
{
    AMI_ILBM_initPaletteFromImage(image->image, &image->screen.palette);
    AMI_ILBM_setScreenPalette(&image->screen.palette, image->intuitionScreen);
}

struct Window *AMI_ILBM_createWindowFromImage(const AMI_ILBM_Image *image)
{
    struct TagItem windowTags[] = {
        {WA_Width, image->image->bitMapHeader->w},
        {WA_Height, image->image->bitMapHeader->h},
        {WA_CustomScreen, image->intuitionScreen},
        {WA_SuperBitMap, image->bitmap},
        {WA_Backdrop, TRUE},
        {WA_Borderless, TRUE},
        {WA_Activate, TRUE},
        {WA_IDCMP, IDCMP_MENUPICK},
        {TAG_DONE, NULL}
    };
    
    return OpenWindowTagList(NULL, windowTags);
}
