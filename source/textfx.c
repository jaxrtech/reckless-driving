#include "screen.h"
#include "objects.h"
#include "textfx.h"
#include "trig.h"
#include "renderframe.h"
#include "packs.h"
#include "preferences.h"
#include "error.h"
#include <gl.h>

#define kMaxFX	10
#define kEndShapeToken		0				// the end of shape maker
#define kLineStartToken		1				// the line start marker
#define kDrawPixelsToken	2				// the draw run marker
#define kSkipPixelsToken	3				// the skip pixels marker
#define kCharSize 32
#define kExplAccel 10.0
#define kEffectAccel 150.0
#define kEffectTimeOut 2.0
#define kEffectSinAmp	30.0
#define kEffectSinFreq	(2.0/kCharSize)
#define kEffectSinVelo	(2*PI)

tTextEffect gTextFX[kMaxFX];
int gFXCount=0;

#define kNumChars 40
GLuint gCharRefs[kNumChars];
int gCharsInited=false;

void DrawChar16(int h,int v,int id)
{
	int rowBytes=gRowBytes;
	UInt8 *spritePos=GetSortedPackEntry(kPackcR16,id,nil)+sizeof(Rect);
	UInt8 *lineStart=gBaseAddr;
	UInt16 *dst=lineStart;
	int stop=0;
	do
	{
		SInt32 tokenData=(*((unsigned long *)spritePos))&0x00ffffff;
		switch (*spritePos)
		{
			case kDrawPixelsToken:
				{
					int i=0;
					UInt16 *src=spritePos+4;
					int tokenSize=tokenData*2;
					spritePos+=4+tokenSize+(tokenSize&3?(4-tokenSize&3):0);
					while(tokenData-2>=i)
					{
						*((long*)(dst+i))=(*((long*)(src+i)))|0x80008000;
						i+=2;
					}
					if(tokenData-(int)sizeof(short)>=i)
						*((short*)(dst+i))=(*((short*)(src+i)))|0x8000;
					dst+=tokenData;
				}
				break;
			case kSkipPixelsToken:
				dst+=tokenData;
				spritePos+=4;
				break;
			case kLineStartToken:
				lineStart+=rowBytes;
				dst=lineStart;
				spritePos+=4;
				break;
			case kEndShapeToken:
				stop=true;
				break;
			default: 
				DoError(paramErr);
		}
	}
	while (!stop);
}
void PrintConsoleString(const char *fmt, ...);

void LoadChars()
{
	int i;
	for(i=0;i<kNumChars;i++)
	{
		Ptr buffer=NewPtr(kCharSize*kCharSize*2);
		gRowBytes=kCharSize*2;
		gBaseAddr=buffer;
		DrawChar16(0,0,i+128);
		glGenTextures(1,&gCharRefs[i]);
		glBindTexture(GL_TEXTURE_2D,gCharRefs[i]);
		glTexImage2D(GL_TEXTURE_2D,0,4,kCharSize,kCharSize,0,GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,buffer);
	}
	gCharsInited=true;	
}

void ScreenVertex(float x,float y);

void DrawGLChar(int h,int v,char ch,float zoom)
{
	glBindTexture(GL_TEXTURE_2D,gCharRefs[ch-'A']);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	ScreenVertex(h,v);
	glTexCoord2f(1,0);
	ScreenVertex(h+kCharSize*zoom,v);
	glTexCoord2f(1,1);
	ScreenVertex(h+kCharSize*zoom,v+kCharSize*zoom);
	glTexCoord2f(0,1);
	ScreenVertex(h,v+kCharSize*zoom);
	glEnd();
}

void NewTextEffect(tTextEffect *effect)
{
	if(gFXCount<kMaxFX)
	{
		gTextFX[gFXCount]=*effect;
		gTextFX[gFXCount].fxStartFrame=gFrameCount;
		gFXCount++;
	}
}

void MakeFXStringFromNumStr(Str31 numStr,Str31 fxStr)
{
	int i;
	for(i=0;i<numStr[0];i++)
		fxStr[i+1]=numStr[i+1]+0x2b;
	fxStr[0]=numStr[0];
}




void DrawTextFXZoomed(float xDrawStart,float yDrawStart,float zoom)
{
	int i,line,ch;
	if(!gCharsInited)
		LoadChars();
	for(i=0;i<gFXCount;i++)
	{
		float dt=(gFrameCount-gTextFX[i].fxStartFrame)*kFrameDuration*(gTextFX[i].effectFlags&kEffectTiny?3:1);
		float exploZoom=(gTextFX[i].effectFlags&kEffectExplode?1+0.5*dt*dt*kExplAccel:1)*(gTextFX[i].effectFlags&kEffectTiny?0.25:1);
		float baseX=gTextFX[i].x-gTextFX[i].text[0]*kCharSize*0.5*exploZoom;
		float baseY=gTextFX[i].y-kCharSize*0.5*exploZoom;
		if(gTextFX[i].effectFlags&kEffectAbsPos)
		{
			baseY=(yDrawStart-baseY)/zoom;
			baseX=(baseX-xDrawStart)/zoom;
			gTextFX[i].x=(gTextFX[i].x-xDrawStart)/zoom;
			gTextFX[i].y=(yDrawStart-gTextFX[i].y)/zoom;
			gTextFX[i].effectFlags^=kEffectAbsPos;
		}
		if(gTextFX[i].effectFlags&kEffectMoveUp)
			baseY-=0.5*dt*dt*kEffectAccel;
		else if(gTextFX[i].effectFlags&kEffectMoveDown)
			baseY+=0.5*dt*dt*kEffectAccel;
		if(gTextFX[i].effectFlags&kEffectMoveLeft)
			baseX-=0.5*dt*dt*kEffectAccel;
		else if(gTextFX[i].effectFlags&kEffectMoveRight)
			baseX+=0.5*dt*dt*kEffectAccel;
		for(ch=1;ch<=gTextFX[i].text[0];ch++)
		{
			glColor4f(1,1,1,1-dt*0.5);
			DrawGLChar(baseX+kCharSize*exploZoom*(ch-1),baseY,gTextFX[i].text[ch],exploZoom);
			glColor4f(1,1,1,1);
		}
		if(dt*(gTextFX[i].effectFlags&kEffectTiny?2:1)>=kEffectTimeOut)
		{
			if(gFXCount>i+1)
				BlockMoveData(gTextFX+i+1,gTextFX+i,(gFXCount-i-1)*sizeof(tTextEffect));
			gFXCount--;
		}
	}
}

void SimpleDrawText(Str255 text,int xPos,int yPos)
{
	int ch,line;
	for(ch=1;ch<=text[0];ch++)
		DrawGLChar(xPos+kCharSize*0.5*(ch-1),yPos,text[ch],0.5);
}

void ClearTextFX()
{
	gFXCount=0;	
}