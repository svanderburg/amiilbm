/*
 * Copyright (c) 2012 Sander van der Burg
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Sander van der Burg <svanderburg@gmail.com>
 */

#ifndef __AMI_ILBM_CYCLE_H
#define __AMI_ILBM_CYCLE_H

typedef struct AMI_ILBM_RangeTimes AMI_ILBM_RangeTimes;

#include <libilbm/ilbmimage.h>
#include <libamivideo/palette.h>

struct AMI_ILBM_RangeTimes
{
    unsigned int *crngTimes;
    unsigned int *drngTimes;
    unsigned int *ccrtTimes;
    unsigned int ticks;
};

void AMI_ILBM_initRangeTimes(AMI_ILBM_RangeTimes *rangeTimes, const ILBM_Image *image);

void AMI_ILBM_cleanupRangeTimes(AMI_ILBM_RangeTimes *rangeTimes);

void AMI_ILBM_shiftActiveRanges(AMI_ILBM_RangeTimes *rangeTimes, const ILBM_Image *image, amiVideo_Palette *palette);

#endif
