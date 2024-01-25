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

#include "viewerdisplay.h"

#include <exec/memory.h>
#include <exec/tasks.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/gadtools_protos.h>

static struct Menu *createViewerDisplayMenu(struct Screen *screen, const amiVideo_Bool previousItemEnabled, const amiVideo_Bool nextItemEnabled)
{
    /* Get some visual info required to properly generate the menu layout */
    APTR vi = GetVisualInfo(screen, TAG_END);

    if(vi == NULL)
    {
        fprintf(stderr, "Cannot obtain visual info!\n");
        return NULL;
    }
    else
    {
        /* Create a menu */

        struct NewMenu newMenu[] = {
            {NM_TITLE, "Picture", 0, 0, 0, 0},
            {NM_ITEM, "Open...", "O", 0, 0, 0},
            {NM_ITEM, "Previous", "P", previousItemEnabled ? 0 : NM_ITEMDISABLED, 0, 0},
            {NM_ITEM, "Next", "N", nextItemEnabled ? 0 : NM_ITEMDISABLED, 0, 0},
            {NM_ITEM, "Hide Title", "S", CHECKIT | MENUTOGGLE, 0, 0},
            {NM_ITEM, "Cycle", "C", CHECKIT | MENUTOGGLE, 0, 0},
            {NM_ITEM, "Quit", "Q", 0, 0, 0},
            {NM_END, NULL, 0, 0, 0, 0}
        };

        struct Menu *menu = CreateMenus(newMenu, GTMN_FullMenu, TRUE, TAG_END);

        if(menu != NULL)
        {
            /* Properly layout the menu items */
            if(!LayoutMenus(menu, vi, TAG_END))
            {
                fprintf(stderr, "Cannot calculate menu layout!\n");
                FreeMenus(menu);
                menu = NULL;
            }
        }

        FreeVisualInfo(vi); /* Cleanup */

        return menu; /* Return generated menu */
    }
}

void AMI_ILBM_cycleTask()
{
    while(!viewerDisplay.cycleTaskMustStop)
    {
        WaitTOF();

        if(viewerDisplay.cycleModeEnabled)
            AMI_ILBM_cycleColors(&viewerDisplay.image);
        else
            AMI_ILBM_resetColors(&viewerDisplay.image); /* Restore the palette back to normal */
    }

    viewerDisplay.cycleTaskTerminated = TRUE;
    Wait(0); /* Puts task in a safe state before it terminates */
}

static struct Task *createCycleTask(void)
{
    viewerDisplay.cycleTaskMustStop = FALSE;
    viewerDisplay.cycleTaskTerminated = FALSE;
    viewerDisplay.cycleModeEnabled = FALSE;

    return CreateTask("amiilbmCycleTask", 0, AMI_ILBM_cycleTask, 4000L);
}

static void deleteCycleTask(struct Task *task)
{
    if(task != NULL)
    {
        /* Signal the cycle task to stop */
        viewerDisplay.cycleTaskMustStop = TRUE;

        /* Wait for task to terminate */
        if(!viewerDisplay.cycleTaskTerminated)
            Delay(10);

        /* Delete the task */
        DeleteTask(task);
    }
}

amiVideo_Bool AMI_ILBM_initViewerDisplay(AMI_ILBM_Set *set, const unsigned int number, const amiVideo_Bool previousItemEnabled, const amiVideo_Bool nextItemEnabled)
{
    memset(&viewerDisplay, '\0', sizeof(AMI_ILBM_ViewerDisplay));

    /* Initialise image from the set and attach it to the viewer display */
    if(!AMI_ILBM_initImageFromSet(set, number, &viewerDisplay.image))
    {
        fprintf(stderr, "Cannot extract ILBM from set!\n");
        return FALSE;
    }

    /* Open a window */
    viewerDisplay.window = AMI_ILBM_createWindowFromImage(&viewerDisplay.image);

    if(viewerDisplay.window == NULL)
    {
        fprintf(stderr, "Cannot open window!\n");
        return FALSE;
    }

    /* Construct and set a menu */
    viewerDisplay.menu = createViewerDisplayMenu(viewerDisplay.image.intuitionScreen, previousItemEnabled, nextItemEnabled);

    if(viewerDisplay.menu == NULL)
    {
        fprintf(stderr, "Cannot create menu!\n");
        return FALSE;
    }

    /* Construct task that cycles the colors in the palette */

    viewerDisplay.task = createCycleTask();

    if(viewerDisplay.task == NULL)
    {
        fprintf(stderr, "Cannot create cycle task!\n");
        return FALSE;
    }

    SetMenuStrip(viewerDisplay.window, viewerDisplay.menu); /* Add the menu strip to the window */

    return TRUE; /* We have set up everything successfully */
}

void AMI_ILBM_destroyViewerDisplay(void)
{
    deleteCycleTask(viewerDisplay.task);

    if(viewerDisplay.menu != NULL)
    {
        ClearMenuStrip(viewerDisplay.window); /* Remove the menu strip from the window */
        FreeMenus(viewerDisplay.menu);
    }

    if(viewerDisplay.window != NULL)
        CloseWindow(viewerDisplay.window);

    AMI_ILBM_destroyImage(&viewerDisplay.image);
}

AMI_ILBM_Action AMI_ILBM_handleScreenActions(char **filename)
{
    AMI_ILBM_Action action = ACTION_ERROR;
    int showTitleStatus = TRUE;

    while(action == ACTION_ERROR)
    {
        struct IntuiMessage *msg;

        /* Wait for a message from the window's user port */
        Wait(1 << viewerDisplay.window->UserPort->mp_SigBit);

        /* Handle all the messages stored in the message queue */
        while((msg = (struct IntuiMessage*)GetMsg(viewerDisplay.window->UserPort)) != NULL)
        {
            UWORD menuSelection;

            switch(msg->Class)
            {
                case IDCMP_MENUPICK: /* Handle menu events */
                    menuSelection = msg->Code;

                    if(menuSelection != MENUNULL)
                    {
                        UWORD menuNum = MENUNUM(menuSelection); /* Retrieve the number of the menu of the selected menu item */
                        UWORD itemNum = ITEMNUM(menuSelection); /* Retrieve the number of the menuitem in the selected menu */

                        /* Determine which action to execute */
                        if(menuNum == 0)
                        {
                            switch(itemNum)
                            {
                                case 0:
                                    *filename = AMI_ILBM_openILBMFile(viewerDisplay.window, "");

                                    if(*filename != NULL)
                                        action = ACTION_OPEN;

                                    break;

                                case 1:
                                    action = ACTION_PREVIOUS;
                                    break;

                                case 2:
                                    action = ACTION_NEXT;
                                    break;

                                case 3:
                                    showTitleStatus = !showTitleStatus;
                                    ShowTitle(viewerDisplay.image.intuitionScreen, showTitleStatus);
                                    break;

                                case 4:
                                    viewerDisplay.cycleModeEnabled = !viewerDisplay.cycleModeEnabled;
                                    break;

                                case 5:
                                    action = ACTION_QUIT;
                                    break;
                            }
                        }
                    }

                    break;
            }

            /* Reply the message, so that it's removed from the message queue */
            ReplyMsg((struct Message*)msg);
        }
    }

    /* Return the resulting action from the user */
    return action;
}
