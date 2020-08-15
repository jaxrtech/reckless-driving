#include <math.h>
#include <gl.h>

#include "objects.h"
#include "screen.h"
#include "sprites.h"
#include "vec2d.h"
#include "trig.h"
#include "textfx.h"
#include "renderframe.h"
#include "gameinitexit.h"
#include "preferences.h"
#include "particlefx.h"
#include "packs.h"
#include "rle.h"
#include "gamesounds.h"
#include "interface.h"

void DrawRoadZoomed(float,float,float);
void DrawRoadZoomed16(float,float,float);

#define kXCameraScreenPos 0.5					
#define kYCameraScreenPos 0.55
//horizontal and vertical position of the object the camera is looking at on screen, => 0,5/0,5 is center.

#define kPanelHeight	121		//heigth of the cockpit graphic on screen in pixels.

#define kMaxDisplayVelo	89.408
#define kZoomVeloFactor 50

#define kTrackSize		3.4		//width of the Tracks left by cars in pixels
#define kTrackLifeTime		(14.0*kCalcFPS)	//time after which rubber tracks left by cars get removed in frames.
#define kTrackDeathDuration	(0.8*kCalcFPS)	//time it takes for rubber tracks to fade out in frames.

#define kDigitOffs		18
#define kTimeDecimalOffs	(kDigitOffs+3)
#define kTimeMinuteOffs		(-2*kDigitOffs-5)
#define kScoreX			610
#define kScoreY			51
#define kLiveX			613
#define kLiveY			76
#define kTimeX			586
#define kTimeY			26
#define kAddOnX			292
#define kAddOnY			25

#define kLifeBarWidth	158
#define kLifeBarY		27
#define kLifeBarX		227
#define kLifeBarFrameId	156

#define kComplYSize		370
#define kTimeTakeX		302
#define kTimeTakeY		204
#define kTimeBestX		302
#define kTimeBestY		233
#define kTimeLeftX		438
#define kTimeLeftY		282
#define kTimeBonusX		459
#define kTimeBonusY		311
#define kTimeBonus2X	459
#define kTimeBonus2Y	342

#define kScrollComplDelay	1.5
#define kShowComplDelay		4
#define kShowBestTimeDelay	6
#define kShowBonusTimeDelay	8
#define kStartBonusDelay	10	
#define kCountBonusSpeed	0.1
#define kCountNoiseSpeed	0.1
#define kMultiplyDelay		1
#define kCloseDelay			3.5

#define kMaxMarkLength	128

int gBonusCount;
int gNoBonusScore;

void DrawNum(int h,int v,int num,int digits)
{
	int i;
	for(i=0;i<digits;i++)
	{
		if(num/(int)pow(10,i)||!i)
			DrawRLE(h,v,128+(num/(int)pow(10,i))%10);
		h-=kDigitOffs;
	}
}

void DrawNumZeroed(int h,int v,int num,int digits)
{
	int i;
	for(i=0;i<digits;i++)
	{
		DrawRLE(h,v,128+(num/(int)pow(10,i))%10);
		h-=kDigitOffs;
	}
}

int DrawComplCount()
{
	float timeDiff=gLevelData->time-gGameTime>0?gLevelData->time-gGameTime:0;
	float countTime=timeDiff*kCountBonusSpeed;
	int bonus=timeDiff*10;
	if(gFinishDelay>=kShowComplDelay) 
		DrawRLE(0,gYSize/2-kComplYSize/2,149);	
	else if(gFinishDelay>=kScrollComplDelay) 
	{
		int yPos=(gYSize/2+kComplYSize/2)*sqrt((gFinishDelay-kScrollComplDelay)/(kShowComplDelay-kScrollComplDelay))-kComplYSize;
		DrawRLEYClip(0,yPos,149);	
		gNoBonusScore=gPlayerScore;
		gBonusCount=0;
	}
	if(gFinishDelay>=kShowBestTimeDelay&&!gLevelResFile)
	{
		if(gPrefs.lapRecords[gLevelID]>gGameTime)
		{
			gPrefs.lapRecords[gLevelID]=gGameTime;
			SimplePlaySound(155);
		}
		DrawNum(kTimeBestX+kTimeMinuteOffs,kTimeBestY,floor(gPrefs.lapRecords[gLevelID])/60,2);
		DrawNumZeroed(kTimeBestX,kTimeBestY,(int)floor(gPrefs.lapRecords[gLevelID])%60,2);
		DrawNum(kTimeBestX+kTimeDecimalOffs,kTimeBestY,(int)floor(gPrefs.lapRecords[gLevelID]*10),1);
		DrawNum(kTimeTakeX+kTimeMinuteOffs,kTimeTakeY,floor(gGameTime)/60,2);
		DrawNumZeroed(kTimeTakeX,kTimeTakeY,(int)floor(gGameTime)%60,2);
		DrawNum(kTimeTakeX+kTimeDecimalOffs,kTimeTakeY,(int)floor(gGameTime*10)%10,1);
	}	
	if(gFinishDelay>=kStartBonusDelay)
	{
		float bonusPerc=(gFinishDelay-kStartBonusDelay)/countTime;
		if(bonusPerc>1)bonusPerc=1;
		if(bonus*bonusPerc*kCountNoiseSpeed>gBonusCount)
		{
			gBonusCount++;
			SimplePlaySound(148);
		}
		DrawNum(kTimeLeftX+kTimeMinuteOffs,kTimeLeftY,floor(timeDiff-timeDiff*bonusPerc)/60,2);
		DrawNumZeroed(kTimeLeftX,kTimeLeftY,(int)floor(timeDiff-timeDiff*bonusPerc)%60,2);
		DrawNum(kTimeLeftX+kTimeDecimalOffs,kTimeLeftY,(int)floor((timeDiff-timeDiff*bonusPerc)*10),1);
		DrawNum(kTimeBonusX,kTimeBonusY,bonus*bonusPerc,5);
		gPlayerScore=gNoBonusScore+bonus*bonusPerc;
	}	
	else if(gFinishDelay>=kShowBonusTimeDelay)
	{
		DrawNum(kTimeLeftX+kTimeMinuteOffs,kTimeLeftY,floor(timeDiff)/60,2);
		DrawNumZeroed(kTimeLeftX,kTimeLeftY,(int)floor(timeDiff)%60,2);
		DrawNum(kTimeLeftX+kTimeDecimalOffs,kTimeLeftY,(int)floor(timeDiff*10),1);
		DrawNum(kTimeBonusX,kTimeBonusY,0,5);
	}	
	if(gFinishDelay>=kStartBonusDelay+countTime+kMultiplyDelay)
		if(gPlayerBonus!=1)
		{	
			if(gBonusCount!=32767)
			{
				SimplePlaySound(141);
				gBonusCount=32767;
			}
			DrawRLE(kTimeBonus2X-5*kDigitOffs,kTimeBonus2Y,137+gPlayerBonus);
			DrawNum(kTimeBonus2X,kTimeBonus2Y,bonus*gPlayerBonus,5);	
		}
	if(gFinishDelay>=kStartBonusDelay+countTime+kCloseDelay)
	{
		gPlayerScore=gNoBonusScore+bonus*gPlayerBonus;
		DisposeLevel();
		gLevelID++;
		return LoadLevel();
	}
	return true;
}	

void DrawDisplays()
{
	int i;
	float timeDisp=gLevelData->time-gGameTime>0?gLevelData->time-gGameTime:0;
	float veloDisplay;
	if(!gCameraObj->jumpHeight)
		veloDisplay=-PI/2+VEC2D_DotProduct(gCameraObj->velo,P2D(sin(gCameraObj->dir),cos(gCameraObj->dir)))*PI/kMaxDisplayVelo;
	else
		veloDisplay=-PI/2+gCameraObj->input.throttle*PI;
	DrawRLE(0,gYSize-kPanelHeight,148);	
	DrawSprite(189,148,gYSize-14,veloDisplay,1);
	DrawNum(kScoreX,gYSize-kScoreY,gDisplayScore,6);
	DrawNum(kLiveX,gYSize-kLiveY,gPlayerLives,2);
	DrawNum(kTimeX+kTimeMinuteOffs,gYSize-kTimeY,floor(timeDisp)/60,2);
	DrawNumZeroed(kTimeX,gYSize-kTimeY,(int)floor(timeDisp)%60,2);
	DrawNum(kTimeX+kTimeDecimalOffs,gYSize-kTimeY,(int)floor(timeDisp*10),1);
	for(i=0;i<6;i++)
		if(gPlayerAddOns&1<<i)
			DrawRLE(kAddOnX+20*i,gYSize-kAddOnY,142+i);
	if(gPlayerBonus!=1)
		DrawRLE(kAddOnX+6*20,gYSize-kAddOnY,137+gPlayerBonus);
	if(gNumMissiles)
	{
		Str31 numStr;
		DrawRLE(gXSize-120,5,154);
		NumToString(gNumMissiles,numStr);
		MakeFXStringFromNumStr(numStr,numStr);
		SimpleDrawText(numStr,gXSize-60,5);
	}
	if(gNumMines)
	{
		Str31 numStr;
		DrawRLE(gXSize-120,gNumMissiles?30:5,155);
		NumToString(gNumMines,numStr);
		MakeFXStringFromNumStr(numStr,numStr);
		SimpleDrawText(numStr,gXSize-60,gNumMissiles?30:5);
	}
}

int DrawPanel()
{
	if(gPlayerLives) 
		if(gFinishDelay) 
			return DrawComplCount();
		else if(gPlayerDeathDelay)
			DrawRLE(0,gYSize-kPanelHeight,150);	
		else 
			DrawDisplays();
	return true;
}


#pragma global_optimizer off
void ScreenVertex(float x,float y);

void DrawTracksZoomed16(float xDrawStart,float yDrawStart,float zoom)
{
	float invZoom=1/zoom;
	SInt32 fixZoom=zoom*65536.0;
	int size=kTrackSize*invZoom;
	UInt32 shiftClip=gXSize-size<<16;
	int i;
	int y1Clip=yDrawStart-(gYSize-(gFinishDelay?0:kInvLines))*zoom;
	UInt16 *textures=GetUnsortedPackEntry(kPackTx16,(*gRoadInfo).tracks,0);
	
	float r=((*(textures+3*128*128)>>10)&0x1f)/32.0;
	float g=((*(textures+3*128*128)>>5)&0x1f)/32.0;
	float b=(*(textures+3*128*128)&0x1f)/32.0;
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	for(i=0;i<gTrackCount;i++)
	{
		if(gTracks[i].p2.y<=yDrawStart+size&&gTracks[i].p1.y>y1Clip&&gTracks[i].time+kTrackLifeTime+kTrackDeathDuration>gFrameCount)
		{
			t2DPoint v1=gTracks[i].p1,v2=gTracks[i].p1,v3=gTracks[i].p2,v4=gTracks[i].p2;
			float offsX=gTracks[i].p2.x-gTracks[i].p1.x;
			float offsY=gTracks[i].p2.y-gTracks[i].p1.y;
			float l=sqrt(offsX*offsX+offsY*offsY);
			float intensity=gTracks[i].intensity*3*((gTracks[i].time+kTrackLifeTime>gFrameCount)?1:1-(gFrameCount-gTracks[i].time-kTrackLifeTime)/kTrackDeathDuration);
			l/=2;
			
			if(l!=0)
			{
				glColor4f(r,g,b,intensity/5);
			
				v1.x-=offsY/l;v1.y+=offsX/l;
				v2.x+=offsY/l;v2.y-=offsX/l;
				v3.x+=offsY/l;v3.y-=offsX/l;
				v4.x-=offsY/l;v4.y+=offsX/l;
				v1.x=(v1.x-xDrawStart)*invZoom;
				v1.y=(yDrawStart-v1.y)*invZoom;
				v2.x=(v2.x-xDrawStart)*invZoom;
				v2.y=(yDrawStart-v2.y)*invZoom;
				v3.x=(v3.x-xDrawStart)*invZoom;
				v3.y=(yDrawStart-v3.y)*invZoom;
				v4.x=(v4.x-xDrawStart)*invZoom;
				v4.y=(yDrawStart-v4.y)*invZoom;
				
				if(isnan(v1.x+v2.x+v3.x+v4.x+v1.y+v2.y+v3.y+v4.y))
					SysBeep(0);
				ScreenVertex(v1.x,v1.y);
				ScreenVertex(v2.x,v2.y);
				ScreenVertex(v3.x,v3.y);
				ScreenVertex(v4.x,v4.y);
			}
		}
	}

	glEnd();
	glColor4f(1,1,1,1);
	glEnable(GL_TEXTURE_2D);
}

void DrawMarksZoomed16(float xDrawStart,float yDrawStart,float zoom)
{
	float invZoom=1/zoom;
	SInt32 fixZoom=zoom*65536.0;
	int size=4.2*invZoom;
	int i;
	int y1Clip=yDrawStart-(gYSize-(gFinishDelay?0:kInvLines))*zoom;
//	UInt16 *texture=GetUnsortedPackEntry(kPackTx16,(*gRoadInfo).marks,0);
	int l=0,r=gMarkSize;
  	while(r-1>l)
		if(gMarks[(l+r)/2].p1.y+gMarks[(l+r)/2].p2.y>yDrawStart*2+kMaxMarkLength)
			l=(l+r)/2;
		else
			r=(l+r)/2;
	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,0.5);
	glBegin(GL_QUADS);

	for(i=l;i<gMarkSize&&gMarks[i].p1.y+gMarks[i].p2.y>y1Clip*2-kMaxMarkLength;i++)
	{
		if(gMarks[i].p2.y<=yDrawStart+size&&gMarks[i].p1.y>y1Clip)
		{
			t2DPoint v1=gMarks[i].p1,v2=gMarks[i].p1,v3=gMarks[i].p2,v4=gMarks[i].p2;
			float offsX=gMarks[i].p2.x-gMarks[i].p1.x;
			float offsY=gMarks[i].p2.y-gMarks[i].p1.y;
			float l=sqrt(offsX*offsX+offsY*offsY);
			l/=2;
			
			if(l!=0)
			{
				v1.x-=offsY/l;v1.y+=offsX/l;
				v2.x+=offsY/l;v2.y-=offsX/l;
				v3.x+=offsY/l;v3.y-=offsX/l;
				v4.x-=offsY/l;v4.y+=offsX/l;
				v1.x=(v1.x-xDrawStart)*invZoom;
				v1.y=(yDrawStart-v1.y)*invZoom;
				v2.x=(v2.x-xDrawStart)*invZoom;
				v2.y=(yDrawStart-v2.y)*invZoom;
				v3.x=(v3.x-xDrawStart)*invZoom;
				v3.y=(yDrawStart-v3.y)*invZoom;
				v4.x=(v4.x-xDrawStart)*invZoom;
				v4.y=(yDrawStart-v4.y)*invZoom;

				if(isnan(v1.x+v2.x+v3.x+v4.x+v1.y+v2.y+v3.y+v4.y))
					SysBeep(0);
				ScreenVertex(v1.x,v1.y);
				ScreenVertex(v2.x,v2.y);
				ScreenVertex(v3.x,v3.y);
				ScreenVertex(v4.x,v4.y);
			}
		}
	}

	glEnd();
	glColor4f(1,1,1,1);
	glEnable(GL_TEXTURE_2D);
}

#pragma global_optimizer default

void DrawSpriteLayerZoomed(float xDrawStart,float yDrawStart,float zoom,float tide,int layer)
{
	tObject *theObj=gFirstVisObj;
	float invZoom=1/zoom;
	float maxDrawOffs=128*invZoom;
	while(theObj!=gLastVisObj)
	{
		if(theObj->layer==layer)
			if(theObj->frame&&!theObj->jumpHeight)
			{
				float x=(theObj->pos.x-xDrawStart)*invZoom;
				float y=(yDrawStart-theObj->pos.y)*invZoom;
				if((y>-maxDrawOffs)&&(y<gYSize+maxDrawOffs))
				{
					float objTide=((*gRoadInfo).water&&(*theObj->type).flags2&kObjectFloating)?1+tide*0.5+tide*VEC2D_Value(theObj->velo)*0.04:1;
					DrawSprite(theObj->frame,x,y,theObj->dir,objTide*invZoom);
				}
			}
		theObj=(tObject*)theObj->next;
	}
}

void DrawSpriteLayerBlurZoomed(float xDrawStart,float yDrawStart,float zoom,float tide,int layer)
{
	tObject *theObj=gFirstVisObj;
	float invZoom=1/zoom;
	float maxDrawOffs=128*invZoom;
	while(theObj!=gLastVisObj)
	{
		if(theObj->layer==layer)
			if(theObj->frame&&!theObj->jumpHeight)
			{
				float x=(theObj->pos.x-xDrawStart)*invZoom;
				float y=(yDrawStart-theObj->pos.y)*invZoom;
				if((y>-maxDrawOffs)&&(y<gYSize+maxDrawOffs))
				{
					t2DPoint velDiff=VEC2D_Difference(gCameraObj->velo,theObj->velo);
					float objTide=((*gRoadInfo).water&&(*theObj->type).flags2&kObjectFloating)?1+tide*0.5+tide*VEC2D_Value(theObj->velo)*0.04:1;
					DrawSprite(theObj->frame,x,y,theObj->dir,objTide*invZoom);
					if(velDiff.x*velDiff.x+velDiff.y*velDiff.y>35*35)
					{
						t2DPoint fuzzPos=VEC2D_Scale(velDiff,kFrameDuration*kScale*0.2);
						DrawSpriteTranslucent(theObj->frame,x+fuzzPos.x,y+fuzzPos.y,theObj->dir-theObj->rotVelo*kFrameDuration,objTide*invZoom);					
					}
				}
			}
		theObj=(tObject*)theObj->next;
	}
}

void DrawSpritesZoomed(float xDrawStart,float yDrawStart,float zoom)
{
	float invZoom=1/zoom;
	int layer;
	float tide=0.05*sin(gGameTime*2.6);
	tObject	*theObj;
	for(layer=0;layer<kNumLayers;layer++)
		if(gPrefs.motionBlur)
			DrawSpriteLayerBlurZoomed(xDrawStart,yDrawStart,zoom,tide,layer);
		else
			DrawSpriteLayerZoomed(xDrawStart,yDrawStart,zoom,tide,layer);
	theObj=gFirstVisObj;
	while(theObj!=gLastVisObj)
	{
		if(theObj->frame&&theObj->jumpHeight)
		{
			float x=(theObj->pos.x-xDrawStart)*invZoom;
			float y=(yDrawStart-theObj->pos.y)*invZoom;
			if((y>-256)&&(y<gYSize+256))
				DrawSprite(theObj->frame,x,y,theObj->dir,(theObj->jumpHeight*0.18+1)*invZoom);
		}
		theObj=(tObject*)theObj->next;
	}
}

void RenderFrame()
{
	float zoom=0.5+gZoomVelo/kZoomVeloFactor+gCameraObj->jumpHeight*0.05;
	float xDrawStart=gCameraObj->pos.x-gXSize*kXCameraScreenPos*zoom;
	float yDrawStart=gCameraObj->pos.y+gYSize*kYCameraScreenPos*zoom;
	int preSpecBlit=gScreenBlitSpecial;
	if(gFinishDelay)
		yDrawStart=gLevelData->levelEnd+gYSize*kYCameraScreenPos*zoom;
	glClear(GL_COLOR_BUFFER_BIT);
	if(gPrefs.hiColor){
		DrawRoadZoomed16(xDrawStart,yDrawStart,zoom);
		DrawMarksZoomed16(xDrawStart,yDrawStart,zoom);
		DrawTracksZoomed16(xDrawStart,yDrawStart,zoom);
	}
	DrawParticleFXZoomed(xDrawStart,yDrawStart,zoom,0);	
	DrawSpritesZoomed(xDrawStart,yDrawStart,zoom);
	DrawParticleFXZoomed(xDrawStart,yDrawStart,zoom,1);	
	DrawTextFXZoomed(xDrawStart,yDrawStart,zoom);
	if(DrawPanel())
		if(preSpecBlit!=gScreenBlitSpecial)
			RenderFrame();
		else
			Blit2Screen();
}