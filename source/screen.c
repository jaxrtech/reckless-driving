#include <QDOffscreen.h>
#include <math.h>
#include "error.h"
#include "screen.h"
#include "screenfx.h"
#include "preferences.h"
#include "AGL_DSp.h"

#define kDecimalPrec 3

#define __option(VAR) 0

DSpContextReference gDrawContext;
Ptr gBaseAddr;
short gRowBytes;
char gMessageBuffer[1024];
char* gMessagePos;
int gMessageCount;
short gXSize,gYSize;
int gRealXSize=1024;
int gRealYSize=768;
int gFlickerMode=false;
int gOddLines;
Handle gTranslucenceTab=nil,g16BitClut=nil;
UInt8 gLightningTab[kLightValues][256];
int gScreenMode;
int gScreenBlitSpecial=false;
int abs(int);

void MakeDecString(int value,StringPtr str)
{
	int i;
	str[0]=kDecimalPrec+1;
	str[1]='.';
	for(i=kDecimalPrec+1;i>1;i--)
	{
		str[i]='0'+value%10;
		value/=10;
	}
}

void AddStringToMessageBuffer(StringPtr str)
{	
	int i;
	for(i=0;i<str[0];i++)
		*(gMessagePos++)=str[i+1];
}

void AddFloatToMessageBuffer(StringPtr label,float value)
{
	Str255 str;
	
	AddStringToMessageBuffer(label);
	NumToString(trunc(value),str);
	AddStringToMessageBuffer(str);
	MakeDecString(fabs((value-trunc(value))*1000),str);
	AddStringToMessageBuffer(str);
	*(gMessagePos++)='\n';
	gMessageCount++;
}

void DrawMessageBuffer()
{	
	int i;
	char* writePos=gMessageBuffer+1;
	char* oldWritePos=gMessageBuffer;
	
	TextFont(1);
	TextMode(srcOr);
	for(i=0;i<gMessageCount;i++)
	{
		MoveTo(10,(i+1)*15);
		while(*writePos!='\n') writePos++;
		*oldWritePos=writePos-oldWritePos-1;
		DrawString(oldWritePos);
		oldWritePos=writePos;
		writePos++;
	}
}

void FlushMessageBuffer()
{
	gMessagePos=gMessageBuffer+1;
	gMessageCount=0;
}
	CGrafPtr theScreen;

void InitGL()
{
	GLint p=1;
		GLint attrib[] = { AGL_RGBA, AGL_PIXEL_SIZE, 32, AGL_DOUBLEBUFFER, AGL_NONE };	   
	gOpenGLContext=SetupAGL((AGLDrawable)theScreen,attrib,gRealXSize,gRealYSize);
	if(gOpenGLContext==NULL)SysBeep(0);
//	aglSetInteger(gOpenGLContext,AGL_SWAP_INTERVAL,&p);
    glMatrixMode(GL_PROJECTION);                                            // Select The Projection Matrix
    glLoadIdentity();                                                       // Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)640/(GLfloat)480,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);                                             // Select The Modelview Matrix	glLoadIdentity();
    glLoadIdentity();                                                       // Reset The Projection Matrix
	glTranslatef(0.0f,0.0f,-1);	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}


void InitScreen()
{
	Rect boundsRect;
	SetRect(&boundsRect,50,50,50+1024,50+768);
	theScreen=GetWindowPort(NewCWindow (0,&boundsRect,"\pRedline",true,0,(WindowRef)-1L,false,0));
	gXSize=640;
	gYSize=480;
	SetScreenClut(8);	
}
/*
void InitGL()
{
	GLint p=1;
	CGrafPtr theScreen;
		GLint attrib[] = { AGL_RGBA, AGL_PIXEL_SIZE, 32, AGL_DOUBLEBUFFER, AGL_NONE };	   
    DSpContext_GetFrontBuffer(gDrawContext,&theScreen);
	gOpenGLContext=SetupAGL((AGLDrawable)theScreen,attrib,gRealXSize,gRealYSize);
	if(gOpenGLContext==NULL)SysBeep(0);
	aglSetInteger(gOpenGLContext,AGL_SWAP_INTERVAL,&p);
    glMatrixMode(GL_PROJECTION);                                            // Select The Projection Matrix
    glLoadIdentity();                                                       // Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)640/(GLfloat)480,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);                                             // Select The Modelview Matrix	glLoadIdentity();
    glLoadIdentity();                                                       // Reset The Projection Matrix
	glTranslatef(0.0f,0.0f,-1);	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}*/
#include "stdio.h"
#include "string.h"
int HasBigVRAM()
{char str[256];

AGLRendererInfo info, head_info;
GLint inum;
GLint dAccel = 0;
GLint dVRAM = 0, dMaxVRAM = 0;
AGLDevice gd=GetMainDevice();
head_info = aglQueryRendererInfo(&gd, 1);
if(!head_info){
return 0;}
info = head_info;
inum = 0;
// see if we have an accelerated renderer, if so ignore non-accelerated ones
// this prevents returning info on software renderer when actually we'll get the hardware one

while (info)
{
aglDescribeRenderer (info, AGL_ACCELERATED, &dAccel);
// if we can accel then we will choose the accelerated renderer
// how about compliant renderers???
if (dAccel)
{
	aglDescribeRenderer (info, AGL_VIDEO_MEMORY, &dVRAM); // we assume that VRAM returned is total thus add texture and VRAM required
	if (dVRAM >= dMaxVRAM) // find card with max VRAM
		dMaxVRAM = dVRAM; // store max
}
info = aglNextRendererInfo(info);
inum++;
}
aglDestroyRendererInfo(head_info);

return dVRAM>8*1024*1024;
	
}
/*
void InitScreen()
{
	DSpContextAttributes inDesiredAttributes;
	SInt32 cpuSpeed;
	OSErr err;
	int ok=false;
	err=Gestalt(gestaltProcClkSpeed,&cpuSpeed);
	gXSize=640;
	gYSize=480;	
	inDesiredAttributes.frequency=0;
	DoError(DSpStartup());
	if(!HasBigVRAM())gPrefs.res=1;
	switch(gPrefs.res)
	{
		case 1:
			gRealXSize=640;
			gRealYSize=480;
			break;
		case 2:
			gRealXSize=800;
			gRealYSize=600;
			break;
		case 3:
			gRealXSize=1024;
			gRealYSize=767;
			break;
		case 4:
			gRealXSize=1280;
			gRealYSize=960;
			break;
		case 5:
			gRealXSize=1600;
			gRealYSize=1200;
			break;
	}
	while(!ok)
	{
		inDesiredAttributes.displayWidth=gRealXSize;
		inDesiredAttributes.displayHeight=gRealYSize;
		inDesiredAttributes.reserved1=0;
		inDesiredAttributes.reserved2=0;
		inDesiredAttributes.colorNeeds=kDSpColorNeeds_Request;
		inDesiredAttributes.colorTable=gPrefs.hiColor?nil:GetCTable(8);
		inDesiredAttributes.contextOptions=0;
		inDesiredAttributes.backBufferDepthMask=kDSpDepthMask_32;
		inDesiredAttributes.displayDepthMask=kDSpDepthMask_32;
		inDesiredAttributes.backBufferBestDepth=32;
		inDesiredAttributes.displayBestDepth=32;
		inDesiredAttributes.pageCount=0;
		inDesiredAttributes.gameMustConfirmSwitch=false;
		inDesiredAttributes.reserved3[0]=0;
		inDesiredAttributes.reserved3[1]=0;
		inDesiredAttributes.reserved3[2]=0;
		inDesiredAttributes.reserved3[3]=0;	
		if(DSpFindBestContext(&inDesiredAttributes,&gDrawContext))
		{
			gRealXSize=640;
			gRealYSize=480;
			gPrefs.res=1;
		}
		else
			ok=true;
	}
	inDesiredAttributes.contextOptions=kDSpContextOption_PageFlip+(((UInt32)cpuSpeed<150000000)||err)?0:kDSpContextOption_DontSyncVBL;
	inDesiredAttributes.pageCount=1;
	DoError(DSpContext_Reserve(gDrawContext,&inDesiredAttributes));
//	gScreenMode=kScreenSuspended;
	gScreenMode=kScreenRunning;
}*/

void FadeScreen(int out)
{
#if __option(scheduling)
	switch(out)
	{
		case 1:DoError(DSpContext_FadeGammaOut(gDrawContext,0));return;
		case 0:DoError(DSpContext_FadeGammaIn(gDrawContext,0));return;
	}	
	if(out>=256)
		DSpContext_FadeGamma(gDrawContext,(out-256)/256.0*100,nil);
	if(out>256+256)
		DSpContext_FadeGamma(gDrawContext,0,nil);
#endif				
}


void ScreenMode(int mode)
{
	GWorldPtr gameGW;
	CGrafPtr theScreen;
	if(gScreenMode!=mode)
	switch(mode)
	{
		case kScreenRunning:
			InitScreen(0);
		//	DoError(DSpContext_SetState(gDrawContext,kDSpContextState_Active));
		
		
		//	DoError(DSpContext_GetBackBuffer(gDrawContext,kDSpBufferKind_Normal,&gameGW));
		//	gBaseAddr=GetPixBaseAddr(GetGWorldPixMap(gameGW));
		//	gRowBytes=(**GetGWorldPixMap(gameGW)).rowBytes & 0x3FFF;
		//	SetGWorld(gameGW,nil);
			//if(gScreenMode==kScreenSuspended)
				SetScreenClut(8);
			if(gOpenGLContext)
			{
			    DSpContext_GetFrontBuffer(gDrawContext,&theScreen);
				if(!aglSetDrawable(gOpenGLContext,theScreen))
				{
					Str255 err;
					SysBeep(0);
					CopyCStringToPascal(aglErrorString(aglGetError()),err);
					DebugStr(err);
				}
			}
			else
					InitGL();

			break;
		case kScreenPaused:
			//DoError(DSpContext_SetState(gDrawContext,kDSpContextState_Paused));
			//if(gScreenMode==kScreenSuspended)
			//	SetScreenClut(8);
			//break;			
		case kScreenSuspended:
			//DoError(DSpContext_SetState(gDrawContext,kDSpContextState_Inactive));
			//break;			
		case kScreenStopped:
			//aglDestroyContext(gOpenGLContext);
			if(gScreenMode==kScreenRunning){
			DoError(DSpContext_Release(gDrawContext));	
			DoError(DSpShutdown());
			}
			break;
	}
	gScreenMode=mode;
}

void KillGL()
{
	aglDestroyContext(gOpenGLContext);
}

GWorldPtr GetScreenGW()
{
	GWorldPtr screenGW;
	DoError(DSpContext_GetFrontBuffer(gDrawContext,&screenGW));
	return screenGW;
}


inline RGBColor FadeColor(RGBColor pRGBColor,float fade)
{
     RGBColor tRGBColor;

     tRGBColor.red=  fade*pRGBColor.red;
     tRGBColor.green=fade*pRGBColor.green;
     tRGBColor.blue= fade*pRGBColor.blue;
     return tRGBColor;
}

void SetScreenClut(int id)
{
/*	if(!gPrefs.hiColor)
	{
		long bright,color,bestScore,bestIndex,score,testIndex;
		RGBColor optColor,testColor;
		CTabHandle ct=GetCTable(id);
		DoError(DSpContext_SetCLUTEntries(gDrawContext,&((**ct).ctTable),0,255));
		if(gTranslucenceTab)
			ReleaseResource(gTranslucenceTab);
		gTranslucenceTab=GetResource('Trtb',id);
		for(bright=0;bright<kLightValues;bright++)
			for(color=0;color<256;color++)
			{
				bestScore=3*65536;
				bestIndex=0;
				optColor=(**ct).ctTable[color].rgb;
				optColor=FadeColor(optColor,(float)bright/kLightValues);
				for(testIndex=0;testIndex<256;testIndex++)
				{
					testColor=(**ct).ctTable[testIndex].rgb;
					score=__abs(optColor.red  -testColor.red)
						 +__abs(optColor.green-testColor.green)
						 +__abs(optColor.blue -testColor.blue);	
					if(score<bestScore)
					{
						bestScore=score;
						bestIndex=testIndex;
					}
				}
				gLightningTab[bright][color]=bestIndex;
			}
		DisposeCTable(ct);
	}
	else
	{*/
		if(g16BitClut)
			ReleaseResource(g16BitClut);
		g16BitClut=GetResource('Cl16',id);			
	//}	
}

extern int gOSX;

void Blit2Screen()
{
	aglSwapBuffers(gOpenGLContext);
}

PicHandle MakeGWPicture(Rect *size)
{
	PicHandle thePicture;
	OpenCPicParams picHeader={{0,0,0,0},0x00480000,0x00480000,-2,0,0};
	GDHandle gd;
	GWorldPtr gw;
	Rect bounds;
	
	picHeader.srcRect=*size;
	GetGWorld(&gw,&gd);
	thePicture=OpenCPicture(&picHeader);
	GetPortBounds(gw,&bounds);
	CopyBits(*(BitMapHandle)GetGWorldPixMap(gw),*(BitMapHandle)GetGWorldPixMap(gw),&bounds,size,nil,srcCopy);
	ClosePicture();
	return thePicture;	
}

void TakeScreenshot()
{
	Rect rec;
	PicHandle screenPic;
	SetRect(&rec,0,0,gXSize,gYSize);
	screenPic=MakeGWPicture(&rec);
	AddResource((Handle)screenPic,'PICT',Count1Resources('PICT')+1000,"\p");
	ReleaseResource((Handle)screenPic);
}

Point GetScreenPos(Point *inPos)
{
/*	Point pos;
	if(inPos)
		pos=*inPos;
	else
		DoError(DSpGetMouse(&pos));
	DoError(DSpContext_GlobalToLocal(gDrawContext,&pos));
	return pos;*/
	Point pos;
	GetMouse(&pos);
	return pos;
}

/*/

	CGrafPtr theScreen=NULL;

void InitGL()
{
	GLint p=1;
		GLint attrib[] = { AGL_RGBA, AGL_PIXEL_SIZE, 32, AGL_DOUBLEBUFFER, AGL_NONE };	   
	gOpenGLContext=SetupAGL((AGLDrawable)theScreen,attrib,gRealXSize,gRealYSize);
	if(gOpenGLContext==NULL)SysBeep(0);
//	aglSetInteger(gOpenGLContext,AGL_SWAP_INTERVAL,&p);
    glMatrixMode(GL_PROJECTION);                                            // Select The Projection Matrix
    glLoadIdentity();                                                       // Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)640/(GLfloat)480,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);                                             // Select The Modelview Matrix	glLoadIdentity();
    glLoadIdentity();                                                       // Reset The Projection Matrix
	glTranslatef(0.0f,0.0f,-1);	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}


void InitScreen()
{
	Rect boundsRect;
	SetRect(&boundsRect,50,50,50+1024,50+768);
	theScreen=GetWindowPort(NewCWindow (0,&boundsRect,"\pRedline",true,0,(WindowRef)-1L,false,0));
	gXSize=640;
	gYSize=480;
	SetScreenClut(8);	
}

void FadeScreen(int out)
{return;
#if __option(scheduling)
	switch(out)
	{
		case 1:DoError(DSpContext_FadeGammaOut(gDrawContext,0));return;
		case 0:DoError(DSpContext_FadeGammaIn(gDrawContext,0));return;
	}	
	if(out>=256)
		DSpContext_FadeGamma(gDrawContext,(out-256)/256.0*100,nil);
	if(out>256+256)
		DSpContext_FadeGamma(gDrawContext,0,nil);
#endif				
}


void ScreenMode(int mode)
{
}

GWorldPtr GetScreenGW()
{
	return theScreen;
}


inline RGBColor FadeColor(RGBColor pRGBColor,float fade)
{
     RGBColor tRGBColor;

     tRGBColor.red=  fade*pRGBColor.red;
     tRGBColor.green=fade*pRGBColor.green;
     tRGBColor.blue= fade*pRGBColor.blue;
     return tRGBColor;
}

void SetScreenClut(int id)
{
	if(g16BitClut)
		ReleaseResource(g16BitClut);
	g16BitClut=GetResource('Cl16',id);			
}

extern int gOSX;

void Blit2Screen()
{
	aglSwapBuffers(gOpenGLContext);
}

PicHandle MakeGWPicture(Rect *size)
{
	PicHandle thePicture;
	OpenCPicParams picHeader={{0,0,0,0},0x00480000,0x00480000,-2,0,0};
	GDHandle gd;
	GWorldPtr gw;
	Rect bounds;
	
	picHeader.srcRect=*size;
	GetGWorld(&gw,&gd);
	thePicture=OpenCPicture(&picHeader);
	GetPortBounds(gw,&bounds);
	CopyBits(*(BitMapHandle)GetGWorldPixMap(gw),*(BitMapHandle)GetGWorldPixMap(gw),&bounds,size,nil,srcCopy);
	ClosePicture();
	return thePicture;	
}

void TakeScreenshot()
{
	Rect rec;
	PicHandle screenPic;
	SetRect(&rec,0,0,gXSize,gYSize);
	screenPic=MakeGWPicture(&rec);
	AddResource((Handle)screenPic,'PICT',Count1Resources('PICT')+1000,"\p");
	ReleaseResource((Handle)screenPic);
}

Point GetScreenPos(Point *inPos)
{
}

*/