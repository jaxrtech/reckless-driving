#define CALL_IN_SPOCKETS_BUT_NOT_IN_CARBON 1

#ifdef __APPLE__
#include <gl.h>
#include "lzrwHandleInterface.h"
#include <QuickTime.h>
#else
#include <QuickTime/QuickTime.h>
#include <GL/gl.h>
#endif

#include "error.h"
#include "screen.h"
#include "input.h"
#include "gameinitexit.h"
#include "preferences.h"
#include "high.h"
#include "gamesounds.h"
#include "register.h"
#include <DrawSprocket.h>

enum{
	kNoButton=-1,
	kStartGameButton,
	kPrefsButton,
	kScoreButton,
	kHelpButton,
	kQuitButton,
	kRegisterButton,
	kAboutButton
};

Rect **gButtonList;
int gButtonLocation;
int gExit;
short gLevelResFile=0,gAppResFile;
Str63 gLevelFileName;
RgnHandle gButtonRgn;
int gInterfaceInited=false;

#define kNumPicts 10
GLuint gPictRefs[kNumPicts];
int gPictLoaded[kNumPicts];
int gPictInited=false;

void LoadPict(int id,int drawNames)
{	
	GWorldPtr oldGW;
	GDHandle oldGD;
	Handle pic;
	int i;
	
	//get image bounds
	Rect bounds={0,0,512,1024};
	Rect dst={0,0,480,640};
	
	//create a buffer to hold the decompressed pixel data
	int rowBytes=1024*4;
	void *imageBuffer=NewPtr(512*rowBytes);
	
	//create a GWorld structure for the pixel buffer
	GWorldPtr imageGW;
	
	GetGWorld(&oldGW,&oldGD);
	QTNewGWorldFromPtr(&imageGW,k32ARGBPixelFormat,&bounds,nil,nil,0,imageBuffer,rowBytes);
		
		
	//decompress the image to the GWorld
	LockPixels(GetGWorldPixMap(imageGW));
	
	SetGWorld(imageGW,nil);
	
	pic=GetResource('PPic',id);
 	LZRWDecodeHandle(&pic);
	DrawPicture((PicHandle)pic,&dst);
	DisposeHandle(pic);	
	
	if(drawNames)
	{
		ForeColor(whiteColor);
		TextSize(12);
		TextFont(3);
		TextMode(srcOr);
		TextFace(0);
		MoveTo(635,470);
		if(gRegistered)
		{
			Move(-StringWidth("\pRegistered To: "),0);
			Move(-StringWidth(gPrefs.name),0);
			DrawString("\pRegistered To: ");
			DrawString(gPrefs.name);
		}
		else{
			Move(-StringWidth("\p��� This Copy is not Registered! ���"),0);
			DrawString("\p��� This Copy is not Registered! ���");
		}			
		MoveTo(10,15);
		if(gLevelResFile)
		{
			DrawString("\pCustom Level File: ");
			DrawString(gLevelFileName);
		}
	}
	UnlockPixels(GetGWorldPixMap(imageGW));

	//convert ARGB to RGBA
	for(i=0;i<512*rowBytes/4;i++)
	{
		UInt32 buf=*(((UInt32*)imageBuffer)+i);
		UInt8 alpha=(buf&0xff000000)>>24;
		buf=(buf<<8)|0xff;
		*(((UInt32*)imageBuffer)+i)=buf;
	}

	glGenTextures(1,&gPictRefs[id-1000]);
	glBindTexture(GL_TEXTURE_2D,gPictRefs[id-1000]);
	glTexImage2D(GL_TEXTURE_2D,0,4,1024,512,0,GL_RGBA,GL_UNSIGNED_BYTE,imageBuffer);


	gPictLoaded[id-1000]=true;
	
	//re-set the GWorld to use.
	SetGWorld(oldGW,oldGD);

	//dispose our structures.
	DisposeGWorld(imageGW);
}


void ScreenVertex(float x,float y);

void ShowPicScreen(int id)
{
	if(!gPictInited)
	{
		int i;
		for(i=0;i<kNumPicts;i++)
			gPictLoaded[i]=false;
		gPictInited=true;
	}
	if(!gPictLoaded[id-1000])
		LoadPict(id,false);

	glBindTexture(GL_TEXTURE_2D,gPictRefs[id-1000]);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	ScreenVertex(0,0);
	glTexCoord2f(640.0/1024,0);
	ScreenVertex(640,0);
	glTexCoord2f(640.0/1024,480.0/512);
	ScreenVertex(640,480);
	glTexCoord2f(0,480.0/512);
	ScreenVertex(0,480);
	glEnd();

	FadeScreen(1);
	ScreenMode(kScreenRunning);
	Blit2Screen();
	FadeScreen(0);
}

/*
void ShowPicScreen(int id)
{
	UInt64 t0;
	float td=0;
	if(!gPictInited)
	{
		int i;
		for(i=0;i<kNumPicts;i++)
			gPictLoaded[i]=false;
		gPictInited=true;
	}
	if(!gPictLoaded[id-1000])
		LoadPict(id,false);

	glBindTexture(GL_TEXTURE_2D,gPictRefs[id-1000]);

	FadeScreen(1);
	ScreenMode(kScreenRunning);
	
	glDrawBuffer(GL_FRONT);
	glColor3f(0,0,0);
	glBegin(GL_QUADS);
	ScreenVertex(0,0);
	ScreenVertex(640,0);
	ScreenVertex(640,480);
	ScreenVertex(0,480);
	glEnd();
	glDrawBuffer(GL_BACK);
	FadeScreen(512);


	t0=GetMSTime();
	while(td<1)
	{
		td=(GetMSTime()-t0)/(float)1000000;
		glColor3f(td,td,td);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		ScreenVertex(0,0);
		glTexCoord2f(640.0/1024,0);
		ScreenVertex(640,0);
		glTexCoord2f(640.0/1024,480.0/512);
		ScreenVertex(640,480);
		glTexCoord2f(0,480.0/512);
		ScreenVertex(0,480);
		glEnd();
		Blit2Screen();
	}
	glColor3f(1,1,1);
}
*/
void ShowPicScreenNoFade(int id)
{
	if(!gPictInited)
	{
		int i;
		for(i=0;i<kNumPicts;i++)
			gPictLoaded[i]=false;
		gPictInited=true;
	}
	if(!gPictLoaded[id-1000])
		LoadPict(id,false);

	ScreenMode(kScreenRunning);
	glBindTexture(GL_TEXTURE_2D,gPictRefs[id-1000]);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	ScreenVertex(0,0);
	glTexCoord2f(640.0/1024,0);
	ScreenVertex(640,0);
	glTexCoord2f(640.0/1024,480.0/512);
	ScreenVertex(640,480);
	glTexCoord2f(0,480.0/512);
	ScreenVertex(0,480);
	glEnd();

	if(id==1006)
	{
		glTexCoord2f(0,0);
		glBegin(GL_QUADS);
		ScreenVertex(0,240);
		ScreenVertex(640,240);
		ScreenVertex(640,480);
		ScreenVertex(0,480);
		glEnd();
	}
	
	Blit2Screen();
}

void UpdateButtonRgn()
{
	if(gButtonLocation!=kNoButton){
		Rect dst=*((*gButtonList)+gButtonLocation);
		SetRect(&dst,dst.left*(gRealXSize/640.0),dst.top*(gRealYSize/480.0),dst.right*(gRealXSize/640.0),dst.bottom*(gRealYSize/480.0));
		RectRgn(gButtonRgn,&dst);
	}
	else{
		int i;
		RgnHandle screenRgn=NewRgn();
		SetRectRgn(screenRgn,0,0,gRealXSize,gRealYSize);
		SetEmptyRgn(gButtonRgn);
		OpenRgn();
		HLock((Handle)gButtonList);
		for(i=0;i<GetHandleSize((Handle)gButtonList)/sizeof(Rect);i++)
		{
			Rect dst=*((*gButtonList)+i);
			SetRect(&dst,dst.left*(gRealXSize/640.0),dst.top*(gRealYSize/480.0),dst.right*(gRealXSize/640.0),dst.bottom*(gRealYSize/480.0));
			FrameRect(&dst);
		}
		HUnlock((Handle)gButtonList);
		CloseRgn(gButtonRgn);
		DiffRgn(screenRgn,gButtonRgn,gButtonRgn);
		DisposeRgn(screenRgn);
	}
}

void DrawScreen(int button,int id)
{
	if(!gPictLoaded[id-1000])
		LoadPict(id,true);
	glEnable(GL_TEXTURE_2D);
	if(button!=kNoButton)
	{
		Rect dst=*((*gButtonList)+button);

		glBindTexture(GL_TEXTURE_2D,gPictRefs[id-1000]);
		glBegin(GL_QUADS);
		glTexCoord2f(dst.left/1024.0,dst.top/512.0);
		ScreenVertex(dst.left,dst.top);
		glTexCoord2f(dst.right/1024.0,dst.top/512.0);
		ScreenVertex(dst.right,dst.top);
		glTexCoord2f(dst.right/1024.0,dst.bottom/512.0);
		ScreenVertex(dst.right,dst.bottom);
		glTexCoord2f(dst.left/1024.0,dst.bottom/512.0);
		ScreenVertex(dst.left,dst.bottom);
		glEnd();
		Blit2Screen();		
	}
	else{
		ScreenMode(kScreenRunning);
		glBindTexture(GL_TEXTURE_2D,gPictRefs[id-1000]);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		ScreenVertex(0,0);
		glTexCoord2f(640.0/1024,0);
		ScreenVertex(640,0);
		glTexCoord2f(640.0/1024,480.0/512);
		ScreenVertex(640,480);
		glTexCoord2f(0,480.0/512);
		ScreenVertex(0,480);
		glEnd();
		Blit2Screen();
	}
	
}

void ScreenUpdate(WindowPtr win)
{
	if(win)
	{
		BeginUpdate(win);
		EndUpdate(win);
	}
	gButtonLocation=kNoButton;
	UpdateButtonRgn();	
	DrawScreen(kNoButton,1000);
}

void UnloadPics()
{
	glDeleteTextures(1,&gPictRefs[0]);
	gPictLoaded[0]=false;
	glDeleteTextures(1,&gPictRefs[1]);
	gPictLoaded[1]=false;
	glDeleteTextures(1,&gPictRefs[2]);
	gPictLoaded[2]=false;
	ScreenUpdate(nil);
}


void DisposeInterface()
{
	if(gInterfaceInited)
	{
		gPictInited=false;
		gInterfaceInited=false;
		ReleaseResource(gButtonList);
		DisposeRgn(gButtonRgn);
	}
}

void SaveFlushEvents()
{
	int eventMask=mDownMask+mUpMask+keyDownMask+keyUpMask+autoKeyMask;
	EventRecord event;
	FlushEvents(eventMask,0);
	while(WaitNextEvent(eventMask,&event,0,nil));
}

void InitInterface()
{
	if(!gInterfaceInited)
	{
		gButtonList = (Rect **) GetResource('Recs', 1000);
		gButtonRgn=NewRgn();
		gInterfaceInited=true;
	}
	InputMode(kInputSuspended);
	ScreenMode(kScreenRunning);
//	FadeScreen(1);
	ScreenUpdate(nil);
//	FadeScreen(0);
	SaveFlushEvents();
	gGameOn=false;
}

void UpdateButtonLocation()
{
	Point mPos=GetScreenPos(nil);
	int i,button=kNoButton;
	HLock((Handle)gButtonList);
	for(i=0;i<GetHandleSize((Handle)gButtonList)/sizeof(Rect);i++)
	{
		Rect dst=*((*gButtonList)+i);
		SetRect(&dst,dst.left*(gRealXSize/640.0),dst.top*(gRealYSize/480.0),dst.right*(gRealXSize/640.0),dst.bottom*(gRealYSize/480.0));
		if(PtInRect(mPos,&dst))
			button=i;
	}
	HUnlock((Handle)gButtonList);
	if(button!=gButtonLocation)
	{
		if(button!=kNoButton)
			DrawScreen(button,1001);
		if(gButtonLocation!=kNoButton)
			DrawScreen(gButtonLocation,1000);
		gButtonLocation=button;
		UpdateButtonRgn();	
	}
}

int GetButtonClick(Point mPos)
{	
	int i,clicked=false,oldclicked=false,button=kNoButton;
	mPos=GetScreenPos(&mPos);
	HLock((Handle)gButtonList);
	for(i=0;i<GetHandleSize((Handle)gButtonList)/sizeof(Rect);i++)
	{
		Rect dst=*((*gButtonList)+i);
		SetRect(&dst,dst.left*(gRealXSize/640.0),dst.top*(gRealYSize/480.0),dst.right*(gRealXSize/640.0),dst.bottom*(gRealYSize/480.0));
		if(PtInRect(mPos,&dst))
			button=i;
	}
	HUnlock((Handle)gButtonList);
	if(button!=kNoButton)
	{
		Rect dst=*((*gButtonList)+button);
		SetRect(&dst,dst.left*(gRealXSize/640.0),dst.top*(gRealYSize/480.0),dst.right*(gRealXSize/640.0),dst.bottom*(gRealYSize/480.0));
		SimplePlaySound(147);
		while(StillDown())
		{
			mPos=GetScreenPos(nil);
			if(clicked=PtInRect(mPos,&dst))
			{
				if(clicked!=oldclicked)
					DrawScreen(button,1002);
			}else
				DrawScreen(button,1000);
			oldclicked=clicked;
		}
	}
	if(clicked){
		DrawScreen(button,1001);
		return button;
	}
	else 
		return kNoButton;
}

int GetKeyClick(long key)
{	
	switch(key&charCodeMask)
	{
		case 's':
		case 'n':
			return kStartGameButton;
		case 'p':
			return kPrefsButton;
		case 'c':
		case 'o':
			return kScoreButton;
		case '/':
		case '?':
		case 'h':
			return kHelpButton;
		case 'q':
			return kQuitButton;
		case 'r':
			return kRegisterButton;
	}
	switch((key&keyCodeMask)>>8)
	{
		case 0x31:
		case 0x24:
		case 0x4c:
			return kStartGameButton;
		case 0x72:
			return kHelpButton;
		case 0x35:
			return kQuitButton;
	}
	return kNoButton;
}

int KeyClick(long key)
{
	UInt32 ticks;
	int button=GetKeyClick(key);
	if(button==kNoButton)return kNoButton;
	DrawScreen(button,1002);
	SimplePlaySound(147);
	Delay(10,&ticks);
	if(gButtonLocation==button)
		DrawScreen(button,1001);
	else
		DrawScreen(button,1000);
	return button;	
}

void WaitForPress()
{
	int pressed=false;
	do{
		EventRecord event;
		KeyMap theKeys;
		GetKeys(theKeys);
		pressed=theKeys[0]|theKeys[1]|theKeys[2]|theKeys[3]|Button()|ContinuePress();
		WaitNextEvent(everyEvent,&event,0,nil);
	}while(!pressed);
	while(Button());
	SaveFlushEvents();
}

void HandleCommand(int cmd,int modifiers)
{
	switch(cmd)
	{
		case kNoButton: return;
		case kRegisterButton:
			Register(true);
			UnloadPics();
			return;
		case kStartGameButton:
			StartGame(modifiers&optionKey);
			break;
		case kPrefsButton:
			Preferences(); break;
		case kHelpButton:
			ShowPicScreen(1007);WaitForPress(); 
			ShowPicScreen(1008);WaitForPress();
			FadeScreen(1); 		ScreenUpdate(nil);
			FadeScreen(0);
			break;
		case kScoreButton:
			ShowHighScores(-1);
			break;
		case kQuitButton:
			gExit=true; break;
	}
}

void HandleMouseDown(EventRecord *event)
{
	HandleCommand(GetButtonClick(event->where),event->modifiers);
}

extern int gOSX;

void Eventloop()
{
	EventRecord event;	
	ShowCursor();
	if(WaitNextEvent(everyEvent,&event,2,gButtonRgn))
	{
		int eventWasProcessed;
		//if(!gOSX)
			//DoError(DSpProcessEvent(&event,&eventWasProcessed));
		switch(event.what)
		{
			case mouseDown:
				HandleMouseDown(&event);
				break;
			case keyDown:
				HandleCommand(KeyClick(event.message),event.modifiers);
				break;			
			case osEvt:
				switch(event.message>>24)
				{
					case mouseMovedMessage:
						UpdateButtonLocation();	
						break;
					case suspendResumeMessage:
						if(event.message&resumeFlag)	
							ScreenUpdate(nil);
						break;	
				}
				break;
			case kHighLevelEvent :
				AEProcessAppleEvent(&event); 
				
				ScreenUpdate(nil);
				break;
			case updateEvt:
				ScreenUpdate((WindowPtr)event.message);
				break;
		}
	}
	if(ContinuePress())
		HandleCommand(kStartGameButton,0);
		
	if(glGetError() != GL_NO_ERROR)
		SysBeep(0);

}
