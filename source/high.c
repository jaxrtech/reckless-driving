#ifdef __APPLE__
#include <gl.h>
#include <QuickTime.h>
#include "lzrwHandleInterface.h"
#elif defined(_WIN32)
#include "windows_gl.h"
#include <gl/gl.h>
#include <MacTypes.h>
#include <HIToolbox/Events.h>
#endif

#include "preferences.h"
#include "interface.h"
#include "screen.h"
#include "error.h"
#include "gamesounds.h"

void ScreenVertex(float x,float y);

void ShowHighScores(int hilite)
{
	int i;
	GWorldPtr screenGW; 
	GWorldPtr oldGW;
	GDHandle oldGD;
	ShowPicScreen(1004);
	screenGW=GetScreenGW();
	GetGWorld(&oldGW,&oldGD);
	SetGWorld(screenGW,nil);
	TextMode(srcOr);
	if(gRealXSize>1000)
		TextSize(36);
	else
		TextSize(24);
	TextFont(3);
	ForeColor(whiteColor);
	for(i=0;i<kNumHighScoreEntrys;i++)
	{
		Str255 scoreString;
		if(i==hilite)
			TextFace(bold+underline);
		else
			TextFace(0);
		MoveTo(gRealXSize-(gRealXSize>1000?750:500),gRealYSize/3+i*(gRealXSize>1000?45:30));
		DrawString(gPrefs.high[i].name);
		NumToString(gPrefs.high[i].score,scoreString);
		MoveTo(gRealXSize-150,gRealYSize/3+i*(gRealXSize>1000?45:30));
		Move(-StringWidth(scoreString),0);
		DrawString(scoreString);
	}
	ForeColor(blackColor);
	SetGWorld(oldGW,oldGD);
	WaitForPress(); 
	FadeScreen(1); 		
	ScreenUpdate(nil);
	FadeScreen(0);
}

extern int gOSX;

/*void SetHighScoreEntry(int index,UInt32 score)
{
	DialogPtr highDlg;
	short type;
	Rect box;
	Handle item;
	short hit;
	Str255 text;
	if(gOSX)
	{
		FadeScreen(1);
		ScreenMode(kScreenSuspended);
		FadeScreen(0);
	}
	
	highDlg=GetNewDialog(130,nil,(WindowPtr)-1);
	DoError(SetDialogDefaultItem(highDlg,1));
	GetDialogItem(highDlg,2,&type,&item,&box);
	SetDialogItemText(item,gPrefs.lastName);
	SelectDialogItemText(highDlg,2,0,32767);
	do ModalDialog(nil,&hit); while(hit!=1);
	GetDialogItemText(item,text);
	BlockMove(text,gPrefs.lastName,text[0]+1);
	DisposeDialog(highDlg);	
	if(text[0]>15)
	{
		text[0]=15;
		text[15]='É';
	}
	BlockMove(text,gPrefs.high[index].name,text[0]+1);
	gPrefs.high[index].score=score;
	GetDateTime(&gPrefs.high[index].time);
	
	if(gOSX)
	{
		FadeScreen(1);
		ScreenMode(kScreenRunning);
		FadeScreen(512);
	}
}*/

void DrawScore(Str255 name)
{	
	GWorldPtr oldGW;
	GDHandle oldGD;
	Handle pic;
	int i;
	GLuint tex;
	
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
	
	pic=GetResource('PPic',1004);
 	LZRWDecodeHandle(&pic);
	DrawPicture((PicHandle)pic,&dst);
	DisposeHandle(pic);	
	
	ForeColor(whiteColor);
	TextSize(36);
	TextFont(3);
	TextFace(bold);
	TextMode(srcOr);
	TextFace(0);
	MoveTo(100,220);
	DrawString("\pEnter your name:");
	MoveTo(150,300);
	DrawString(name);

	UnlockPixels(GetGWorldPixMap(imageGW));

	//convert ARGB to RGBA
	for(i=0;i<512*rowBytes/4;i++)
	{
		UInt32 buf=*(((UInt32*)imageBuffer)+i);
		UInt8 alpha=(buf&0xff000000)>>24;
		buf=(buf<<8)|alpha;
		*(((UInt32*)imageBuffer)+i)=buf;
	}
	
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,4,1024,512,0,GL_RGBA,GL_UNSIGNED_BYTE,imageBuffer);
	
	//re-set the GWorld to use.
	SetGWorld(oldGW,oldGD);

	//dispose our structures.
	DisposeGWorld(imageGW);

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
	
	glDeleteTextures(1,&tex);
	
	Blit2Screen();
}

void SetHighScoreEntry(int index,UInt32 score)
{
	Str255 text="\p";
	char ch;
	int done=false;
	FadeScreen(512);
	DrawScore(text);
	do
	{
		EventRecord event;	
		while(WaitNextEvent(everyEvent,&event,2,nil))
			switch(event.what)
			{
				case keyDown:
				case autoKey:
					ch=event.message&charCodeMask;
					if((ch=='\r'||ch==0x1b||ch=='\n'))
						done=true;
					else if((ch==0x7f||ch==0x08)&&text[0]>0)
						text[0]--;
					else if(ch>=' '&&ch<=256&&text[0]<15)
						text[++text[0]]=ch;
					DrawScore(text);
					break;			
			}
	}
	while(!done);
	if(text[0]==0)
		BlockMove("\pAnonymous",gPrefs.high[index].name,10);
	else		
		BlockMove(text,gPrefs.high[index].name,text[0]+1);
	gPrefs.high[index].score=score;
	GetDateTime(&gPrefs.high[index].time);
}

void CheckHighScore(UInt32 score)
{
	int i;
	if(gLevelResFile)return;
	for(i=kNumHighScoreEntrys;score>gPrefs.high[i-1].score&&i>0;i--);
	if(i<kNumHighScoreEntrys)
	{
		BlockMoveData(gPrefs.high+i,gPrefs.high+i+1,sizeof(tScoreRecord)*(kNumHighScoreEntrys-i-1));
		SimplePlaySound(153);
		SetHighScoreEntry(i,score);
		WritePrefs(false);
		ShowHighScores(i);
	}

}

void ClearHighScores()
{
	Handle prefDefault=GetResource('Pref',128);
	BlockMoveData(&(((tPrefs*)*prefDefault)->high),&(gPrefs.high),sizeof(tScoreRecord)*kNumHighScoreEntrys);
	ReleaseResource(prefDefault);
	WritePrefs(false);
}

