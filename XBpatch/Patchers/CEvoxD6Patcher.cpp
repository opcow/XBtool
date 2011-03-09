//////////////////////////////////////////////////////////////////////////////
// CEvoxD6Patcher.cpp
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
#include "CEvoxD6Patcher.h"

void CEvoxD6Patcher::InitPatches()
{
	static int XGlowOffset[] =				{ 0x0004C104, 0x0004C10E, 0x0004C117 };
	static int AniGlowOffset =				0x00049FF4;
	static int XWallOffset[] =				{ 0x0004C121, 0x0004C12A, 0x0004C134 };
	static int XLipHighlightsOffset =		0x0004B772;
	static int XLipOffset =					0x0004B777;
	static int XBOXTextOffset =				0x0004C2C4;
	static int DefXBEOffset =				0x27db;

	static AnimPatch xAnimPatch =
	{
		{ 0x00046B61, 0x000472C8, 0x000479C2, 0x000479EA },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0xE9, 0x0B, 0x00, 0x00, 0x00 }, { 0x75, 0x0E, 0xE8, 0xA4, 0x9C } },
		{ { 0xE9, 0x02, 0x00, 0x00, 0x00 }, { 0x75, 0x05, 0xE8, 0xB6, 0x9C } }
	};

	static XLogoPatch xXLogoPatch =
	{
		{ 0x00046EAB, 0x000479AC },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0xA7, 0xF7, 0xFF, 0xFF } },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0x5A, 0xF3, 0xFF, 0xFF } },
	};

	static SplashLogoPatch xSplashLogoPatch =
	{
		0x00045476,
		{ { 0x90, 0x90 }, { 0x75, 0x0B } }
	};

	static TMarkPatch xTMarkPatch =
	{
		0x0004C1C2,
		{ 0x20, 0x18 }
	};

	static DashPatch xDashPatch =
	{
		"d6LoadDashboard",
		0x1f2d1,
		{ 0x1ec9b, 0x27e8, 0 },
		{ 0x6A, 0x00, 0x6A, 0x01, 0x68, 0x9B, 0xEC, 0x02, 0x80, 0xE9, 0xA0, 0xF9, 0xFF, 0xFF, 0x85, 0xC0, 0x7D, 0x48, 0x80, 0x3D, 0xDC, 0xA8 },
		3,
		{ 12, 18, 0},
		0x2984
	};

	static DvdPatch xDvdPatch = {
		{ 0x00045D6E, 0x00045DC2, 0x00045DE0, 0x00045DFF },
		{ { 0xE9, 0x31, 0x00, 0x00, 0x00 }, { 0x75, 0x34, 0xBE, 0x58, 0x02 } },
		{ { 0xE9, 0x09, 0x00, 0x00, 0x00 }, { 0x7D, 0x0C, 0x8A, 0x4D, 0xFF } },
		{ { 0x90, 0x90 }, { 0x7C, 0xE2 } },
		{ { 0xE9, 0x09, 0x00, 0x00, 0x00 }, { 0x7D, 0x0C, 0xE8, 0x62, 0xE5 } }
	};

	static LedPatch xLedPatch =
	{
		"D6LEDFan",
		0x58e1,
		{0xE8, 0x22, 0xC3, 0x00, 0x00}
	};

	static DiscardInitPatch xDiscardInitPatch =
	{
		"D6DiscardInit",
		0x58D0,
		0x8003cb20,
		{0xE8, 0x2D, 0x91, 0x00, 0x00}
	};

	static LBA48Patch xLBA48Patch =
	{
		"d6lba48",
		{0x141CB, 0x14355, 0x1457A, 0x146D5, 0x45BE2, 0x45C64},
		{0x24f, 0x24a, 0x259, 0x245, 0x240, 0x254},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x55, 0x8B, 0xEC, 0x56, 0x8B, 0x75, 0x0C, 0x8B, 0x46, 0x5C, 0x8B, 0x48, 0x10, 0x81, 0xE9, 0x28, 0xD0, 0x04, 0x00, 0x57, 0x74, 0x3F},
		{0x74, 0x3B, 0x48, 0x74, 0x26, 0x83, 0xFB, 0x06, 0x72, 0x0A, 0xE9, 0x66, 0xA5, 0x00, 0x00, 0xE9, 0xC2, 0x00, 0x00, 0x00, 0x8D, 0x43},
		{0x8B, 0x85, 0x50, 0xFE, 0xFF},
		{0xE8, 0x5D, 0xFE, 0xFF, 0xFF},
		{ 0x108 }
	};

	static unsigned char xID_Digest[] = { 0x2c, 0xdb, 0xbd, 0x29, 0xfa, 0x9e, 0x53, 0x44, 0xc1, 0x00, 0x75, 0x5e, 0xc1, 0x41, 0x96, 0x79 };

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
	mDvdPatch				= &xDvdPatch;
	mLBA48Patch				= &xLBA48Patch;
	mDiscardInitPatch		= &xDiscardInitPatch;
	mNativeSizeOfKernel		= 0x9B520;
	mpKernelName			= "EvolutionX D6";

	mSupOpts.Animation				= 1;
	mSupOpts.BootPart				= 1;
	mSupOpts.DashName				= 1;
	mSupOpts.ExtendedDashNames		= 1;
	mSupOpts.DefaultXbeName			= 1;
	mSupOpts.DVDCheck				= 1;
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
};

void CEvoxD6Patcher::GetSupportedOptions(BIOS_Supported_Options &SupOpts)
{
	SupOpts = mSupOpts;
}

int CEvoxD6Patcher::Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
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
	ReadDvd(BIOSOptions->DVDCheck);
	BIOSOptions->ResetOnEject = *(mpKernelBuffer + 0x1ee9e) == 0x80;
	ReadLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed);
	ReadLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);

	ReadBootFlags(BIOSOptions->BootFlags);
	ReadKernelVersion(&BIOSOptions->Version);

	return 1;
}

int CEvoxD6Patcher::Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
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
	if (!WriteDash(BIOSOptions->BootPart, BIOSOptions->DashName, BIOSOptions->DashNameLen, BIOSOptions->DefaultXbeName))
		return errWriteDash;
	if (!WriteDvd(BIOSOptions->DVDCheck))
		return errWriteDVD;
	if (!WriteLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed))
		return errWriteLED;
	if (!WriteLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable))
		return errWriteLBA48;

	if (!WriteBootFlags(BIOSOptions->BootFlags))
		return errWriteBootFlags;

	return 0;
}
int CEvoxD6Patcher::WritePreIDPatches(BIOS_Options *BIOSOptions)
{
	ShrinkKernel();
	WriteColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor);
	WriteAnimation(BIOSOptions->Animation);
	WriteXLogo(BIOSOptions->XLogo);
	WriteSplashLogo(BIOSOptions->SplashLogo);
	WriteTradeMark(BIOSOptions->TrademarkSymbol);
	WriteBootFlags(BIOSOptions->BootFlags);
	WriteDvd(BIOSOptions->DVDCheck);
	WriteLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);

	memcpy(mpKernelBuffer + mLedPatch->offset, mLedPatch->data, sizeof(mLedPatch->data));
	memcpy(mpKernelBuffer + mDashPatch->pHookOffset, mDashPatch->data, 22);

	strncpy((char *) (mpKernelBuffer + *mDefXBEOffset), "default.xbe", 0x11);

	memset(mpKernelBuffer + 0x0001EE9E, 0, 0x03f6); //ejectfix diffs

	return 1;
}
