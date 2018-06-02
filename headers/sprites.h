#ifndef __SPRITES
#define __SPRITES

void DrawSpriteAlpha(int id, float cx, float cy, float dir, float zoom,float alpha);
void DrawSprite(int,float,float,float,float);
void DrawSpriteTranslucent(int,float,float,float,float);
void LoadSprites();
void UnloadSprites();
void SpriteUnused(int);
int YDistortSprite(int,int,int,int,int,int,float);
int XDistortSprite(int,int,int,int,int,int,float);
int BulletHitSprite(int,int,int);
void DrawLifeBar(int,int,int);

#endif