#ifndef __ERROR
#define __ERROR

#ifndef __APPLE__
#include <MacTypes.h>
#endif

void HandleError(int id);

inline void DoError(OSErr id)
{
	if(id) HandleError(id);
}

#endif