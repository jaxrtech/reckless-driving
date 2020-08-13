#ifndef __APPLE__
#include <MacTypes.h>
#include <QD/Quickdraw.h>
#endif

#ifdef __APPLE__
#if PRAGMA_STRUCT_ALIGN
     #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
     #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
     #pragma pack(2)
#endif
#endif

typedef struct PortList {
     short       count;
     GrafPtr     ports[1];
     } PortList, **PortListHdl;

#ifdef __APPLE__
#if PRAGMA_STRUCT_ALIGN
     #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
     #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
     #pragma pack()
#endif
#endif

Boolean IsPortListValid() {
     return(true);
}