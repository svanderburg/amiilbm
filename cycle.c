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

#include "cycle.h"
#include <stdlib.h>

#define TICKS_PER_SECOND 50
#define _60_STEPS 60.0
#define MEGAPREFIX 1000000

static void shiftColorRange(amiVideo_Palette *palette, const ILBM_ColorRange *colorRange)
{
    unsigned int i;
    amiVideo_Color *color = palette->bitplaneFormat.color;
    amiVideo_Color temp = color[colorRange->low];

    for(i = colorRange->low; i < colorRange->high; i++)
	color[i] = color[i + 1];

    color[colorRange->high] = temp;
}

static void shiftDRange(amiVideo_Palette *palette, const ILBM_DRange *drange)
{
    unsigned int i;
    amiVideo_Color *color = palette->bitplaneFormat.color;
    amiVideo_Color temp = color[drange->dindex[drange->min].index];

    for(i = drange->min; i < drange->max; i++)
	color[drange->dindex[i].index] = color[drange->dindex[i + 1].index];

    color[drange->dindex[drange->max].index] = temp;
}

static void shiftCycleInfo(amiVideo_Palette *palette, const ILBM_CycleInfo *cycleInfo)
{
    amiVideo_Color *color = palette->bitplaneFormat.color;
    
    if(cycleInfo->direction == -1)
    {
	/* Shift left */
	unsigned int i;
	amiVideo_Color temp = color[cycleInfo->end];
	
	for(i = cycleInfo->end; i > cycleInfo->start; i--)
	    color[i] = color[i - 1];
	
	color[cycleInfo->start] = temp;
    }
    else if(cycleInfo->direction == 1)
    {
	/* Shift right */
	unsigned int i;
	amiVideo_Color temp = color[cycleInfo->start];
	
	for(i = cycleInfo->start; i < cycleInfo->end; i++)
	    color[i] = color[i + 1];
	
	color[cycleInfo->end] = temp;
    }
}

static unsigned int computeColorRangeTime(unsigned int ticks, const ILBM_ColorRange *colorRange)
{
    return (unsigned int)(TICKS_PER_SECOND / (_60_STEPS * colorRange->rate / ILBM_COLORRANGE_60_STEPS_PER_SECOND) + ticks);
}

static unsigned int computeDRangeTime(unsigned int ticks, const ILBM_DRange *drange)
{
    return (unsigned int)(TICKS_PER_SECOND / (_60_STEPS * drange->rate / ILBM_DRANGE_60_STEPS_PER_SECOND) + ticks);
}

static unsigned int computeCycleInfoTime(unsigned int ticks, const ILBM_CycleInfo *cycleInfo)
{
    return (unsigned int)(TICKS_PER_SECOND * cycleInfo->seconds + TICKS_PER_SECOND * cycleInfo->microSeconds / MEGAPREFIX + ticks);
}

void AMI_ILBM_initRangeTimes(AMI_ILBM_RangeTimes *rangeTimes, const ILBM_Image *image)
{
    unsigned int i;
    
    rangeTimes->crngTimes = (unsigned int*)malloc(image->colorRangeLength * sizeof(unsigned int));
    rangeTimes->drngTimes = (unsigned int*)malloc(image->drangeLength * sizeof(unsigned int));
    rangeTimes->ccrtTimes = (unsigned int*)malloc(image->cycleInfoLength * sizeof(unsigned int));
    rangeTimes->ticks = 0;
    
    for(i = 0; i < image->colorRangeLength; i++)
        rangeTimes->crngTimes[i] = computeColorRangeTime(rangeTimes->ticks, image->colorRange[i]);
    
    for(i = 0; i < image->drangeLength; i++)
        rangeTimes->drngTimes[i] = computeDRangeTime(rangeTimes->ticks, image->drange[i]);
    
    for(i = 0; i < image->cycleInfoLength; i++)
        rangeTimes->ccrtTimes[i] = computeCycleInfoTime(rangeTimes->ticks, image->cycleInfo[i]);
}

void AMI_ILBM_cleanupRangeTimes(AMI_ILBM_RangeTimes *rangeTimes)
{
    free(rangeTimes->crngTimes);
    free(rangeTimes->drngTimes);
    free(rangeTimes->ccrtTimes);
}

void AMI_ILBM_shiftActiveRanges(AMI_ILBM_RangeTimes *rangeTimes, const ILBM_Image *image, amiVideo_Palette *palette)
{
    unsigned int i;
    unsigned int *crngTimes = rangeTimes->crngTimes;
    unsigned int *drngTimes = rangeTimes->drngTimes;
    unsigned int *ccrtTimes = rangeTimes->ccrtTimes;
    
    for(i = 0; i < image->colorRangeLength; i++)
    {
	ILBM_ColorRange *colorRange = image->colorRange[i];
	
	if(colorRange->active != 0 && rangeTimes->ticks >= crngTimes[i])
	{
	    shiftColorRange(palette, colorRange);
	    crngTimes[i] = computeColorRangeTime(rangeTimes->ticks, colorRange); /* Update time */
	}
    }

    for(i = 0; i < image->drangeLength; i++)
    {
	ILBM_DRange *drange = image->drange[i];
	
	if((drange->flags & ILBM_RNG_ACTIVE) == ILBM_RNG_ACTIVE && rangeTimes->ticks >= drngTimes[i])
	{
	    shiftDRange(palette, drange);
	    drngTimes[i] = computeDRangeTime(rangeTimes->ticks, drange); /* Update time */
	}
    }
    
    for(i = 0; i < image->cycleInfoLength; i++)
    {
	ILBM_CycleInfo *cycleInfo = image->cycleInfo[i];
	
	if(cycleInfo->direction != 0 && rangeTimes->ticks >= ccrtTimes[i])
	{
	    shiftCycleInfo(palette, cycleInfo);
	    ccrtTimes[i] = computeCycleInfoTime(rangeTimes->ticks, cycleInfo); /* Update time */
	}
    }
    
    /* Update tick counter */
    rangeTimes->ticks++;
}
