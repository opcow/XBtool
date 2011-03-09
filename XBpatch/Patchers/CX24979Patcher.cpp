//////////////////////////////////////////////////////////////////////////////
// CX24979Patcher.cpp
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
#include "CX24979Patcher.h"

void CX24979Patcher::InitPatches()
{
	static int XGlowOffset[] =			{ 0x00060149, 0x00060153, 0x0006015C };
	static int AniGlowOffset =			0x0005E039;
	static int XWallOffset[] =			{ 0x00060166, 0x0006016f, 0x00060179 };
	static int XLipHighlightsOffset =	0x0005f7b7;
	static int XLipOffset =				0x0005f7bc;
	static int XBOXTextOffset =			0x00060309;
	static int DefXBEOffset =			0x2c93;

	static AnimPatch xAnimPatch =
	{
		{ 0x0005aba6, 0x0005b30d, 0x0005ba07, 0x0005ba2f },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0xE9, 0x0B, 0x00, 0x00, 0x00 }, { 0x75, 0x0E, 0xE8, 0xA3, 0x9C } },
		{ { 0xE9, 0x02, 0x00, 0x00, 0x00 }, { 0x75, 0x05, 0xE8, 0xB5, 0x9C } }
	};

	static XLogoPatch xXLogoPatch =
	{
		{ 0x0005aef0, 0x0005b9f1 },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0xA7, 0xF7, 0xFF, 0xFF } },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0x5A, 0xF3, 0xFF, 0xFF } },
	};

	static SplashLogoPatch xSplashLogoPatch =
	{
		0x0005a1a0,
		{ { 0x90, 0x90 }, { 0x75, 0x0B } }
	};

	static TMarkPatch xTMarkPatch =
	{
		0x00060207,
		{ 0x20, 0x18 }
	};

	static DashPatch xDashPatch =
	{
		"4979LoadDashboard",
		0x2300A,
		{ 0x2ca0,  0x2ccc, 0x2cf8 },
		{ 0x53, 0x33, 0xF6, 0x46, 0x56, 0x68, 0xA0, 0x2C, 0x01, 0x80, 0xE8, 0xC1, 0xFB, 0xFF, 0xFF, 0x3B, 0xC3, 0x0F, 0x8D, 0x8F, 0x00, 0x00 },
		3,
		{ 14, 14, 14},
		0x2984
	};

	static LedPatch xLedPatch =
	{
		"4979LEDFan",
		0x8684,
		{0xE8, 0x4b, 0xC7, 0x00, 0x00}
	};

	static DiscardInitPatch xDiscardInitPatch =
	{
		"4979DiscardInit",
		0x8673,
		0x80050b20,
		{0xE8, 0x4e, 0x93, 0x00, 0x00}
	};

	static LBA48Patch xLBA48Patch =
	{
		"4979lba48",
		{0x15EEF, 0x16079, 0x1629E, 0x163FF, 0x562DD, 0x5635F},
		{0x24f, 0x24a, 0x259, 0x245, 0x240, 0x254},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x8A, 0x46, 0x0F, 0x24, 0x0F, 0x0C, 0xE0, 0x66, 0xBA, 0xF6, 0x01, 0xEE, 0x8A, 0x46, 0x0C, 0x66, 0x83, 0xC2, 0xFD, 0xEE, 0x8A, 0x46},
		{0x55, 0x8B, 0xEC, 0x56, 0x8B, 0x75, 0x0C, 0x8B, 0x46, 0x5C, 0x8B, 0x48, 0x10, 0x81, 0xE9, 0x28, 0xD0, 0x04, 0x00, 0x57, 0x74, 0x3F},
		{0x74, 0x5D, 0x48, 0x74, 0x48, 0x83, 0xFB, 0x06, 0x72, 0x2B, 0x75, 0x1F, 0xA1, 0x58, 0xCF, 0x04, 0x80, 0xB9, 0xB0, 0x8A, 0xEE, 0x00},
		{0x8B, 0x85, 0x50, 0xFE, 0xFF},
		{0xE8, 0x5D, 0xFE, 0xFF, 0xFF},
		{ 0x108 },
	};

	static unsigned char xID_Digest[] = { 0xc1, 0x95, 0xcd, 0x26, 0xd7, 0x4e, 0xa3, 0x28, 0x01, 0xc0, 0x9d, 0xca, 0xaf, 0xdd, 0x15, 0x50 };

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
	mNativeSizeOfKernel		= 0xAccc0;
	mpKernelName			= "X2 4979";

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
	mSupOpts.LBA48					= 0;
	mSupOpts.IgnoreHDTable			= 0;
	mSupOpts.PartitionOpt			= 1;
	mSupOpts.FanSpeed				= 1;
	mSupOpts.ResetOnEject			= 0;
	mSupOpts.EmbedEeprom			= 0;
	mSupOpts.EmbedHDKey				= 0;
};

void CX24979Patcher::GetSupportedOptions(BIOS_Supported_Options &SupOpts)
{
	SupOpts = mSupOpts;
}

int CX24979Patcher::Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
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
	BIOSOptions->ResetOnEject = 1;
	ReadLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed);
//	ReadLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);
	BIOSOptions->LBA48 = 1;
	BIOSOptions->PartitionOpt = (int) *(mpKernelBuffer + 0x2d629);
	BIOSOptions->IgnoreHDTable = 1;

	ReadBootFlags(BIOSOptions->BootFlags);
	ReadKernelVersion(&BIOSOptions->Version);

	return 1;
}

int CX24979Patcher::Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
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
//	if (!WriteLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable))
//		return errWriteLBA48;
	 *(mpKernelBuffer + 0x2d629) = (unsigned char) BIOSOptions->PartitionOpt;

	if (!WriteBootFlags(BIOSOptions->BootFlags))
		return errWriteBootFlags;

	return 0;
}

int CX24979Patcher::WritePreIDPatches(BIOS_Options *BIOSOptions)
{
	ShrinkKernel();
	WriteColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor);
	WriteAnimation(BIOSOptions->Animation);
	WriteXLogo(BIOSOptions->XLogo);
	WriteSplashLogo(BIOSOptions->SplashLogo);
	WriteTradeMark(BIOSOptions->TrademarkSymbol);
	WriteBootFlags(BIOSOptions->BootFlags);
	//WriteLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);

	memcpy(mpKernelBuffer + mLedPatch->offset, mLedPatch->data, sizeof(mLedPatch->data));
	memcpy(mpKernelBuffer + mDashPatch->pHookOffset, mDashPatch->data, 22);

	strncpy((char *) (mpKernelBuffer + *mDefXBEOffset), "default.xbe", 13);

	memset(mpKernelBuffer + 0xf8, 0, 4);
	memset(mpKernelBuffer + 0x0148, 0, 2);
	memset(mpKernelBuffer + 0x02e4, 0, 4);
	memset(mpKernelBuffer + 0x0964, 0, 4);
	memset(mpKernelBuffer + 0x0bb6, 0, 1);
	memset(mpKernelBuffer + 0x5800, 0, 6);
	memset(mpKernelBuffer + 0x02d629, 0, 1);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[0], "\\Device\\Harddisk0\\Partition2\\evoXdash.xbe", 44);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[1], "\\Device\\Harddisk0\\Partition2\\avadash.xbe", 44);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[2], "\\Device\\Harddisk0\\Partition2\\mxmdash.xbe", 44);

//	FILE *fh = fopen("c:\\foo2.img", "wb");
//	fwrite(mpKernelBuffer, sizeof(char), mNativeSizeOfKernel, fh);
//	fclose(fh);
	return 1;
}