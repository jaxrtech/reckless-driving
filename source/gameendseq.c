/*
#include "gameframe.h"
#include "lzrwHandleInterface.h"
#include "error.h"
#include "screen.h"
#include "input.h"
#include "gamesounds.h"
#include <gl.h>
#include <QuickTime.h>

#define kScrollSpeed	35	//pixels per second
#define kTexHeight	1024	//pixels per second

extern GLuint gPictRefs[];
extern int gPictLoaded[];

int KeyPress()
{
	int pressed=false;
	KeyMap theKeys;
	GetKeys(theKeys);
	pressed=theKeys[0]|theKeys[1]|theKeys[2]|theKeys[3]|Button();
	return pressed;
}

void ScreenVertex(float x,float y);

void GameEndSequence()
{
	int yScroll;
	PicHandle pic=(PicHandle)GetResource('PPic',1009);
	Rect picSize,draw;
	UInt64 startMS;
 	LZRWDecodeHandle(&pic);
	if(!gPictLoaded[9])
	{
		GWorldPtr oldGW;
		GDHandle oldGD;
		Rect picSize;
		Pattern black;
		int i;
		
		//get image bounds
		Rect bounds={0,0,kTexHeight,1024};

		//create a buffer to hold the decompressed pixel data
		int rowBytes=1024;
		void *imageBuffer=NewPtr(kTexHeight*rowBytes);
		
		//create a GWorld structure for the pixel buffer
		GWorldPtr imageGW;
		
		GetGWorld(&oldGW,&oldGD);
		QTNewGWorldFromPtr(&imageGW,k8IndexedGrayPixelFormat,&bounds,nil,nil,0,imageBuffer,rowBytes);
			
			
		//decompress the image to the GWorld
		LockPixels(GetGWorldPixMap(imageGW));
		
		SetGWorld(imageGW,nil);
		
	 	picSize=(**pic).picFrame;
	 	GetQDGlobalsBlack(&black);
		FillRect(&picSize,&black);
		DrawPicture((PicHandle)pic,&picSize);
		
		UnlockPixels(GetGWorldPixMap(imageGW));		
		
		glGenTextures(1,&gPictRefs[9]);
		glBindTexture(GL_TEXTURE_2D,gPictRefs[9]);
		glTexImage2D(GL_TEXTURE_2D,0,1,1024,kTexHeight,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,imageBuffer);
		gPictLoaded[9]=true;
		
		//re-set the GWorld to use.
		SetGWorld(oldGW,oldGD);

		//dispose our structures.
		DisposeGWorld(imageGW);
	}

	PauseFrameCount();
	BeQuiet();
	FadeScreen(1);
 	picSize=(**pic).picFrame;
	FadeScreen(512);
	startMS=GetMSTime();
	for(yScroll=0;yScroll<(picSize.bottom-picSize.top)+480&&!KeyPress();yScroll+=1)
	{
		float time=(GetMSTime()-startMS)/(float)1000000;
		if(yScroll>=time*kScrollSpeed)
		{
			glBindTexture(GL_TEXTURE_2D,gPictRefs[9]);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0+(float)yScroll/kTexHeight);
			ScreenVertex(0,0);
			glTexCoord2f(640.0/1024,0+(float)yScroll/kTexHeight);
			ScreenVertex(640,0);
			glTexCoord2f(640.0/1024,480.0/512+(float)yScroll/kTexHeight);
			ScreenVertex(640,480);
			glTexCoord2f(0,480.0/512+(float)yScroll/kTexHeight);
			ScreenVertex(0,480);
			glEnd();
			Blit2Screen();
		}
	}
	FadeScreen(1);
	FadeScreen(512);
	DisposeHandle(pic);	
	FlushInput();
	ResumeFrameCount();
}*/


#include "gameframe.h"

#ifdef __APPLE__
#include "lzrwHandleInterface.h"
#endif // __APPLE__

#include "error.h"
#include "screen.h"
#include "input.h"
#include "gamesounds.h"

#define kScrollSpeed	35	//pixels per second

int KeyPress()
{
	int pressed=false;
	KeyMap theKeys;
	GetKeys(theKeys);
	pressed=theKeys[0]|theKeys[1]|theKeys[2]|theKeys[3]|Button();
	return pressed;
}

void GameEndSequence()
{
	int yScroll;
	float scale=1.5;
	Pattern black;
	GWorldPtr textGW;
	GWorldPtr screenGW=GetScreenGW(); 
	GWorldPtr oldGW;
	GDHandle oldGD;
	UInt64 startMS;
	PicHandle pic=(PicHandle)GetResource('PPic',1009);
	Rect picSize,draw,gwSize;
	PauseFrameCount();
	BeQuiet();
	FadeScreen(1);
 	LZRWDecodeHandle(&pic);
 	GetQDGlobalsBlack(&black);
 	picSize=(**pic).picFrame;
	GetGWorld(&oldGW,&oldGD);
	SetRect(&gwSize,0,0,picSize.right*scale,picSize.bottom*scale);
	DoError(NewGWorld(&textGW,8,&gwSize,nil,nil,0));
	SetGWorld(textGW,nil);
	FillRect(&picSize,&black);
	DrawPicture((PicHandle)pic,&gwSize);
	DisposeHandle(pic);		
	SetGWorld(screenGW,nil);
//	SetRect(&draw,0,0,640,480);
	SetRect(&draw,0,0,1024,768);
	FillRect(&draw,&black);
	FadeScreen(512);
	startMS=GetMSTime();
	for(yScroll=0;yScroll<(picSize.bottom-picSize.top)*scale+480&&!KeyPress();yScroll+=1)
	{
		float time=(GetMSTime()-startMS)/(float)1000000;
		if(yScroll>=time*kScrollSpeed)
		{
//			SetRect(&draw,320-(picSize.right-picSize.left)/2,480-yScroll,320+(picSize.right-picSize.left)/2,480+(picSize.bottom-picSize.top)-yScroll);
			SetRect(&draw,512-scale*(picSize.right-picSize.left)/2,768-yScroll,512+scale*(picSize.right-picSize.left)/2,768+scale*(picSize.bottom-picSize.top)-yScroll);
			CopyBits(GetPortBitMapForCopyBits(textGW),GetPortBitMapForCopyBits(screenGW),&gwSize,&draw,srcCopy,nil);	
			while(yScroll>=time*kScrollSpeed)time=(GetMSTime()-startMS)/(float)1000000;
		}
	}
	SetRect(&draw,0,0,1024,768);
//	SetRect(&draw,0,0,640,480);
	FillRect(&draw,&black);
	SetGWorld(oldGW,oldGD);
	DisposeGWorld(textGW);
	FadeScreen(1);
	FadeScreen(512);
	FlushInput();
	ResumeFrameCount();
}