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

#include "filepicker.h"

#include <stdlib.h>
#include <string.h>

#include <libraries/asl.h>
#include <clib/asl_protos.h>

char *AMI_ILBM_openILBMFile(struct Window *window, char *initialDrawer)
{
    char *filename;

    struct TagItem requestTags[] = {
        {ASLFR_Window, window},
        {ASLFR_TitleText, "Pick an ILBM file"},
        {ASLFR_InitialDrawer, initialDrawer},
        {ASLFR_RejectIcons, TRUE},
        {TAG_DONE, NULL}
    };

    /* Open requester */
    struct FileRequester *requester = (struct FileRequester*)AllocAslRequest(ASL_FileRequest, requestTags);

    if(AslRequest(requester, 0))
    {
        /* Create filename path */
        filename = (char*)malloc((strlen(requester->fr_Drawer) + 1 + strlen(requester->fr_File) + 1) * sizeof(char));

        /* Append drawer value */
        strcat(filename, requester->fr_Drawer);

        /* If the drawer is not an assignment, add a / */
        if(requester->fr_Drawer[strlen(requester->fr_Drawer) - 1] != ':')
            strcat(filename, "/");

        /* Append the filename */
        strcat(filename, requester->fr_File);
    }
    else
        filename = NULL;

    /* Cleanup */
    FreeAslRequest(requester);

    /* Return filename */
    return filename;
}
