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

#include "viewer.h"
#include <stdlib.h>
#include "filepicker.h"
#include "set.h"
#include "viewerdisplay.h"

static AMI_ILBM_Action viewILBMImage(AMI_ILBM_Set *set, const unsigned int number, char **filename, const int previousItemEnabled, const amiVideo_Bool nextItemEnabled)
{
    AMI_ILBM_Action action;

    /* Initialise a viewer display and handle all user inputs */
    if(AMI_ILBM_initViewerDisplay(set, number, previousItemEnabled, nextItemEnabled))
        action = AMI_ILBM_handleScreenActions(filename);
    else
        action = ACTION_ERROR;

    /* Cleanup */
    AMI_ILBM_destroyViewerDisplay();

    /* Return action */
    return action;
}

amiVideo_Bool AMI_ILBM_viewImages(char *filename)
{
    char *aslFilename = NULL;
    AMI_ILBM_Action action;

    do /* Repeat this every time the user picks the 'Open file' option */
    {
        AMI_ILBM_Set set;

        if(!AMI_ILBM_initSetFromFilename(&set, filename))
        {
            fprintf(stderr, "Cannot open IFF file! It may be invalid or missing!\n");
            action = ACTION_ERROR;
        }
        else if(set.imagesLength == 0)
        {
            fprintf(stderr, "IFF file does not contain any ILBM images!\n");
            action = ACTION_ERROR;
        }
        else
        {
            unsigned int number = 0;

            do /* Repeat this every time the user picks the 'Previous' or the 'Next' option */
            {
                action = viewILBMImage(&set, number, &aslFilename, number > 0, number < set.imagesLength - 1);
                filename = aslFilename;

                /* If the user has picked 'Previous' or 'Next' we should modify the imageNumber counter */

                if(action == ACTION_PREVIOUS)
                    number--;
                else if(action == ACTION_NEXT)
                    number++;
            }
            while(action == ACTION_PREVIOUS || action == ACTION_NEXT);
        }

        /* Cleanup */
        AMI_ILBM_cleanupSet(&set);
    }
    while(action == ACTION_OPEN);

    /* Check whether we have an error or not */
    return (action == ACTION_ERROR);
}
