//////////////////////////////////////////////////////////////////////////////
// CX24976Patcher.cpp
// XBpatch - Xbox BIOS recognition and patch library
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
#include "stdafx.h"
#include <atlstr.h>
#include "CX24976Patcher.h"

void CX24976Patcher::InitPatches()
{
	static int XGlowOffset[] =			{ 0x0005C98B, 0x0005C995, 0x0005C99E };
	static int AniGlowOffset =			0x0005A87B;
	static int XWallOffset[] =			{ 0x0005C9A8, 0x0005C9B1, 0x0005C9BB };
	static int XLipHighlightsOffset =	0x0005BFF9;
	static int XLipOffset =				0x0005BFFE;
	static int XBOXTextOffset =			0x0005CB4B;
	static int DefXBEOffset =			0x2a37;

	static AnimPatch xAnimPatch =
	{
		{ 0x000573E8, 0x00057B4F, 0x00058249, 0x00058271 },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0xE9, 0x0B, 0x00, 0x00, 0x00 }, { 0x75, 0x0E, 0xE8, 0xA3, 0x9C } },
		{ { 0xE9, 0x02, 0x00, 0x00, 0x00 }, { 0x75, 0x05, 0xE8, 0xB5, 0x9C } }
	};

	static XLogoPatch xXLogoPatch =
	{
		{ 0x00057732, 0x00058233 },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0xA7, 0xF7, 0xFF, 0xFF } },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0x5A, 0xF3, 0xFF, 0xFF } },
	};

	static SplashLogoPatch xSplashLogoPatch =
	{
		0x000569E2,
		{ { 0x90, 0x90 }, { 0x75, 0x0B } }
	};

	static TMarkPatch xTMarkPatch =
	{
		0x0005CA49,
		{ 0x20, 0x18 }
	};

	static DashPatch xDashPatch =
	{
		"4976LoadDashboard",
		0x2152d,
		{ 0x2a44, 0x2a9c, 0x2A70 },
		{ 0x53, 0x68, 0xF8, 0x2B, 0x01, 0x80, 0xE8, 0xAC, 0x24, 0xFF, 0xFF, 0x59, 0x33, 0xDB, 0x53, 0x6A, 0x01, 0x68, 0x44, 0x2A, 0x01, 0x80 },
		3,
		{ 14, 10, 14},
		0x2984
	};

	static LedPatch xLedPatch =
	{
		"4976LEDFan",
		0x71c3,
		{0xE8, 0x9C, 0xC6, 0x00, 0x00}
	};

	static DiscardInitPatch xDiscardInitPatch =
	{
		"4976DiscardInit",
		0x71B2,
		0x8004d320,
		{0xE8, 0x2A, 0x93, 0x00, 0x00}
	};

	static LBA48Patch xLBA48Patch =
	{
		"4976lba48",
		{0x15EEF, 0x16079, 0x1629E, 0x163FF, 0x564DD, 0x5655F},
		{0x24f, 0x24a, 0x259, 0x245, 0x240, 0x254},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x55, 0x8B, 0xEC, 0x56, 0x8B, 0x75, 0x0C, 0x8B, 0x46, 0x5C, 0x8B, 0x48, 0x10, 0x81, 0xE9, 0x28, 0xD0, 0x04, 0x00, 0x57, 0x74, 0x3F},
		{0x74, 0x5D, 0x48, 0x74, 0x48, 0x83, 0xFB, 0x06, 0x72, 0x2B, 0x75, 0x1F, 0xA1, 0xB8, 0xCF, 0x04, 0x80, 0xB9, 0xB0, 0x8A, 0xEE, 0x00},
		{0x8B, 0x85, 0x50, 0xFE, 0xFF},
		{0xE8, 0x5D, 0xFE, 0xFF, 0xFF},
		{ 0x108 },
	};

	static unsigned char xID_Digest[] = { 0x26, 0x80, 0x57, 0xb3, 0xcb, 0x60, 0xfb, 0x16, 0x51, 0x86, 0x5e, 0x01, 0xc9, 0x9b, 0x30, 0x14 };

	mID_Digest = xID_Digest;

	mAddressAdjust = 0x80010000;
	mPartitonStr = "\\Device\\Harddisk0\\Partition2\\";

	mXGlowOffset			= XGlowOffset;
	mAniGlowOffset			= AniGlowOffset;
	mXWallOffset			= XWallOffset;
	mXLipHighlightsOffset	= XLipHighlightsOffset;
	mXLipOffset				= XLipOffset;
	mXBOXTextOffset			= XBOXTextOffset;

	mAnimPatch				= &xAnimPatch;
	mXLogoPatch				= &xXLogoPatch;
	mSplashLogoPatch		= &xSplashLogoPatch;
	mTMarkPatch				= &xTMarkPatch;
	mDashPatch				= &xDashPatch;
	mDefXBEOffset			= &DefXBEOffset;
	mLedPatch				= &xLedPatch;
	mLBA48Patch				= &xLBA48Patch;
	mDiscardInitPatch		= &xDiscardInitPatch;
	mNativeSizeOfKernel		= 0xA9500;

	mpKernelName			= "X2 4976.02";

	mSupOpts.Animation				= 1;
	mSupOpts.BootPart				= 1;
	mSupOpts.DashName				= 1;
	mSupOpts.ExtendedDashNames		= 1;
	mSupOpts.DefaultXbeName			= 1;
	mSupOpts.DVDCheck				= 0;
	mSupOpts.LEDColor				= 1;
	mSupOpts.SplashLogo				= 1;
	mSupOpts.TrademarkSymbol		= 1;
	mSupOpts.XboxColor				= 1;
	mSupOpts.XGlowColor				= 1;
	mSupOpts.XLipColor				= 1;
	mSupOpts.XLipHighlightsColor	= 1;
	mSupOpts.XLogo					= 1;
	mSupOpts.XWallColor				= 1;
	mSupOpts.BootFlags				= 1;
	mSupOpts.LBA48					= 1;
	mSupOpts.IgnoreHDTable			= 1;
	mSupOpts.PartitionOpt			= 1;
	mSupOpts.FanSpeed				= 1;
	mSupOpts.ResetOnEject			= 0;
	mSupOpts.EmbedEeprom			= 0;
	mSupOpts.EmbedHDKey				= 0;
}

void CX24976Patcher::GetSupportedOptions(BIOS_Supported_Options &SupOpts)
{
	SupOpts = mSupOpts;
}

int CX24976Patcher::Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	mpKernelBuffer = kernelBuffer;

	BIOSOptions->Supported = true;
	strncpy(BIOSOptions->Name, mpKernelName, 63);

	ReadColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor);
	ReadAnimation(BIOSOptions->Animation);
	ReadXLogo(BIOSOptions->XLogo);
	ReadSplashLogo(BIOSOptions->SplashLogo);
	ReadTradeMark(BIOSOptions->TrademarkSymbol);
	ReadDash(BIOSOptions->BootPart, BIOSOptions->DashName, BIOSOptions->DashNameLen, BIOSOptions->DefaultXbeName);
	BIOSOptions->DVDCheck = 0;
	ReadLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed);
	ReadLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);

	ReadBootFlags(BIOSOptions->BootFlags);
	ReadKernelVersion(&BIOSOptions->Version);

	return 1;
}

int CX24976Patcher::Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	mpKernelBuffer = kernelBuffer;

	if (!ExpandKernel())
		return errWriteExpKernel;
	if (!WriteColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor))
		return errWriteColor;
	if (!WriteAnimation(BIOSOptions->Animation))
		return errWriteAni;
	if (!WriteXLogo(BIOSOptions->XLogo))
		return errWriteXLogo;
	if (!WriteSplashLogo(BIOSOptions->SplashLogo))
		return errWriteSpLogo;
	if (!WriteTradeMark(BIOSOptions->TrademarkSymbol))
		return errWriteTM;
	if (mSupOpts.DashName)
	{
		if (!WriteDash(BIOSOptions->BootPart, BIOSOptions->DashName, BIOSOptions->DashNameLen, BIOSOptions->DefaultXbeName))
			return errWriteDash;
	}
	if (!WriteLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed))
		return errWriteLED;
	if (!WriteLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable))
		return errWriteLBA48;

	if (!WriteBootFlags(BIOSOptions->BootFlags))
		return errWriteBootFlags;

	return 0;
}

int CX24976Patcher::WritePreIDPatches(BIOS_Options *BIOSOptions)
{
	ShrinkKernel();
	WriteColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor);
	WriteAnimation(BIOSOptions->Animation);
	WriteXLogo(BIOSOptions->XLogo);
	WriteSplashLogo(BIOSOptions->SplashLogo);
	WriteTradeMark(BIOSOptions->TrademarkSymbol);
	WriteBootFlags(BIOSOptions->BootFlags);
	WriteLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);

	memcpy(mpKernelBuffer + mLedPatch->offset, mLedPatch->data, sizeof(mLedPatch->data));
	memcpy(mpKernelBuffer + mDashPatch->pHookOffset, mDashPatch->data, 22);

	strncpy((char *) (mpKernelBuffer + *mDefXBEOffset), "default.xbe", 0x11);

	return 1;
}