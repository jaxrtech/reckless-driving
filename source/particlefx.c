#include "vec2d.h"
#include "screen.h"
#include "objects.h"
#include "preferences.h"
#include "random.h"

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#define kMaxParticles	64
#define kMaxPFX			64
#define E 				2.71828182846
#define kParticleDuration	0.5

typedef struct{
	int active;
	int numParticles;
	int layer;
	UInt32 fxStartFrame;
	UInt16 color16;
	UInt8 color;
	t2DPoint pos;
	t2DPoint p[kMaxParticles*4];
} tParticleFX;

tParticleFX gParticleFX[kMaxPFX];


void NewParticleFX(t2DPoint pos,t2DPoint velo,int num,UInt8 color,int layer,float spread)
{
	int numFX;
	for(numFX=0;numFX<kMaxPFX&&gParticleFX[numFX].active;numFX++);
	if(numFX<kMaxPFX)
	{
		int i;
		tParticleFX *fx=&gParticleFX[numFX];
		fx->numParticles=num;
		fx->active=true;
		fx->layer=layer;
		fx->fxStartFrame=gFrameCount;
		fx->pos=pos;
		if(gPrefs.hiColor)
			fx->color16=((UInt16*)*g16BitClut)[color];
		else
			fx->color=color;
		num*=8;
		if(num>kMaxParticles)num=kMaxParticles;
		for(i=0;i<num;i++)
			fx->p[i]=VEC2D_Sum(velo,P2D(RanFl(-spread,spread),RanFl(-spread,spread)));
	}
}

void ScreenVertex(float x,float y);

void DrawParticleFXZoomed(float xDrawStart,float yDrawStart,float zoom,int layer)
{
	int numFX;
	float invZoom=1/zoom;
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POINTS);
	for(numFX=0;numFX<kMaxPFX;numFX++)
		if(gParticleFX[numFX].active&&gParticleFX[numFX].layer==layer)
		{
			tParticleFX *fx=&gParticleFX[numFX];
			float dt=(gFrameCount-fx->fxStartFrame)*kFrameDuration;
			int numDrawParticles=fx->numParticles*invZoom*invZoom*2;
			int i;
			if(numDrawParticles>8*kMaxParticles)
				numDrawParticles=8*kMaxParticles;
			if(dt<kParticleDuration)
					for(i=0;i<numDrawParticles;i++)
					{
						float epdt=-pow(E,-dt);
						t2DPoint pos=VEC2D_Sum(fx->pos,P2D(fx->p[i].x*epdt+fx->p[i].x,fx->p[i].y*epdt+fx->p[i].y));
						float x=(pos.x-xDrawStart)*invZoom,y=(yDrawStart-pos.y)*invZoom;
						glColor4f(((fx->color16>>10)&0x1f)/32.0,((fx->color16>>5)&0x1f)/32.0,((fx->color16>>0)&0x1f)/32.0,1);
						if(x>0&&y>0)
							if(x<gXSize&&y<gYSize)
								ScreenVertex(x,y);
					}				
			else
				fx->active=false;
		}
	glEnd();
	glColor4f(1,1,1,1);
	glEnable(GL_TEXTURE_2D);
}