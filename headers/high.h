#ifndef __HIGH
#define __HIGH

#ifdef _WIN32
#include <MacTypes.h>
#endif

void CheckHighScore(UInt32 score);
void ClearHighScores();
void ShowHighScores(int hilite);
	
#endif