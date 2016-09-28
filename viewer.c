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

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/gadtools_protos.h>

#include <libilbm/ilbm.h>
#include <libilbm/ilbmimage.h>

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
Action;

typedef struct
{
    struct Screen *screen;
    struct Window *window;
    struct Menu *menu;
}
ViewerDisplay;

static Action handleScreenActions(ViewerDisplay *viewerDisplay, char **filename)
{
    Action action = ACTION_ERROR;
    int showTitleStatus = TRUE;
    
    while(action == ACTION_ERROR)
    {
        struct IntuiMessage *msg;
	
	/* Wait for a message from the window's user port */
        Wait(1 << viewerDisplay->window->UserPort->mp_SigBit);
	
	/* Handle all the messages stored in the message queue */
        while((msg = (struct IntuiMessage*)GetMsg(viewerDisplay->window->UserPort)) != NULL)
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
				    *filename = AMI_ILBM_openILBMFile(viewerDisplay->window, "");
				
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
				    ShowTitle(viewerDisplay->screen, showTitleStatus);
				    break;
				
				case 4:
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

static void destroyViewerDisplay(ViewerDisplay *viewerDisplay)
{
    if(viewerDisplay->menu != NULL)
    {
	ClearMenuStrip(viewerDisplay->window); /* Remove the menu strip from the window */
	FreeMenus(viewerDisplay->menu);
    }
    
    if(viewerDisplay->window != NULL)
	CloseWindow(viewerDisplay->window);
    
    if(viewerDisplay->screen != NULL)
	CloseScreen(viewerDisplay->screen);
}

static struct Menu *createViewerDisplayMenu(struct Screen *screen, int previousItemEnabled, int nextItemEnabled)
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

static int initViewerDisplay(ViewerDisplay *viewerDisplay, ILBM_Image *image, int previousItemEnabled, int nextItemEnabled)
{
    memset(viewerDisplay, '\0', sizeof(ViewerDisplay));
    
    /* Create a screen containing a window and menu */
    viewerDisplay->screen = AMI_ILBM_createScreen(image);
    
    if(viewerDisplay->screen == NULL)
    {
        fprintf(stderr, "Cannot open screen!\n");
        return FALSE;
    }
    else
    {
	amiVideo_Palette palette;
	
	/* Generate a palette from the image */
	AMI_ILBM_initPaletteFromImage(image, &palette);
	
	/* Set the palette of the screen, having the same as defined in the image */
	AMI_ILBM_setPalette(viewerDisplay->screen, &palette);
	
	/* Open a window */
	viewerDisplay->window = AMI_ILBM_createWindow(image, viewerDisplay->screen);
	
	if(viewerDisplay->window == NULL)
	{
	    fprintf(stderr, "Cannot open window!\n");
	    return FALSE;
	}
	
	/* Construct and set a menu */
	viewerDisplay->menu = createViewerDisplayMenu(viewerDisplay->screen, previousItemEnabled, nextItemEnabled);
	
	if(viewerDisplay->menu == NULL)
	{
	    fprintf(stderr, "Cannot create menu!\n");
	    return FALSE;
	}
	
	SetMenuStrip(viewerDisplay->window, viewerDisplay->menu); /* Add the menu strip to the window */
	return TRUE; /* We have set up everything successfully */
    }
}

static Action displayScreen(ILBM_Image *image, char **filename, int previousItemEnabled, int nextItemEnabled)
{
    ViewerDisplay viewerDisplay;
    Action action;
    
    if(initViewerDisplay(&viewerDisplay, image, previousItemEnabled, nextItemEnabled))
        action = handleScreenActions(&viewerDisplay, filename);
    else
        action = ACTION_ERROR;

    destroyViewerDisplay(&viewerDisplay);
    return action;
}

int AMI_ILBM_viewImages(char *filename)
{
    char *aslFilename = NULL;
    Action action;
    
    do /* Repeat this every time the user picks the 'Open file' option */
    {
	/* Read the ILBM file */
	IFF_Chunk *chunk = ILBM_read(filename);
	
	/* We no longer need the filename obtained by the file requester dialog */
	free(aslFilename);
	aslFilename = NULL;
    
	if(chunk == NULL)
	{
	    fprintf(stderr, "Cannot open ILBM file!\n");
    	    return 1;
	}
	else
	{
	    unsigned int imagesLength;
    	    ILBM_Image **images = ILBM_extractImages(chunk, &imagesLength);
        
    	    if(!ILBM_checkImages(chunk, images, imagesLength)) /* Check whether the images are valid */
	    {
		fprintf(stderr, "This ILBM file is not valid!\n");
		action = ACTION_ERROR;
	    }
	    else if(imagesLength == 0) /* We need at least one 1 ILBM image in the IFF file */
	    {
	        fprintf(stderr, "IFF file does not contain any ILBM images!\n");
	        action = ACTION_ERROR;
	    }
	    else
	    {
	        unsigned int imageNumber = 0;
	
	        do /* Repeat this every time the user picks the 'Previous' or the 'Next' option */
	        {
		    action = displayScreen(images[imageNumber], &aslFilename, imageNumber > 0, imageNumber < imagesLength - 1);
		    filename = aslFilename;
		    
		    /* If the user has picked 'Previous' or 'Next' we should modify the imageNumber counter */
		    
		    if(action == ACTION_PREVIOUS)
		        imageNumber--;
		    else if(action == ACTION_NEXT)
		        imageNumber++;
		}
		while(action == ACTION_PREVIOUS || action == ACTION_NEXT);
	    }
	
	    /* Cleanup */
	    ILBM_freeImages(images, imagesLength);
	    ILBM_free(chunk);
	}
    }
    while(action == ACTION_OPEN);
    
    /* Check whether we have an error or not */
    return (action == ACTION_ERROR);
}
