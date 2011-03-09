//////////////////////////////////////////////////////////////////////////////
// CX24978Patcher.cpp
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
#include "CX24978Patcher.h"

void CX24978Patcher::InitPatches()
{
	static int XGlowOffset[] =			{ 0x0005CF8B, 0x0005CF95, 0x0005CF9E };
	static int AniGlowOffset =			0x0005AE7B;
	static int XWallOffset[] =			{ 0x0005CFA8, 0x0005CFB1, 0x0005CFBB };
	static int XLipHighlightsOffset =	0x0005C5F9;
	static int XLipOffset =				0x0005C5FE;
	static int XBOXTextOffset =			0x0005D14B;
	static int DefXBEOffset =			0x2abb;

	static AnimPatch xAnimPatch =
	{
		{ 0x000579E8, 0x0005814F, 0x00058849, 0x00058871 },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0xE9, 0x0B, 0x00, 0x00, 0x00 }, { 0x75, 0x0E, 0xE8, 0xA3, 0x9C } },
		{ { 0xE9, 0x02, 0x00, 0x00, 0x00 }, { 0x75, 0x05, 0xE8, 0xB5, 0x9C } }
	};

	static XLogoPatch xXLogoPatch =
	{
		{ 0x00057D32, 0x00058833 },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0xA7, 0xF7, 0xFF, 0xFF } },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0x5A, 0xF3, 0xFF, 0xFF } },
	};

	static SplashLogoPatch xSplashLogoPatch =
	{
		0x00056FE2,
		{ { 0x90, 0x90 }, { 0x75, 0x0B } }
	};

	static TMarkPatch xTMarkPatch =
	{
		0x0005D049,
		{ 0x20, 0x18 }
	};

	static DashPatch xDashPatch =
	{
		"4978LoadDashboard",
		0x21bcf,
		{ 0x2ac8, 0x2b20, 0x2af4  },
		{ 0x53, 0x55, 0x56, 0x57, 0x68, 0x04, 0x02, 0x00, 0x00, 0xBF, 0xFE, 0xFF, 0x0F, 0x00, 0x57, 0x68, 0x00, 0x00, 0xF0, 0xFF, 0xE8, 0x84 },
		3,
		{ 14, 10, 14},
		0x24c5
	};

	static LedPatch xLedPatch =
	{
		"4978LEDFan",
		0x73c3,
		{0xE8, 0xB0, 0xC6, 0x00, 0x00}
	};

	static DiscardInitPatch xDiscardInitPatch =
	{
		"4978DiscardInit",
		0x73B2,
		0x8004d920,
		{0xE8, 0x3F, 0x93, 0x00, 0x00}
	};

	static LBA48Patch xLBA48Patch =
	{
		"4978lba48",
		{0x160FF, 0x16289, 0x164AE, 0x1660F, 0x56ADD, 0x56B5F},
		{0x24f, 0x24a, 0x259, 0x245, 0x240, 0x254},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x55, 0x8B, 0xEC, 0x56, 0x8B, 0x75, 0x0C, 0x8B, 0x46, 0x5C, 0x8B, 0x48, 0x10, 0x81, 0xE9, 0x28, 0xD0, 0x04, 0x00, 0x57, 0x74, 0x3F},
		{0x74, 0x5D, 0x48, 0x74, 0x48, 0x83, 0xFB, 0x06, 0x72, 0x2B, 0x75, 0x1F, 0xA1, 0x18, 0xD7, 0x04, 0x80, 0xB9, 0xB0, 0x8A, 0xEE, 0x00},
		{0x8B, 0x85, 0x50, 0xFE, 0xFF},
		{0xE8, 0x5D, 0xFE, 0xFF, 0xFF},
		{ 0x108 },
	};

	static unsigned char xID_Digest[] = { 0x68, 0x91, 0x6a, 0x1e, 0x4e, 0xab, 0x12, 0x1f, 0xcf, 0xa2, 0x74, 0xb7, 0x71, 0x6d, 0x8c, 0x6e };

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
	mNativeSizeOfKernel		= 0xA9B00;
	mpKernelName			= "X2 4978.03";

	mSupOpts.Animation				= 1;
	mSupOpts.BootPart				= 0;
	mSupOpts.DashName				= 0;
	mSupOpts.ExtendedDashNames		= 0;
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
};

void CX24978Patcher::GetSupportedOptions(BIOS_Supported_Options &SupOpts)
{
	SupOpts = mSupOpts;
}

int CX24978Patcher::WritePreIDPatches(BIOS_Options *BIOSOptions)
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