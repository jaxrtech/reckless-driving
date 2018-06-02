#ifndef __INTERFACE
#define __INTERFACE

#ifdef _WIN32
#include <MacTypes.h>
#endif // _WIN32


extern int gExit;
extern short gLevelResFile,gAppResFile;
extern Str63 gLevelFileName;

void SaveFlushEvents();
void Eventloop();
void InitInterface();
void DisposeInterface();
void ScreenUpdate(WindowPtr win);
void ShowPicScreen(int id);
void ShowPicScreenNoFade(int id);
void WaitForPress();

#endif