#include <gl.h>
#include "screen.h"
#include "packs.h"
#include "error.h"
#include "preferences.h"

#define kEndShapeToken		0				// the end of shape maker
#define kLineStartToken		1				// the line start marker
#define kDrawPixelsToken	2				// the draw run marker
#define kSkipPixelsToken	3				// the skip pixels marker

#define kNumRLEs 50

GLuint gRLERefs[kNumRLEs];
int gRLELoaded[kNumRLEs];
int gRLEXSize[kNumRLEs];
int gRLEYSize[kNumRLEs];
float gRLEU[kNumRLEs];
float gRLEV[kNumRLEs];
int gRLEInited=false;

void DrawRLE16(int h,int v,int id)
{
	int rowBytes=gRowBytes;
	UInt8 *spritePos=GetSortedPackEntry(kPacksR16,id,nil)+sizeof(Rect);
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

void MeasureRLE(int id)
{
	int x=0,xSize=0,ySize=1;
	UInt8 *spritePos=GetSortedPackEntry(kPacksR16,id,nil)+sizeof(Rect);
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
					x+=tokenData;
				}
				break;
			case kSkipPixelsToken:
				x+=tokenData;
				spritePos+=4;
				break;
			case kLineStartToken:
				if(x>xSize)xSize=x;
				x=0;
				ySize++;
				spritePos+=4;
				break;
			case kEndShapeToken:
				if(x>xSize)xSize=x;
				stop=true;
				break;
			default: 
				DoError(paramErr);
		}
	}
	while (!stop);
	gRLEXSize[id-128]=xSize;	
	gRLEYSize[id-128]=ySize;
}


void LoadRLE(int id)
{
	int xSize;
	int ySize;
	int i;
	Ptr buffer;
	for(i=16;i>0;i--)
	{
		if((1<<i)>=gRLEXSize[id-128]) xSize=1<<i;
		if((1<<i)>=gRLEYSize[id-128]) ySize=1<<i;
	}
	gRLEU[id-128]=(float)gRLEXSize[id-128]/xSize;
	gRLEV[id-128]=(float)gRLEYSize[id-128]/ySize;
	buffer=NewPtrClear(xSize*ySize*2);
	gRowBytes=xSize*2;
	gBaseAddr=buffer;
	DrawRLE16(0,0,id);
	glGenTextures(1,&gRLERefs[id-128]);
	glBindTexture(GL_TEXTURE_2D,gRLERefs[id-128]);
	glTexImage2D(GL_TEXTURE_2D,0,4,xSize,ySize,0,GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,buffer);
	gRLELoaded[id-128]=true;
}

void ScreenVertex(float x,float y);

void DrawRLE(int h,int v,int id)
{
	if(!gRLEInited)
	{
		int i;
		for(i=0;i<kNumRLEs;i++)
			gRLELoaded[i]=false;
		gRLEInited=true;
	}
	if(!gRLELoaded[id-128])
	{
		MeasureRLE(id);
		LoadRLE(id);
	}
	glBindTexture(GL_TEXTURE_2D,gRLERefs[id-128]);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	ScreenVertex(h,v);
	glTexCoord2f(gRLEU[id-128],0);
	ScreenVertex(h+gRLEXSize[id-128],v);
	glTexCoord2f(gRLEU[id-128],gRLEV[id-128]);
	ScreenVertex(h+gRLEXSize[id-128],v+gRLEYSize[id-128]);
	glTexCoord2f(0,gRLEV[id-128]);
	ScreenVertex(h,v+gRLEYSize[id-128]);
	glEnd();
}

void DrawRLEYClip(int h,int v,int id)
{
	DrawRLE(h,v,id);
}
