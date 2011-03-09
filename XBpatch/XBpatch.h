//////////////////////////////////////////////////////////////////////////////
// XBPatch.h
// XBPatch - Xbox BIOS patch library
// Copyright (C) 2003  Mitch Crane
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "BiosOptions.h"
#include "Patchers\CX24981Patcher.h"
#include "Patchers\CX24979Patcher.h"
#include "Patchers\CX24978Patcher.h"
#include "Patchers\CX24977Patcher.h"
#include "Patchers\CX24976Patcher.h"
#include "Patchers\CEvoxD6Patcher.h"
#include "Patchers\CEvoxM7Patcher.h"
#include "Patchers\CUnknownPatcher.h"

#ifdef XBPATCH_EXPORTS
#define XBPATCH_API __declspec(dllexport)
#else
#define XBPATCH_API __declspec(dllimport)
#endif

XBPATCH_API unsigned long
XBPatchGetVersion();

XBPATCH_API int
getIDString(const char *fname, char *biosID);

int
RC4StrToArray(CString RC4Str, unsigned char *RC4);

XBPATCH_API void
initBIOSOptions(BIOS_Options *BIOSOptions);

XBPATCH_API int
resizeBIOS(const char *biosFPName, const char *saveFPName, int theRomSize);

XBPATCH_API int
patchKernelBootFlags(unsigned char *kernelBuffer, unsigned char *flag, int rwFlag);

