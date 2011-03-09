//////////////////////////////////////////////////////////////////////////////
// CX24982Patcher.h
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
#include "Patchers\CKernelPatcher.h"

class CXboxKernel {

public:
	XBPATCH_API CXboxKernel(const char *kernelFile);
	XBPATCH_API CXboxKernel();
	XBPATCH_API ~CXboxKernel();
	XBPATCH_API int Read();
	XBPATCH_API int Write();
	XBPATCH_API int WriteBootFlags();
	XBPATCH_API bool IsSupported();


	BIOS_Options			mOpts;
	BIOS_Supported_Options	mSupportedOpts;

	CUnknownPatcher			UnknownPatcher;

	void GetPatcher();
	void InitOpts();

	unsigned char *			mpKernelBuffer;
	CKernelPatcher *		mpKernelPatcher;
	CX24981Patcher			X24981Patcher;
	CX24979Patcher			X24979Patcher;
	CX24977Patcher			X24977Patcher;
	CX24976Patcher			X24976Patcher;
	CEvoxD6Patcher			EvoxD6Patcher;
	CX24978Patcher			X24978Patcher;
	CEvoxM7Patcher			EvoxM7Patcher;
	CString					mSKernelFile;
	unsigned int			mKernelSize;

};