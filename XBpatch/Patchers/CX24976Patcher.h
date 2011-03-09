//////////////////////////////////////////////////////////////////////////////
// CX24976Patcher.h
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
#include "CKernelPatcher.h"

class CX24976Patcher
	: public CKernelPatcher {

public:
	virtual XBPATCH_API int Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions);
	virtual XBPATCH_API int Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions);
	virtual XBPATCH_API void GetSupportedOptions(BIOS_Supported_Options &SupOpts);
	XBPATCH_API CX24976Patcher() {};

	virtual void InitPatches();
	virtual int WritePreIDPatches(BIOS_Options *BIOSOptions);

};
