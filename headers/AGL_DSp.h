#pragma once

#ifndef _AGL_DSP_H_
#define _AGL_DSP_H_


/**> HEADER FILES <**/
#if defined(__APPLE__) && defined(__APPLE_CC__)	// Include headers if using ProjectBuilder
    #include <unistd.h>
    #include <Carbon/Carbon.h>
    #include <OpenGL/gl.h>			// OpenGL
    #include <OpenGL/glu.h>			// OpenGL Utilities
    #include <DrawSprocket/DrawSprocket.h>	// DrawSprocket
    #include <AGL/agl.h>			// Apple's OpenGL
#elif __APPLE__			// Include headers normally using other compilers (such as CodeWarrior)
	#define CALL_IN_SPOCKETS_BUT_NOT_IN_CARBON 1
    #include <unistd.h>
    #include <gl.h>
    #include <glu.h>
    #include <DrawSprocket.h>
    #include <agl.h>
#endif

#include <gl.h>
#include <QD/QD.h>
#include <DrawSprocket/DrawSprocket.h>
#include <AGL/agl.h>


/**> CONSTANT DECLARATIONS <**/
#define	kMoveToFront			kFirstWindowOfClass

// Screen Dimensions
#define SCREEN_WIDTH			640
#define SCREEN_HEIGHT			480


/**> GLOBAL VARIABLES <**/
extern DSpContextAttributes		gDSpContextAttributes;	// Global DrawSprocket context attributes
extern DSpContextReference		gDSpContext;		// The global DrawSprocket context
extern AGLContext			gOpenGLContext;		// The global OpenGL (AGL) context
                                                                // Note: These are actually defined in AGL_DSp.cpp


/**> FUNCTION PROTOTYPES <**/
void		ToolboxInit( void );
CGrafPtr	SetupScreen( int width, int height ,int depth);
void		CreateWindow( WindowRef *theFrontBuffer, int width, int height );
void		ShutdownScreen( void );
AGLContext	SetupAGL( AGLDrawable window,GLint *attrib,int width, int height);
void		CleanupAGL( AGLContext context );


#endif