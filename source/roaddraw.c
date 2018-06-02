#include "roads.h"
#include "objects.h"
#include "packs.h"
#include "preferences.h"
#include "screen.h"
#include "error.h"

#ifdef _WIN32

#include "windows_gl.h"
#include <GL/gl.h>
#include <GL/glu.h>

#else

#include <gl.h>

#endif

void ScreenVertex(float x,float y);
#define kNumBackgrounds 30
GLuint gBackgroundTextures[kNumBackgrounds];
int gBackgroundsLoaded[kNumBackgrounds];
GLuint gBorderTextures[kNumBackgrounds];
int gBordersLoaded[kNumBackgrounds];
int gBackgroundsInited=false;

#define kBackgroundTexSize 128
#define kBorderSize 16



void SetBackground(int id)
{
	if(!gBackgroundsInited)
	{
		int i;
		for(i=0;i<kNumBackgrounds;i++)
		{
			gBackgroundsLoaded[i]=false;
			gBordersLoaded[i]=false;
		}
		gBackgroundsInited=true;
	}
	if(!gBackgroundsLoaded[id-128])
	{
		Ptr backgrTex=GetUnsortedPackEntry(kPackTx16,id,0);
		glGenTextures(1,&gBackgroundTextures[id-128]);
		glBindTexture(GL_TEXTURE_2D,gBackgroundTextures[id-128]);
		glTexImage2D(GL_TEXTURE_2D,0,4,kBackgroundTexSize,kBackgroundTexSize,0,GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,backgrTex);
		gBackgroundsLoaded[id-128]=true;
	}
	else
		glBindTexture(GL_TEXTURE_2D,gBackgroundTextures[id-128]);
}

void SetBorder(int id)
{
	if(!gBordersLoaded[id-1000])
	{
		Ptr backgrTex=GetUnsortedPackEntry(kPackTx16,id,0);
		glGenTextures(1,&gBorderTextures[id-1000]);
		glBindTexture(GL_TEXTURE_2D,gBorderTextures[id-1000]);
		glTexImage2D(GL_TEXTURE_2D,0,4,kBorderSize,kBackgroundTexSize,0,GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,backgrTex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		gBordersLoaded[id-1000]=true;
	}
	else
		glBindTexture(GL_TEXTURE_2D,gBorderTextures[id-1000]);
}

void DrawRoadZoomed16(float xDrawStart,float yDrawStart,float zoom)
{
	float invZoom=1/zoom;
	int i=0;
	int drawing;
	int top=ceil(yDrawStart/2);
/*	for(i=top-240*zoom;i<top;i++)
	{
		tRoad r1=gRoadData+i;
		tRoad r2=gRoadData+i+1;

		SetBackground((*gRoadInfo).backgroundTex);
		glBegin(GL_QUADS);		
		glTexCoord2f((float)xDrawStart/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
		ScreenVertex(0,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(((float)(*r1)[0]-kBorderSize)/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r1)[0]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(((float)(*r2)[0]-kBorderSize)/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r2)[0]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
		glTexCoord2f((float)xDrawStart/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(0,(yDrawStart-i*2-2)*invZoom);
		glEnd();
		
		SetBorder((*gRoadInfo).roadRightBorder);
		glBegin(GL_QUADS);		
		glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r1)[0]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r1)[0]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(1,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r2)[0]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
		glTexCoord2f(0,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r2)[0]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
		glEnd();
		
		if((*r1)[1]!=(*r1)[2]&&(*r2)[1]!=(*r2)[2])
		{
			SetBackground((*gRoadInfo).foregroundTex);
			glBegin(GL_QUADS);		
			glTexCoord2f(((float)(*r1)[0])/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r1)[0]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*r1)[1])/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r1)[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*r2)[1])/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r2)[1]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
			glTexCoord2f(((float)(*r2)[0])/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r2)[0]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
			glEnd();
			
			if((*r1)[2]-(*r1)[1]>2*kBorderSize)
			{
				SetBorder((*gRoadInfo).roadLeftBorder);
				glBegin(GL_QUADS);				
				glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[1]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[1]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glTexCoord2f(0,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[1]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glEnd();

				SetBackground((*gRoadInfo).backgroundTex);
				glBegin(GL_QUADS);				
				glTexCoord2f(((float)(*r1)[1]+kBorderSize)/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[1]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(((float)(*r1)[2]-kBorderSize)/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[2]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(((float)(*r2)[2]-kBorderSize)/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[2]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glTexCoord2f(((float)(*r2)[1]+kBorderSize)/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[1]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glEnd();

				SetBorder((*gRoadInfo).roadRightBorder);
				glBegin(GL_QUADS);				
				glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[2]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[2]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glTexCoord2f(0,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[2]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glEnd();
			}
			else
			{
				SetBorder((*gRoadInfo).roadLeftBorder);
				glBegin(GL_QUADS);				
				glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)((*r1)[1]+(*r1)[2])/2-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)((*r1)[1]+(*r1)[2])/2-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glTexCoord2f(0,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[1]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glEnd();

				SetBorder((*gRoadInfo).roadRightBorder);
				glBegin(GL_QUADS);				
				glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)((*r1)[1]+(*r1)[2])/2-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r1)[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*r2)[2]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glTexCoord2f(0,(i-1)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)((*r1)[1]+(*r1)[2])/2-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
				glEnd();
			}
			
			
			
			SetBackground((*gRoadInfo).foregroundTex);
			glBegin(GL_QUADS);		
			glTexCoord2f(((float)(*r1)[2])/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r1)[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*r1)[3])/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r1)[3]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*r2)[3])/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r2)[3]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
			glTexCoord2f(((float)(*r2)[2])/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r2)[2]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
			glEnd();
		}
		else
		{
			SetBackground((*gRoadInfo).foregroundTex);
			glBegin(GL_QUADS);		
			glTexCoord2f(((float)(*r1)[0])/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r1)[0]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*r1)[3])/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r1)[3]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*r2)[3])/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r2)[3]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
			glTexCoord2f(((float)(*r2)[0])/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r2)[0]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
			glEnd();
		}	
		
		SetBorder((*gRoadInfo).roadLeftBorder);
		glBegin(GL_QUADS);		
		glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r1)[3]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r1)[3]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(1,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r2)[3]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
		glTexCoord2f(0,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r2)[3]-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
		glEnd();
			
			
		SetBackground((*gRoadInfo).backgroundTex);
		glBegin(GL_QUADS);				
		glTexCoord2f(((float)(*r1)[3]+kBorderSize)/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r1)[3]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f((float)(xDrawStart+zoom*gXSize)/kBackgroundTexSize,i/(0.5*kBackgroundTexSize));
		ScreenVertex(gXSize,(yDrawStart-i*2)*invZoom);
		glTexCoord2f((float)(xDrawStart+zoom*gXSize)/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(gXSize,(yDrawStart-i*2-2)*invZoom);
		glTexCoord2f(((float)(*r2)[3]+kBorderSize)/kBackgroundTexSize,(i-1)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r2)[3]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2-2)*invZoom);
		glEnd();
	}*/
	
	glDisable(GL_BLEND);
	SetBackground((*gRoadInfo).backgroundTex);
	glBegin(GL_QUADS);
	glTexCoord2f((float)(xDrawStart+gXDriftPos)/kBackgroundTexSize,(float)(yDrawStart+gYDriftPos)/kBackgroundTexSize);
	ScreenVertex(0,0);
	glTexCoord2f((float)(xDrawStart+gXDriftPos+640*zoom)/kBackgroundTexSize,(float)(yDrawStart+gYDriftPos)/kBackgroundTexSize);
	ScreenVertex(640,0);
	glTexCoord2f((float)(xDrawStart+gXDriftPos+640*zoom)/kBackgroundTexSize,(float)(yDrawStart+gYDriftPos-480*zoom)/kBackgroundTexSize);
	ScreenVertex(640,480);
	glTexCoord2f((float)(xDrawStart+gXDriftPos)/kBackgroundTexSize,(float)(yDrawStart+gYDriftPos-480*zoom)/kBackgroundTexSize);
	ScreenVertex(0,480);
	glEnd();

	SetBackground((*gRoadInfo).foregroundTex);
	glBegin(GL_QUAD_STRIP);
	for(i=top-240*zoom;i<top;i++)
	{
		tRoad r=gRoadData+i;
		glTexCoord2f(((float)(*r)[0]+gXFrontDriftPos)/kBackgroundTexSize,(i+gYFrontDriftPos)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r)[0]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(((float)(*r)[3]+gXFrontDriftPos)/kBackgroundTexSize,(i+gYFrontDriftPos)/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r)[3]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
	}
	glEnd();


		
	SetBorder((*gRoadInfo).roadRightBorder);
	glBegin(GL_QUAD_STRIP);
	for(i=top-240*zoom;i<top;i++)
	{
		tRoad r=gRoadData+i;
		glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r)[0]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r)[0]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);		
	}
	glEnd();

	SetBorder((*gRoadInfo).roadLeftBorder);
	glBegin(GL_QUAD_STRIP);
	for(i=top-240*zoom;i<top;i++)
	{
		tRoad r=gRoadData+i;
		glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r)[3]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
		ScreenVertex(((float)(*r)[3]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
	}
	glEnd();

	SetBackground((*gRoadInfo).backgroundTex);
	drawing=false;
	for(i=top-240*zoom;i<top;i++)
	{
		tRoad r=gRoadData+i;
		if((*r)[2]!=(*r)[1])
		{
			if(!drawing)
			{
				glBegin(GL_QUAD_STRIP);
				drawing=true;
			}
			glTexCoord2f(((float)(*r)[1]+gXDriftPos)/kBackgroundTexSize,(i+gYDriftPos)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r)[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*r)[2]+gXDriftPos)/kBackgroundTexSize,(i+gYDriftPos)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r)[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		}
		else if((*(r+1))[2]!=(*(r+1))[1])
		{
			if(!drawing)
			{
				glBegin(GL_QUAD_STRIP);
				drawing=true;
			}
			glTexCoord2f(((float)(*(r+1))[1]+gXDriftPos)/kBackgroundTexSize,(i+gYDriftPos)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*(r+1))[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(((float)(*(r+1))[2]+gXDriftPos)/kBackgroundTexSize,(i+gYDriftPos)/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*(r+1))[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		}
		else
			if(drawing)
			{
				if(fabs((*r)[3]-(*(r-1))[3])+fabs((*r)[0]-(*(r-1))[0])>50)
				glTexCoord2f(((float)(*(r-1))[1]+gXDriftPos)/kBackgroundTexSize,(i+gYDriftPos)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*(r-1))[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(((float)(*(r-1))[2]+gXDriftPos)/kBackgroundTexSize,(i+gYDriftPos)/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*(r-1))[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);				
				glEnd();
				drawing=false;
			}
	}
	if(drawing)
		glEnd();

	SetBorder((*gRoadInfo).roadLeftBorder);
	drawing=false;
	for(i=top-240*zoom;i<top;i++)
	{
		tRoad r=gRoadData+i;
		if((*r)[1]!=(*r)[2])
		{
			if(!drawing)
			{
				glBegin(GL_QUAD_STRIP);
				drawing=true;
			}
			glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r)[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
			if((*r)[2]-(*r)[1]>2*kBorderSize)
				ScreenVertex(((float)(*r)[1]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			else
				ScreenVertex((((*r)[1]+(*r)[2])/2.0-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		}
		else if((*(r+1))[2]!=(*(r+1))[1])
		{
			if(!drawing)
			{
				glBegin(GL_QUAD_STRIP);
				drawing=true;
			}
			glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*(r+1))[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
			if((*(r+1))[2]-(*(r+1))[1]>2*kBorderSize)
				ScreenVertex(((float)(*(r+1))[1]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			else
				ScreenVertex((((*(r+1))[1]+(*(r+1))[2])/2.0-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		}
		else
			if(drawing)
			{
				glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*(r-1))[1]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
				if((*(r-1))[2]-(*(r-1))[1]>2*kBorderSize)
					ScreenVertex(((float)(*(r-1))[1]+kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				else
					ScreenVertex((((*(r-1))[1]+(*(r-1))[2])/2.0-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glEnd();
				drawing=false;
			}
	}
	if(drawing)
		glEnd();

	SetBorder((*gRoadInfo).roadRightBorder);
	drawing=false;
	for(i=top-240*zoom;i<top;i++)
	{
		tRoad r=gRoadData+i;
		if((*r)[1]!=(*r)[2])
		{
			if(!drawing)
			{
				glBegin(GL_QUAD_STRIP);
				drawing=true;
			}
			glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
			if((*r)[2]-(*r)[1]>2*kBorderSize)
				ScreenVertex(((float)(*r)[2]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			else
				ScreenVertex((((*r)[1]+(*r)[2])/2.0-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*r)[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		}
		else if((*(r+1))[2]!=(*(r+1))[1])
		{
			if(!drawing)
			{
				glBegin(GL_QUAD_STRIP);
				drawing=true;
			}
			glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
			if((*(r+1))[2]-(*(r+1))[1]>2*kBorderSize)
				ScreenVertex(((float)(*(r+1))[2]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			else
				ScreenVertex((((*(r+1))[1]+(*(r+1))[2])/2.0-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
			glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
			ScreenVertex(((float)(*(r+1))[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
		}
		else
			if(drawing)
			{
				glTexCoord2f(0,i/(0.5*kBackgroundTexSize));
				if((*(r-1))[2]-(*(r-1))[1]>2*kBorderSize)
					ScreenVertex(((float)(*(r-1))[2]-kBorderSize-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				else
					ScreenVertex((((*(r-1))[1]+(*(r-1))[2])/2.0-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glTexCoord2f(1,i/(0.5*kBackgroundTexSize));
				ScreenVertex(((float)(*(r-1))[2]-xDrawStart)*invZoom,(yDrawStart-i*2)*invZoom);
				glEnd();
				drawing=false;
			}
	}
	if(drawing)
		glEnd();

	glEnable(GL_BLEND);
}

