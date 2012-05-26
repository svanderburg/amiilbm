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

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <exec/memory.h>

#include <clib/exec_protos.h>

#include "viewer.h"
#include "filepicker.h"

#define INTUITION_VERSION 39
#define GRAPHICS_VERSION 39
#define GADTOOLS_VERSION 37
#define ASL_VERSION 37

/** Struct pointing to intuition.library */
struct Library *IntuitionBase = NULL;

/** Struct pointing to graphics.library */
struct Library *GfxBase = NULL;

/** Struct pointing to the asl.library */
struct Library *AslBase = NULL;

/** Struct pointing to the gadtools.library */
struct Library *GadToolsBase = NULL;

/**
 * Opens all required external libraries
 */
static int openLibraries()
{
    IntuitionBase = OpenLibrary("intuition.library", INTUITION_VERSION);
    
    if(IntuitionBase == NULL)
    {
	fprintf(stderr, "Cannot open intuition.library\n");
	return FALSE;
    }
    
    GfxBase = OpenLibrary("graphics.library", GRAPHICS_VERSION);
    
    if(GfxBase == NULL)
    {
	fprintf(stderr, "Cannot open graphics.library\n");
	return FALSE;
    }
    
    GadToolsBase = OpenLibrary("gadtools.library", GADTOOLS_VERSION);
    
    if(GadToolsBase == NULL)
    {
	fprintf(stderr, "Cannot open gadtools.library\n");
	return FALSE;
    }

    AslBase = OpenLibrary("asl.library", ASL_VERSION);
    
    if(AslBase == NULL)
	fprintf(stderr, "Cannot open asl.library!\n");
    
    return TRUE;
}

/**
 * Closes all required external libraries
 */
static void closeLibraries()
{
    if(IntuitionBase != NULL)
	CloseLibrary(IntuitionBase);
	
    if(GfxBase != NULL)
	CloseLibrary(IntuitionBase);

    if(GadToolsBase != NULL);
	CloseLibrary(GadToolsBase);

    if(AslBase != NULL)
	CloseLibrary(AslBase);
}

int main(int argc, char *argv[])
{
    char *aslFilename = NULL, *filename;
    int status;
    
    openLibraries();
    
    if(argc > 1)
	filename = argv[1]; /* If a command-line filename is given -> use it */
    else
    {
	/* If no command-line filename is given, open a file dialog to pick a file */
	
	aslFilename = AMI_ILBM_openILBMFile(NULL, "");
	filename = aslFilename;
    }
    
    if(filename == NULL)
    {
	fprintf(stderr, "No valid IFF filename given!\n");
	status = 1;
    }
    else
	return AMI_ILBM_viewImages(filename); /* Try to view the given IFF file */

    /* Cleanup */
    free(aslFilename);
    closeLibraries();

    /* Return the status */
    return status;
}
