#define CALL_IN_SPOCKETS_BUT_NOT_IN_CARBON 1

#include "gameframe.h"
#include "interface.h"
#include "input.h"
#include "error.h"
#include "gamesounds.h"
#include "screenfx.h"
#include "screen.h"
#include <DrawSprocket.h>

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

int HandleMouseDownPause(EventRecord *event)
{
	return true;
}

void PauseGame()
{
	EventRecord event;
	int end=false;
	PauseFrameCount();
	SaveFlushEvents();
	InputMode(kInputSuspended);
	BeQuiet();
	//glDisable(GL_BLEND);
	ShowPicScreenNoFade(1006);
	ShowCursor();
	do{
		if(WaitNextEvent(everyEvent,&event,60,nil))
		{
			int eventWasProcessed;
			//DoError(DSpProcessEvent(&event,&eventWasProcessed));
			if(event.what==mouseDown)
				end=HandleMouseDownPause(&event);
			if(event.what==updateEvt)
			{	
				BeginUpdate((WindowPtr)event.message);
				ShowPicScreen(1006);
				EndUpdate((WindowPtr)event.message);
			}
			if(event.what==osEvt)
				if(event.message>>24==suspendResumeMessage)
					if(event.message&resumeFlag)
					{
					//	ScreenMode(kScreenRunning);	
						ShowPicScreenNoFade(1006);	
					}
				//	else
					//	ScreenMode(kScreenStopped);						
		}
	}while(!end);
	InputMode(kInputRunning);
	HideCursor();		
	//glEnable(GL_BLEND);
	ScreenClear();
	StartCarChannels();
	ResumeFrameCount();
}