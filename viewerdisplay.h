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

#ifndef __AMI_ILBM_VIEWERDISPLAY_H
#define __AMI_ILBM_VIEWERDISPLAY_H

#include <libilbm/ilbmimage.h>
#include <exec/types.h>
#include "set.h"
#include "image.h"
#include "filepicker.h"

typedef enum
{
    ACTION_OPEN,
    ACTION_PREVIOUS,
    ACTION_NEXT,
    ACTION_QUIT,
    ACTION_ERROR
}
AMI_ILBM_Action;

typedef struct
{
    AMI_ILBM_Image image;
    struct Window *window;
    struct Menu *menu;
    struct Task *task;
    int cycleTaskMustStop;
    int cycleModeEnabled;
    int cycleTaskTerminated;
}
AMI_ILBM_ViewerDisplay;

/**
 * Viewer display is a singleton instance that can be referenced both from the
 * program as well as the cycle task
 */
AMI_ILBM_ViewerDisplay viewerDisplay;

extern void AMI_ILBM_cycleTask();

int AMI_ILBM_initViewerDisplay(AMI_ILBM_Set *set, const unsigned int number, const int previousItemEnabled, const int nextItemEnabled);

void AMI_ILBM_destroyViewerDisplay(void);

AMI_ILBM_Action AMI_ILBM_handleScreenActions(char **filename);

#endif
