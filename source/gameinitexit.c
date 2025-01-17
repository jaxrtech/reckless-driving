#include "input.h"
#include "screen.h"
#include "gameframe.h"
#include "roads.h"
#include "objects.h"
#include "error.h"
#include "trig.h"
#include "gamesounds.h"
#include "renderframe.h"
#include "interface.h"
#include "screenfx.h"
#include "textfx.h"
#include "sprites.h"
#include "packs.h"
#include "high.h"
#include "register.h"
#include "preferences.h"
#include "gamesounds.h"
#include "AGL_DSp.h"


tRoad gRoadData;	
UInt32 *gRoadLenght;
tRoadInfo *gRoadInfo;
tLevelData *gLevelData;
tTrackInfo *gTrackUp,*gTrackDown;
tMarkSeg *gMarks;
int gMarkSize;
int gLevelID;
tObject *gFirstObj,*gCameraObj,*gPlayerObj,*gSpikeObj,*gBrakeObj,*gFirstVisObj,*gLastVisObj;
tTrackSeg gTracks[kMaxTracks];
int gTrackCount;
int gPlayerLives,gExtraLives;
int gNumMissiles,gNumMines;
float gPlayerDeathDelay,gFinishDelay;
int gPlayerScore,gDisplayScore;
int gPlayerBonus;
UInt32 gPlayerAddOns;
float gGameTime;
float gXDriftPos,gYDriftPos,gXFrontDriftPos,gYFrontDriftPos,gZoomVelo;
int gGameOn;
int gPlayerCarID;
float gPlayerSlide[4]={0,0,0,0};
float gSpikeFrame;
int gLCheat;

int abs(int x);
void CopClear();

Ptr LoadObjs(Ptr dataPos)
{
	int i;
	tObjectPos *objs=(tObjectPos*)(dataPos+sizeof(UInt32));
	for(i=0;i<*(UInt32*)dataPos;i++)
	{
		tObject *theObj=NewObject(gFirstObj,objs[i].typeRes);
		theObj->dir=objs[i].dir;
		theObj->pos.x=objs[i].x;
		theObj->pos.y=objs[i].y;		
	}
	return (Ptr)(objs+*(UInt32*)dataPos);
}

int NumLevels()
{	
	int i=kPackLevel1;
	SetResLoad(false);
	for(i=140;Get1Resource('Pack',i);i++);
	SetResLoad(true);
	if(i==140)i=150;
	return i-140;
}

void GameEndSequence();

int LoadLevel()
{
	int i,sound;
	if(gLevelID>=kEncryptedPack-kPackLevel1||gLevelResFile)
		if(!gRegistered)
		{
			ShowPicScreen(1005);
			WaitForPress();
			BeQuiet();
			InitInterface();
			ShowCursor();
			if(!gLCheat)
				CheckHighScore(gPlayerScore);
			return false;
		}

	gFirstObj = (tObject *) NewPtrClear(sizeof(tObject));
	gFirstObj->next=gFirstObj;
	gFirstObj->prev=gFirstObj;
	
	if(gLevelID>=NumLevels())
	{
		GameEndSequence();
		gLevelID=0;
	}

	LoadPack(kPackLevel1+gLevelID);
	gLevelData = (tLevelData *) GetSortedPackEntry(kPackLevel1 + gLevelID, 1, nil);
	gMarks = (tMarkSeg *) GetSortedPackEntry(kPackLevel1 + gLevelID, 2, &gMarkSize);
	gMarkSize/=sizeof(tMarkSeg);
	gRoadInfo= (tRoadInfo *) GetSortedPackEntry(kPackRoad, gLevelData->roadInfo, nil);
	gTrackUp= (tTrackInfo *) ((Ptr) gLevelData + sizeof(tLevelData));
	gTrackDown= (tTrackInfo *) (gTrackUp + sizeof(UInt32) + gTrackUp->num * sizeof(tTrackInfoSeg));
	gRoadLenght= (UInt32 *) LoadObjs((Ptr) (gTrackDown + sizeof(UInt32) + gTrackDown->num * sizeof(tTrackInfoSeg)));
	gRoadData= (tRoad) ((Ptr) gRoadLenght + sizeof(UInt32));
	
	for(i=0;i<9;i++)
		if((*gLevelData).objGrps[i].resID)
			InsertObjectGroup((*gLevelData).objGrps[i]);

	gPlayerObj=NewObject(gFirstObj,gRoadInfo->water?kNormalPlayerBoatID:gPlayerCarID);
	gPlayerObj->pos.x=gLevelData->xStartPos;
	gPlayerObj->pos.y=500;
	gPlayerObj->control=kObjectDriveUp;
	gPlayerObj->target=1;
	gCameraObj=gPlayerObj;
	gPlayerBonus=1;
//	gPlayerObj=nil; //	Uncomment this line to make the player car ai controlled
	gSpikeObj=nil;
	gBrakeObj=nil;
	CopClear();
	SortObjects();
	
	gGameTime=0;
	gTrackCount=0;
	gPlayerDeathDelay=0;
	gFinishDelay=0;
	gPlayerBonus=1;
	gDisplayScore=gPlayerScore;
	gXDriftPos=0;
	gYDriftPos=0;
	gXFrontDriftPos=0;
	gYFrontDriftPos=0;
	gZoomVelo=kMaxZoomVelo;
	ClearTextFX();
	StartCarChannels();
	gScreenBlitSpecial=true;
	return true;
}

void DisposeLevel()
{
	UnloadPack(kPackLevel1+gLevelID);
	gPlayerObj=nil;
	while((tObject*)gFirstObj->next!=gFirstObj)
	{
		SpriteUnused((*(tObject*)gFirstObj->next).frame);
		RemoveObject((tObject*)gFirstObj->next);
	}
	DisposePtr((Ptr)gFirstObj);
}

extern int gOSX;

void GetLevelNumber()
{
	DialogPtr cheatDlg;
	short type;
	Rect box;
	Handle item;
	short hit;
	long num;
	Str255 text;
	if(gOSX)
	{
		FadeScreen(1);
		ScreenMode(kScreenSuspended);
		FadeScreen(0);
	}
	cheatDlg=GetNewDialog(129,nil,(WindowPtr)-1);
	DoError(SetDialogDefaultItem(cheatDlg,1));
	do ModalDialog(nil,&hit); while(hit!=1);
	GetDialogItem(cheatDlg,2,&type,&item,&box);
	GetDialogItemText(item,text);
	StringToNum(text,&num);
	gLevelID=num-1;
	if(gLevelID>=NumLevels())gLevelID=0;
	GetDialogItem(cheatDlg,5,&type,&item,&box);
	GetDialogItemText(item,text);
	StringToNum(text,&num);
	gPlayerCarID=num;
	DisposeDialog(cheatDlg);
	if(gOSX)
	{
		FadeScreen(1);
		ScreenMode(kScreenRunning);
		FadeScreen(512);
	}
}

void StartGame(int lcheat)
{
	DisposeInterface();
	gPlayerLives=3;
	gExtraLives=0;
	gPlayerAddOns=0;
	gPlayerDeathDelay=0;
	gFinishDelay=0;
	gPlayerScore=0;
	gLevelID=0;
	gPlayerCarID=kNormalPlayerCarID;
	gNumMissiles=0;
	gNumMines=0;
	gGameOn=true;
	gEndGame=false;
	if(lcheat)
		GetLevelNumber();
	gLCheat=lcheat;
	FadeScreen(1);
	HideCursor();
	ScreenMode(kScreenRunning);
	InputMode(kInputRunning);
	if(LoadLevel()){
		ScreenClear();
		FadeScreen(512);	
		RenderFrame();
		InitFrameCount();
	}
}

void EndGame()
{	
	gPlayerLives=0;//so RenderFrame will not draw Panel.
	RenderFrame();
	DisposeLevel();
	BeQuiet();
	SimplePlaySound(152);
	GameOverAnim();	
	InitInterface();
	ShowCursor();
	if(!gLCheat)
		CheckHighScore(gPlayerScore);
}
