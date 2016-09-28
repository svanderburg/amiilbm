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

#include <exec/types.h>
#include <clib/intuition_protos.h>

#include <libilbm/ilbmimage.h>
#include <libamivideo/palette.h>

void AMI_ILBM_initPaletteFromImage(const ILBM_Image *image, amiVideo_Palette *palette);

amiVideo_ULong AMI_ILBM_extractViewportModeFromImage(const ILBM_Image *image);

int AMI_ILBM_agaIsSupported();

void AMI_ILBM_setPalette(struct Screen *screen, const amiVideo_Palette *palette);

struct BitMap *AMI_ILBM_generateBitMap(ILBM_Image *image);

struct Screen *AMI_ILBM_createScreen(const ILBM_Image *image);

struct Window *AMI_ILBM_createWindow(ILBM_Image *image, struct Screen *screen);

#endif
