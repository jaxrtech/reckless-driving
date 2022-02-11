#define CALL_IN_SPOCKETS_BUT_NOT_IN_CARBON 1

#ifdef __APPLE__
#include <Appearance.h>
#include <Sound.h>
#include <DrawSprocket.h>
#include <InputSprocket.h>
#else

#include <QD/QD.h>
#include <DrawSprocket/DrawSprocket.h>
#include <InputSprocket/InputSprocket.h>
#define __option(NAME) 0

#endif
#include "input.h"
#include "screen.h"
#include "error.h"
#include "sprites.h"
#include "trig.h"
#include "gamesounds.h"
#include "gameinitexit.h"
#include "interface.h"
#include "preferences.h"
#include "random.h"
#include "packs.h"
#include "register.h"

#if __option(profile)
#include <profiler.h>
#endif


float gSinTab[kSinTabSize];
int gInitSuccessful=false;
int gOSX;

void InitToolbox()
{
/*	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);*/
	InitCursor();
}

#undef sin
void InitTrig()
{
	int i;
	for(i=0;i<kSinTabSize;i++)
		gSinTab[i]=sin(2*PI*(float)i/(float)kSinTabSize);
}

UInt32 U32Version(NumVersion v)
{
	return *((UInt32*)(&v));
}

int ReqCheck()
{
	int hit;
	long resp;
	AlertStdAlertParamRec alertParam={
		false,false,nil,
		"\pExit",
		nil,
		nil,
		kAlertStdAlertOKButton,
		0,
		kWindowDefaultPosition};
	DoError(Gestalt(gestaltSystemVersion,&resp));
	gOSX=resp>=0x00001000;
	if(resp<0x00001020)
	{
		StandardAlert(kAlertStopAlert,
		"\pThis version of Reckless Drivin' requires Mac OS X 10.2.",
		"\pGo to http://reckless.x2.nu to download an older version which will run on your machine.",
		&alertParam,
		&hit);
		return false;
	}
	return true;	
}

void InitAE();

/*
static OSStatus LoadFrameworkBundle(CFBundleRef *bundlePtr)
{
	Boolean didLoad = false; //	Flag that indicates the status returned when attempting to load a bundle's executable code.
	CFBundleRef refMainBundle = NULL;
	CFURLRef refMainBundleURL = NULL, refPathBundleURL = NULL;

	CFURLRef bundleURL = NULL;
	CFBundleRef bundle = NULL;

	//	See the Core Foundation URL Services chapter for details.
	// get app bundle (even for a CFM app!)
	refMainBundle = CFBundleGetMainBundle();
	if (!refMainBundle)
	{
        DebugStr ("\pCould open main bundle");
		return paramErr;
	}
	// create a URL to the app bundle
	refMainBundleURL = CFBundleCopyBundleURL (refMainBundle);
	if (!refMainBundleURL)
	{
        DebugStr ("\pCould not copy main bundle URL");
		return paramErr;
	}
	// create a URL that points to the app's directory
	// create a URL to the HID library bundle
	bundleURL = CFURLCreateCopyAppendingPathComponent (NULL, refMainBundleURL, CFSTR("Contents/MacOS/FpuExceptions.bundle"), true);
	// release created URLs
	if (refMainBundleURL != NULL)
		CFRelease (refMainBundleURL);
	if (refPathBundleURL != NULL)
		CFRelease (refPathBundleURL);
	// did we actaully get a bundle URL
	if (!bundleURL)
	{
        DebugStr ("\pCould create HID bundle URL");
		return paramErr;
    }
	// get the actual bundle for the HID library
	bundle = CFBundleCreate (NULL, bundleURL);
	if (!bundle)
	{
        DebugStr ("\pCould not create HID MachO library bundle");
		return paramErr;
	}
    if (!CFBundleLoadExecutable (bundle)) // If the code was successfully loaded, look for our function.
	{
    	DebugStr ("\pCould not load MachO executable");
    	return paramErr;
	}
	*bundlePtr=bundle;
}

extern void PPC_EnableFloatingPointExceptions();
typedef void (*fPtr)();

Call the Omni Group's floating-point exception enable function
used for debugging, this makes it easier to catch operations returning NaN results.
void CallOmniEnableFloatExceptions()
{
	CFBundleRef 		bundle;
	fPtr exceptPtr;
	LoadFrameworkBundle(&bundle);


	exceptPtr = (fPtr) CFBundleGetFunctionPointerForName( bundle, CFSTR("PPC_EnableFloatingPointExceptions") );
	exceptPtr();
}
*/

void Init()
{	//CallOmniEnableFloatExceptions();
	InitToolbox();
	if(!ReqCheck())
		ExitToShell();
	InitAE();
	gAppResFile=CurResFile();
	//DoError(RegisterAppearanceClient());
	Randomize();
	LoadPrefs();
	gPrefs.hiColor=true;
	CheckRegi();
//	if(!gRegistered)
//		Register(false);
	//InitScreen(0);
	ScreenMode(kScreenRunning);
//	InitGL();
	ShowPicScreen(1003);
	LoadPack(kPackSnds);
	LoadPack(kPackObTy);
	LoadPack(kPackOgrp);
	LoadPack(kPackRoad);
	if(gPrefs.hiColor)
	{
		LoadPack(kPacksR16);
		LoadPack(kPackcR16);
		LoadPack(kPackTx16);
	}
	else
	{
		LoadPack(kPacksRLE);
		LoadPack(kPackcRLE);
		LoadPack(kPackTxtR);
	}
	LoadSprites();
	InitTrig();
	InitInput();
	SetGameVolume(-1);
	InitChannels();
	InitInterface();
	#if __option(profile)
	{
		FSSpec spec;
		OSErr err;
		
		err=FSMakeFSSpec(0,0,"\pProfiler Dump",&spec);
		DoError(err==fnfErr?noErr:err);
		err=FSpDelete(&spec);
		DoError(err==fnfErr?noErr:err);
		DoError(ProfilerInit(collectSummary,bestTimeBase,200,5));
	}
	#endif	
	gInitSuccessful=true;
}


void Exit()
{
	#if __option(profile)
	DoError(ProfilerDump("\pProfiler Dump"));
	#endif
	if(gInitSuccessful)
	{
		WritePrefs(false);
		FadeScreen(1);
		SetSystemVolume();
		KillGL();
		ScreenMode(kScreenStopped);
		InputMode(kInputStopped);
	}
	ExitToShell();
}