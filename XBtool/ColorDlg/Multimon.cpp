#include "stdafx.h"


// this file includes the multimon stub function
// that are implmented in multimon.h

// this is only necessary if the app is supporting win95
#if !defined(COMPILE_MULTIMON_STUBS) && (WINVER < 0x0500)
//
// this needs to be here so that we get the wrapper stubs that deal with win95 platforms
#define COMPILE_MULTIMON_STUBS
#include "multimon.h"
//
#endif // !defined(COMPILE_MULTIMON_STUBS) && (WINVER < 0x0500)

