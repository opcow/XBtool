//////////////////////////////////////////////////////////////////////////////
// CKernelPatcher.h
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
#include "..\BiosOptions.h"

#ifdef XBPATCH_EXPORTS
#define XBPATCH_API __declspec(dllexport)
#else
#define XBPATCH_API __declspec(dllimport)
#endif

enum defaultColors
{
	DefLogoColor = 0x0013CA62,
	DefXLipColor = 0x00000100,
	DefXLipHighlightsColor = 0x004B9B4B,
	DefXLogoInnerColor = 0x00166a20,
	DefXLogoGlowColor = 0x0000DECA
};

// Kernel patching class
class CKernelPatcher {
public:
	virtual XBPATCH_API int Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions) = 0;
	virtual XBPATCH_API int Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions) = 0;
	virtual XBPATCH_API void GetSupportedOptions(BIOS_Supported_Options &SupOpts);
	virtual XBPATCH_API int KernelID(unsigned char * kernelBuffer);
	XBPATCH_API int ReadKernelVersion(KernelVersion * Version);
	XBPATCH_API int ReadBootFlags(unsigned char & BootFlags);
	XBPATCH_API int WriteBootFlags(unsigned char BootFlags);

	virtual void InitPatches() = 0;
	virtual int WritePreIDPatches(BIOS_Options *BIOSOptions) = 0;
	int ExpandKernel();
	int ShrinkKernel();
	int GetKernelSize();

	int ReadColors(COLORREF &XLipHighlightsColor, COLORREF &XLipColor, COLORREF &XWallColor, COLORREF &XGlowColor, COLORREF &XboxColor);
	int ReadAnimation(int & Animation);
	int ReadXLogo(int & XLogo);
	int ReadSplashLogo(int & SplashLogo);
	int ReadTradeMark(int & TrademarkSymbol);
	int ReadDash(unsigned char * BootPart, CString * DashName, int * DashNameLen, CString & DefaultXbeName);
	int ReadDvd(int & DVDCheck);
	int ReadLed(unsigned char & LEDColor, int & FanSpeed);
	int ReadLBA48(int & LBA48, int & PartitionOpt, int & IgnoreHDTable);
	int ReadEeprom(int & EmbedEeprom);
	int ReadHDKey(int & EmbedHDKey, unsigned char * key);

	int WriteColors(COLORREF XLipHighlightsColor, COLORREF XLipColor, COLORREF XWallColor, COLORREF XGlowColor, COLORREF XboxColor);
	int WriteAnimation(int Animation);
	int WriteXLogo(int XLogo);
	int WriteSplashLogo(int SplashLogo);
	int WriteTradeMark(int TrademarkSymbol);
	int WriteDash(unsigned char * BootPart, CString * DashName, int * DashNameLen, CString & DefaultXbeName);
	int WriteDvd(int DVDCheck);
	int WriteLed(unsigned char LEDColor, int FanSpeed);
	int WriteLBA48(int LBA48, int PartitionOpt, int IgnoreHDTable);
	int WriteEeprom(int & EmbedEeprom);
	int WriteHDKey(int & EmbedHDKey, unsigned char * key);

	PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader);
	LPVOID GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase );
	unsigned char * GetExportOffset(int exportNum);


	char				*mpKernelName;
	unsigned char		*mID_Digest;
	int					mNativeSizeOfKernel;
	long				mAddressAdjust;
	CString				mPartitonStr;
	int					*mXGlowOffset;
	int					mAniGlowOffset;
	int					*mXWallOffset;
	int					mXLipHighlightsOffset;
	int					mXLipOffset;
	int					mXBOXTextOffset;
	AnimPatch			*mAnimPatch;
	XLogoPatch			*mXLogoPatch;
	SplashLogoPatch		*mSplashLogoPatch;
	TMarkPatch			*mTMarkPatch;
	ResetOnEjectPatch	*mResetOnEjectPatch;
	DashPatch			*mDashPatch;
	int					*mDefXBEOffset;
	LedPatch			*mLedPatch;
	DvdPatch			*mDvdPatch;
	LBA48Patch			*mLBA48Patch;
	DiscardInitPatch	*mDiscardInitPatch;
	EepromPatch			*mEepromPatch;
	EmbedHDKeyPatch		*mEmbedHDKeyPatch;

	BIOS_Supported_Options  mSupOpts;

	unsigned char		*mpKernelBuffer;
};