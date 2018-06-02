#include <math.h>
#include "gl_platform.h"
#include "objects.h"
#include "screen.h"
#include "error.h"
#include "trig.h"
#include "packs.h"
#include "preferences.h"
#include "random.h"

#define kNumSprites			300
#define kNumSpecialSprites	100
#define kLifeBarId			318

enum{
	kDrawModeMaskColor=0,
	kDrawModeTransparent=1<<0,
	kDrawModeDeepMask=1<<1,
	kDrawModeDoubleSize=1<<2
};

typedef struct{
	UInt16 xSize,ySize;
	UInt8 log2xSize;
	UInt8 filler1;	
	UInt8 drawMode;	
	UInt8 filler2;
	UInt8 data[1];
} tSpriteHeader;

typedef struct{
	UInt16 xSize,ySize;
	UInt8 log2xSize;
	UInt8 filler1;	
	UInt8 drawMode;	
	UInt8 filler2;
	UInt16 data[1];
} tSpriteHeader16;

typedef struct{
	float x,y;
	int u,v;
} tVertex;

Handle gSprites[kNumSprites+kNumSpecialSprites];
GLuint gSpriteRefs[kNumSprites+kNumSpecialSprites];
void LoadSpriteGL(int id);

void ScreenVertex(float x,float y)
{
	glVertex2f((x-320)*0.00175,-(y-240)*0.00175);
}

void DrawSpriteAlpha(int id, float cx, float cy, float dir, float zoom,float alpha)
{
	tSpriteHeader *sprite=(tSpriteHeader*)*(gSprites[id-128]);
	if(sprite->drawMode&kDrawModeDoubleSize)
		zoom*=0.5;
	if(sprite->drawMode&kDrawModeTransparent)
		glColor4f(1,1,1,0.5*alpha);
	else
		glColor4f(1,1,1,alpha);
	{
		int y,y2,i;
		tVertex vertices[4];
		float dirCos=cos(dir);
		float dirSin=sin(dir);
		int dudx=dirCos*256/zoom;
		int dvdx=-dirSin*256/zoom;
		dirCos*=zoom;
		dirSin*=zoom;	

		vertices[0].x=cx-(dirCos*sprite->xSize/2)+(dirSin*sprite->ySize/2);
		vertices[1].x=cx+(dirCos*sprite->xSize/2)+(dirSin*sprite->ySize/2);
		vertices[2].x=cx+(dirCos*sprite->xSize/2)-(dirSin*sprite->ySize/2);
		vertices[3].x=cx-(dirCos*sprite->xSize/2)-(dirSin*sprite->ySize/2);
		vertices[0].y=cy-(dirSin*sprite->xSize/2)-(dirCos*sprite->ySize/2);
		vertices[1].y=cy+(dirSin*sprite->xSize/2)-(dirCos*sprite->ySize/2);
		vertices[2].y=cy+(dirSin*sprite->xSize/2)+(dirCos*sprite->ySize/2);
		vertices[3].y=cy-(dirSin*sprite->xSize/2)+(dirCos*sprite->ySize/2);
		vertices[0].u=0;
		vertices[1].u=1;
		vertices[2].u=1;
		vertices[3].u=0;
		vertices[0].v=0;
		vertices[1].v=0;
		vertices[2].v=1;
		vertices[3].v=1;
		
		glBindTexture(GL_TEXTURE_2D,gSpriteRefs[id-128]);
		glBegin(GL_QUADS);
		for(i=0;i<4;i++)
		{
			glTexCoord2f(vertices[i].u,vertices[i].v);
			ScreenVertex(vertices[i].x,vertices[i].y);
		}
		glEnd();
	}
	glColor4f(1,1,1,1);
}


void DrawSprite(int id, float cx, float cy, float dir, float zoom)
{
	DrawSpriteAlpha(id,cx,cy,dir,zoom,1);
}

void DrawSpriteTranslucent(int id, float cx, float cy, float dir, float zoom)
{
	DrawSpriteAlpha(id,cx,cy,dir,zoom,0.5);
}

int GetUniqueSpriteNum()
{
	int i;
	for(i=kNumSprites;i<kNumSprites+kNumSpecialSprites;i++)
		if(!gSprites[i])return i;
	return 0;
}

void XDistortSprite8(tSpriteHeader *sprite,int startY,int endY,int startX,int endX,int dir,float damage)
{
	UInt8 *lineStart;
	int line;
	lineStart=&sprite->data[sprite->xSize*startY+startX];
	if(damage>2)damage=2;
	if(!dir)
		for(line=0;line<endY;line++)
		{
			int i;
			float c=(float)line/endY;
			float crunch=(-c*c+c)*(1+RanFl(-0.3,0.3))*endX*damage;
			if(crunch){
				float crunchFactor=endX/(endX-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endX-crunch);
				for(i=endX;i>(int)crunch;i--){
					UInt8 pixelValue=lineStart[(int)(endX-((endX-i)*crunchFactor))];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*((endX-i)/crunchLength);
						lineStart[i]=gLightningTab[shade][pixelValue];
					}
				}
				for(i=0;i<=(int)crunch;i++)
					lineStart[i]=sprite->data[0];
			}
			lineStart+=sprite->xSize;
		}
	else for(line=0;line<endY;line++)
		{
			int i;
			float c=(float)line/endY;
			float crunch=(-c*c+c)*(1+RanFl(-0.3,0.3))*endX*damage;
			if(crunch){
				float crunchFactor=endX/(endX-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endX-crunch);
				for(i=0;i<endX-(int)crunch;i++){
					UInt8 pixelValue=lineStart[(int)(i*crunchFactor)];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*(i/crunchLength);
						lineStart[i]=gLightningTab[shade][pixelValue];
					}
				}				
				for(i=endX;i>=endX-(int)crunch;i--)
					lineStart[i]=sprite->data[0];
			}
			lineStart+=sprite->xSize;
		}	
}

void XDistortSprite16(tSpriteHeader16 *sprite,int startY,int endY,int startX,int endX,int dir,float damage)
{
	UInt16 *lineStart;
	int line;
	lineStart=&sprite->data[sprite->xSize*startY+startX];
	if(damage>2)damage=2;
	if(!dir)
		for(line=0;line<endY;line++)
		{
			int i;
			float c=(float)line/endY;
			float crunch=(-c*c+c)*(1+RanFl(-0.5,0.5))*endX*damage;
			if(crunch){
				float crunchFactor=endX/(endX-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endX-crunch);
				for(i=endX;i>(int)crunch;i--){
					UInt16 pixelValue=lineStart[(int)(endX-((endX-i)*crunchFactor))];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*((endX-i)/crunchLength);
						lineStart[i]=ShadeRGB16(shade,pixelValue);
					}
				}
				for(i=0;i<=(int)crunch;i++)
					lineStart[i]=sprite->data[0];
			}
			lineStart+=sprite->xSize;
		}
	else for(line=0;line<endY;line++)
		{
			int i;
			float c=(float)line/endY;
			float crunch=(-c*c+c)*(1+RanFl(-0.5,0.5))*endX*damage;
			if(crunch){
				float crunchFactor=endX/(endX-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endX-crunch);
				for(i=0;i<endX-(int)crunch;i++){
					UInt16 pixelValue=lineStart[(int)(i*crunchFactor)];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*(i/crunchLength);
						lineStart[i]=ShadeRGB16(shade,pixelValue);
					}
				}				
				for(i=endX;i>=endX-(int)crunch;i--)
					lineStart[i]=sprite->data[0];
			}
			lineStart+=sprite->xSize;
		}	
}

int XDistortSprite(int id,int startY,int endY,int startX,int endX,int dir,float damage)
{
	tSpriteHeader *sprite;
	if(id-128<kNumSprites)
	{
		int newID=GetUniqueSpriteNum();
		if(!newID) return id;
		gSprites[newID]=gSprites[id-128];
		HandToHand(&gSprites[newID]);	
		DoError(MemError());	
		sprite=(tSpriteHeader*)*(gSprites[newID]);
		id=newID+128;
	}
	else
	{
		sprite=(tSpriteHeader*)*(gSprites[id-128]);
		glDeleteTextures(1,&gSpriteRefs[id-128]);
	}
	if(sprite->drawMode&kDrawModeDoubleSize)
	{
		startY*=2;
		endY*=2;
		startX*=2;
		endX*=2;
	}
	endY-=startY;
	endX-=startX;
	startX+=sprite->xSize/2;
	startY+=sprite->ySize/2;
	if(startX<0){endX+=startX;startX=0;}
	if(startY<0){endY+=startY;startY=0;}
	if(startX+endX>=sprite->xSize){endX-=startX+endX+1-sprite->xSize;}
	if(startY+endY>=sprite->ySize){endY-=startY+endY+1-sprite->ySize;}
	if(gPrefs.hiColor)
		XDistortSprite16(sprite,startY,endY,startX,endX,dir,damage);
	else
		XDistortSprite8(sprite,startY,endY,startX,endX,dir,damage);
	LoadSpriteGL(id);
	return id;
}

void YDistortSprite8(tSpriteHeader *sprite,int startX,int endX,int startY,int endY,int dir,float damage)
{
	UInt8 *collumStart;
	int collum;
	collumStart=&sprite->data[sprite->xSize*startY+startX];
	if(damage>2)damage=2;
	if(!dir)
		for(collum=0;collum<endX;collum++)
		{
			int i;
			float c=(float)collum/endX;
			float crunch=(-c*c+c)*(1+RanFl(-0.5,0.5))*endY*damage;
			if(crunch){
				float crunchFactor=endY/(endY-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endY-crunch);
				for(i=endY;i>(int)crunch;i--){
					UInt8 pixelValue=collumStart[(int)(endY-((endY-i)*crunchFactor))*sprite->xSize];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*((endY-i)/crunchLength);
						collumStart[i*sprite->xSize]=gLightningTab[shade][pixelValue];
					}
				}
				for(i=0;i<=(int)crunch;i++)
					collumStart[i*sprite->xSize]=sprite->data[0];
			}
			collumStart++;
		}
	else for(collum=0;collum<endX;collum++)
		{
			int i;
			float c=(float)collum/endX;
			float crunch=(-c*c+c)*(1+RanFl(-0.5,0.5))*endY*damage;
			if(crunch){
				float crunchFactor=endY/(endY-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endY-crunch);
				for(i=0;i<endY-(int)crunch;i++){
					UInt8 pixelValue=collumStart[(int)(i*crunchFactor)*sprite->xSize];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*(i/crunchLength);
						collumStart[i*sprite->xSize]=gLightningTab[shade][pixelValue];
					}
				}
				for(i=endY;i>=endY-(int)crunch;i--)
					collumStart[i*sprite->xSize]=sprite->data[0];
			}
			collumStart++;
		}
}

void YDistortSprite16(tSpriteHeader16 *sprite,int startX,int endX,int startY,int endY,int dir,float damage)
{
	UInt16 *collumStart;
	int collum;
	collumStart=&sprite->data[sprite->xSize*startY+startX];
	if(damage>2)damage=2;
	if(!dir)
		for(collum=0;collum<endX;collum++)
		{
			int i;
			float c=(float)collum/endX;
			float crunch=(-c*c+c)*(1+RanFl(-0.5,0.5))*endY*damage;
			if(crunch){
				float crunchFactor=endY/(endY-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endY-crunch);
				for(i=endY;i>(int)crunch;i--){
					UInt16 pixelValue=collumStart[(int)(endY-((endY-i)*crunchFactor))*sprite->xSize];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*((endY-i)/crunchLength);
						collumStart[i*sprite->xSize]=ShadeRGB16(shade,pixelValue);
					}
				}
				for(i=0;i<=(int)crunch;i++)
					collumStart[i*sprite->xSize]=sprite->data[0];
			}
			collumStart++;
		}
	else for(collum=0;collum<endX;collum++)
		{
			int i;
			float c=(float)collum/endX;
			float crunch=(-c*c+c)*(1+RanFl(-0.5,0.5))*endY*damage;
			if(crunch){
				float crunchFactor=endY/(endY-crunch);
				float invCrunchFac=1/crunchFactor;
				float maxShade=(kLightValues-1)*(1-invCrunchFac);
				float crunchLength=(endY-crunch);
				for(i=0;i<endY-(int)crunch;i++){
					UInt16 pixelValue=collumStart[(int)(i*crunchFactor)*sprite->xSize];
					if(pixelValue!=sprite->data[0]){
						int shade=(kLightValues-1)-maxShade*(i/crunchLength);
						collumStart[i*sprite->xSize]=ShadeRGB16(shade,pixelValue);
					}
				}
				for(i=endY;i>=endY-(int)crunch;i--)
					collumStart[i*sprite->xSize]=sprite->data[0];
			}
			collumStart++;
		}
}

int YDistortSprite(int id,int startX,int endX,int startY,int endY,int dir,float damage)
{
	tSpriteHeader *sprite;
	if(id-128<kNumSprites)
	{
		int newID=GetUniqueSpriteNum();
		if(!newID) return id;
		gSprites[newID]=gSprites[id-128];
		HandToHand(&gSprites[newID]);		
		DoError(MemError());	
		sprite=(tSpriteHeader*)*(gSprites[newID]);
		id=newID+128;
	}
	else
	{
		sprite=(tSpriteHeader*)*(gSprites[id-128]);
		glDeleteTextures(1,&gSpriteRefs[id-128]);
	}
	if(sprite->drawMode&kDrawModeDoubleSize)
	{
		startY*=2;
		endY*=2;
		startX*=2;
		endX*=2;
	}
	endY-=startY;
	endX-=startX;
	startX+=sprite->xSize/2;
	startY+=sprite->ySize/2;
	if(startX<0){endX+=startX;startX=0;}
	if(startY<0){endY+=startY;startY=0;}
	if(startX+endX>=sprite->xSize){endX-=startX+endX+1-sprite->xSize;}
	if(startY+endY>=sprite->ySize){endY-=startY+endY+1-sprite->ySize;}
	if(gPrefs.hiColor)
		YDistortSprite16(sprite,startX,endX,startY,endY,dir,damage);
	else
		YDistortSprite8(sprite,startX,endX,startY,endY,dir,damage);
	LoadSpriteGL(id);
	return id;
}
/*
int BulletHitSprite8(tSpriteHeader16 *sprite,int x,int y)
{
	int offs;
	if(y-1>=0&&y-1<sprite->ySize)
	{
		if(x-1>=0&&x-1<sprite->xSize) 
			if(sprite->data[offs=((y-1)*sprite->xSize+x-1)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.9)][sprite->data[offs]];
		if(x>=0&&x<sprite->xSize) 
			if(sprite->data[offs=((y-1)*sprite->xSize+x)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.7)][sprite->data[offs]];
		if(x+1>=0&&x+1<sprite->xSize) 
			if(sprite->data[offs=((y-1)*sprite->xSize+x+1)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.9)][sprite->data[offs]];
	}
	if(y>=0&&y<sprite->ySize)
	{
		if(x-1>=0&&x-1<sprite->xSize) 
			if(sprite->data[offs=(y*sprite->xSize+x-1)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.7)][sprite->data[offs]];
		if(x>=0&&x<sprite->xSize) 
			if(sprite->data[offs=(y*sprite->xSize+x)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.6)][sprite->data[offs]];
		if(x+1>=0&&x+1<sprite->xSize) 
			if(sprite->data[offs=(y*sprite->xSize+x+1)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.7)][sprite->data[offs]];
	}
	if(y+1>=0&&y+1<sprite->ySize)
	{
		if(x-1>=0&&x-1<sprite->xSize) 
			if(sprite->data[offs=((y+1)*sprite->xSize+x-1)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.9)][sprite->data[offs]];
		if(x>=0&&x<sprite->xSize) 
			if(sprite->data[offs=((y+1)*sprite->xSize+x)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.7)][sprite->data[offs]];
		if(x+1>=0&&x+1<sprite->xSize) 
			if(sprite->data[offs=((y+1)*sprite->xSize+x+1)]!=sprite->data[0])
				sprite->data[offs]=gLightningTab[(int)(kLightValues*0.9)][sprite->data[offs]];
	}
	return id;	
}

int BulletHitSprite16(tSpriteHeader16 *sprite,int x,int y)
{
	int offs;
	if(y-1>=0&&y-1<sprite->ySize)
	{
		if(x-1>=0&&x-1<sprite->xSize) 
			if(sprite->data[offs=((y-1)*sprite->xSize+x-1)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.9),sprite->data[offs]);
		if(x>=0&&x<sprite->xSize) 
			if(sprite->data[offs=((y-1)*sprite->xSize+x)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.7),sprite->data[offs]);
		if(x+1>=0&&x+1<sprite->xSize) 
			if(sprite->data[offs=((y-1)*sprite->xSize+x+1)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.9),sprite->data[offs]);
	}
	if(y>=0&&y<sprite->ySize)
	{
		if(x-1>=0&&x-1<sprite->xSize) 
			if(sprite->data[offs=(y*sprite->xSize+x-1)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.7),sprite->data[offs]);
		if(x>=0&&x<sprite->xSize) 
			if(sprite->data[offs=(y*sprite->xSize+x)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.6),sprite->data[offs]);
		if(x+1>=0&&x+1<sprite->xSize) 
			if(sprite->data[offs=(y*sprite->xSize+x+1)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.7),sprite->data[offs]);
	}
	if(y+1>=0&&y+1<sprite->ySize)
	{
		if(x-1>=0&&x-1<sprite->xSize) 
			if(sprite->data[offs=((y+1)*sprite->xSize+x-1)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.9),sprite->data[offs]);
		if(x>=0&&x<sprite->xSize) 
			if(sprite->data[offs=((y+1)*sprite->xSize+x)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.7),sprite->data[offs]);
		if(x+1>=0&&x+1<sprite->xSize) 
			if(sprite->data[offs=((y+1)*sprite->xSize+x+1)]!=sprite->data[0])
				sprite->data[offs]=ShadeRGB16((int)(kLightValues*0.9),sprite->data[offs]);
	}
	return id;	
}
*/
void BulletHitSprite8(tSpriteHeader *sprite,int x,int y,int size)
{
	int offs;
	float invSize=1/size;
	int xPos,yPos;
	for(yPos=y-size;yPos<=y+size;yPos++)
		if(yPos>=0&&yPos<sprite->ySize)
			for(xPos=x-size;xPos<=x+size;xPos++)
				if(xPos>=0&&xPos<sprite->ySize)
					if(sprite->data[offs=(yPos*sprite->xSize+xPos)]!=sprite->data[0])
						sprite->data[offs]=gLightningTab[(int)(kLightValues*(0.6+0.2*invSize*(fabs(x-xPos)+fabs(y-yPos))))][sprite->data[offs]];
}

void BulletHitSprite16(tSpriteHeader16 *sprite,int x,int y,int size)
{
	int offs;
	float invSize=1/size;
	int xPos,yPos;
	for(yPos=y-size;yPos<=y+size;yPos++)
		if(yPos>=0&&yPos<sprite->ySize)
			for(xPos=x-size;xPos<=x+size;xPos++)
				if(xPos>=0&&xPos<sprite->ySize)
					if(sprite->data[offs=(yPos*sprite->xSize+xPos)]!=sprite->data[0])
						sprite->data[offs]=ShadeRGB16((int)(kLightValues*(0.6+0.2*invSize*(fabs(x-xPos)+fabs(y-yPos)))),sprite->data[offs]);
}

int BulletHitSprite(int id,int x,int y)
{
	tSpriteHeader *sprite;
	int size;
	if(id-128<kNumSprites)
	{
		int newID=GetUniqueSpriteNum();
		if(!newID) return id;
		gSprites[newID]=gSprites[id-128];
		HandToHand(&gSprites[newID]);		
		sprite=(tSpriteHeader*)*(gSprites[newID]);
		id=newID+128;
	}
	else
		sprite=(tSpriteHeader*)*(gSprites[id-128]);
	if(sprite->drawMode&kDrawModeDoubleSize)
	{
		x*=2;
		y*=2;
		size=2;
	}else 
		size=1;
	x+=sprite->xSize/2;
	y-=sprite->ySize/2;
	y=-y;
	if(gPrefs.hiColor)
		BulletHitSprite16(sprite,x,y,size);
	else
		BulletHitSprite8(sprite,x,y,size);
	return id;
}

void SpriteUnused(int id)
{
	if(id-128>=kNumSprites)
		if(gSprites[id-128])
		{
			DisposeHandle(gSprites[id-128]);
			gSprites[id-128]=nil;
			glDeleteTextures(1,&gSpriteRefs[id-128]);
		}	
}

void LoadSpriteGL(int id)
{
	tSpriteHeader16 *sprite=(tSpriteHeader*)*(gSprites[id-128]);
	int i;
	UInt16 mask=sprite->data[0];
	for(i=0;i<sprite->xSize*sprite->ySize;i++)
		if(sprite->data[i]==mask)
			sprite->data[i]&=0x7fff;
		else
			sprite->data[i]|=0x8000;
	glGenTextures(1,&gSpriteRefs[id-128]);
	glBindTexture(GL_TEXTURE_2D,gSpriteRefs[id-128]);
	glTexImage2D(GL_TEXTURE_2D,0,4,sprite->xSize,sprite->ySize,0,GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,&sprite->data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
}

void LoadSprites()
{
	int i;
	int spritePack=gPrefs.hiColor?kPackSp16:kPackSprt;
	LoadPack(spritePack);
	for(i=128;i<128+kNumSprites;i++)
	{
		int size;
		Ptr data=GetUnsortedPackEntry(spritePack,i,&size);
		if(data)
		{
			DoError(PtrToHand(data,&gSprites[i-128],size));
			LoadSpriteGL(i);
		}
		else
			gSprites[i-128]=nil;
	}
	for(i=kNumSprites;i<kNumSprites+kNumSpecialSprites;i++)
		gSprites[i]=nil;
	UnloadPack(spritePack);		
}

void DrawLifeBar8(int cy,int cx,int shift)
{
	tSpriteHeader *sprite=(tSpriteHeader*)*(gSprites[kLifeBarId-128]);
	int y=cy-sprite->ySize/2;
	int x=cx-sprite->xSize/2-shift;
	int vShift=sprite->log2xSize;
	UInt8 mask=sprite->data[0];
	int v=0;
	int endU=sprite->xSize;
	int startU=shift;
	int endV=sprite->ySize;
	UInt8 *dst=gBaseAddr+y*gRowBytes+x;
	UInt8 *trTab=*gTranslucenceTab;
	for(;v<endV;v++)
	{
		int u=startU;
		for(;u<endU;u++)
		{		
			UInt8 color=sprite->data[u+(v<<vShift)];
			if(color!=mask)
				*(dst+u)=trTab[(color<<8)+*(dst+u)];
		}
		dst+=gRowBytes;				
	}
}

void DrawLifeBar16(int cy,int cx,int shift)
{
	tSpriteHeader16 *sprite=(tSpriteHeader*)*(gSprites[kLifeBarId-128]);
	int y=cy-sprite->ySize/2;
	int x=cx-sprite->xSize/2-shift;
	int vShift=sprite->log2xSize;
	UInt16 mask=sprite->data[0];
	int v=0;
	int endU=sprite->xSize;
	int startU=shift;
	int endV=sprite->ySize;
	UInt16 *dst=gBaseAddr+y*gRowBytes+2*x;
	for(;v<endV;v++)
	{
		int u=startU;
		for(;u<endU;u++)
		{		
			UInt16 color=sprite->data[u+(v<<vShift)];
			if(color!=mask)
				*(dst+u)=BlendRGB16(color,*(dst+u));
		}
		dst+=gRowBytes/2;		
	}
}

void DrawLifeBar(int cx,int cy,int shift)
{
	if(gPrefs.hiColor)
		DrawLifeBar16(cy,cx,shift);
	else
		DrawLifeBar8(cy,cx,shift);
}


void UnloadSprites()
{
	int i;
	for(i=0;i<kNumSprites+kNumSpecialSprites;i++)
		if(gSprites[i])
		{
			DisposeHandle(gSprites[i]);
			gSprites[i]=nil;
			glDeleteTextures(1,&gSpriteRefs[i]);
		}
}