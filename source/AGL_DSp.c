/**> HEADER FILES <**/
#include "AGL_DSp.h"


/**> GLOBAL VARIABLES <**/
DSpContextAttributes	gDSpContextAttributes;	// Global DrawSprocket context attributes
DSpContextReference	gDSpContext;		// The global DrawSprocket context
AGLContext		gOpenGLContext=nil;		// The global OpenGL (AGL) context

/********************> SetupScreen() <*****/
CGrafPtr	SetupScreen( int width, int height ,int depth)
{
	
	OSStatus	theError;
	CGrafPtr	theFrontBuffer;
	
	// Start DrawSprocket
	theError = DSpStartup();
	if( theError )
		return NULL;
	
	// Set the Context Attributes
	gDSpContextAttributes.displayWidth = width;
	gDSpContextAttributes.displayHeight = height;
	gDSpContextAttributes.colorNeeds = kDSpColorNeeds_Require;
	gDSpContextAttributes.displayDepthMask = kDSpDepthMask_32;
	gDSpContextAttributes.backBufferDepthMask = kDSpDepthMask_32;
	gDSpContextAttributes.displayBestDepth = depth;
	gDSpContextAttributes.backBufferBestDepth = depth;
	gDSpContextAttributes.pageCount = 1;
	
	// Find the best context for our attributes
	theError = DSpFindBestContext( &gDSpContextAttributes, &gDSpContext );
	if( theError != noErr )
		return NULL;
	
	// Reserve that context
	theError = DSpContext_Reserve( gDSpContext, &gDSpContextAttributes );
	if( theError != noErr )
		return NULL;
	
	// Fade out
	theError = DSpContext_FadeGammaOut( NULL, NULL );	
	if( theError != noErr )
		return NULL;
	
	// Activate the context
	theError = DSpContext_SetState( gDSpContext, kDSpContextState_Active );
	if( theError != noErr )
	{
		// Fade back in the display before dying
		theError = DSpContext_FadeGammaIn( NULL, NULL );
		
		// Now do the fatal error alert
		return NULL;
	}
	
	// Fade in
	theError = DSpContext_FadeGammaIn( NULL, NULL );
	if( theError != noErr )
		return NULL;
		
	// Get the front buffer to the DrawSprocket context
        DSpContext_GetFrontBuffer( gDSpContext, &theFrontBuffer );
	
	return theFrontBuffer;
	
}

/********************> CreateWindow() <*****/
void		CreateWindow( WindowRef *theFrontBuffer, int width, int height )
{
	
	Rect			rect;
	OSErr			error;
	RGBColor 		backColor = { 0xFFFF, 0xFFFF, 0xFFFF };
	RGBColor 		foreColor = { 0x0000, 0x0000, 0x0000 };
	
	// Set the window rect
	rect.top = rect.left = 0;
	DSpContext_LocalToGlobal( gDSpContext, ( Point* )&rect );
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	
	// Create a new color window
	theFrontBuffer = NewCWindow( NULL, &rect, "\p", 0, plainDBox, kMoveToFront, 0, 0 );
	
	// Show the window
	ShowWindow( theFrontBuffer );
	SetPortWindowPort( theFrontBuffer );
	
	// Set current pen colors
	RGBForeColor( &foreColor );
	RGBBackColor( &backColor );
	
}

/********************> ShutdownScreen() <*****/
void		ShutdownScreen( void )
{
	
	DSpContext_FadeGammaOut( NULL, NULL );
	//DisposeWindow( theFrontBuffer );
	DSpContext_SetState( gDSpContext, kDSpContextState_Inactive );
	DSpContext_FadeGammaIn( NULL, NULL );
	DSpContext_Release( gDSpContext );
	DSpShutdown();
	
}

/********************> SetupAGL() <*****/
AGLContext	SetupAGL( AGLDrawable window,GLint *attrib,int width, int height)
{
	AGLPixelFormat format;
	AGLContext     context;
	GLboolean      ok;
	GLint fullScreenAttrib[64];
	int i;
	
	GDHandle screen;
	if(window)
		screen=GetGWorldDevice((GWorldPtr)window);
	else
		screen=DMGetFirstScreenDevice(true);
	
	if(!window)
	{
		fullScreenAttrib[0]=AGL_FULLSCREEN;
		for(i=0;attrib[i-1]!=AGL_NONE;i++)
			fullScreenAttrib[i+1]=attrib[i];
		attrib=fullScreenAttrib;
	}
	
	// Choose an rgb pixel format
	format = aglChoosePixelFormat( &screen, 1, attrib );
	if ( format == NULL ){DebugStr("\pFORMAT");
		return NULL;}

	// Create an AGL context
	context = aglCreateContext( format, NULL );
	if ( context == NULL ){DebugStr("\pCONTEXT");
		return NULL;}

	if(window){
		ok = aglSetDrawable(context,window);
		if ( !ok ){DebugStr("\pDRAWABLE");
			return NULL;}
	}
	else{
		ok = aglSetFullScreen (context,width,height,60,0);
		if ( !ok )
			return NULL;
	}
	
	// Make the context the current context
	ok = aglSetCurrentContext( context );
	if ( !ok )
		return NULL;

	// The pixel format is no longer needed so get rid of it
	aglDestroyPixelFormat( format );

	return context;
	
}


/********************> CleanupAGL() <*****/
void		CleanupAGL( AGLContext context )
{
	
	aglSetCurrentContext( NULL );
	aglSetDrawable( context, NULL );
	aglDestroyContext( context );
	
}
