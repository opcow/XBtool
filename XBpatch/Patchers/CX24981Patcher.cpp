//////////////////////////////////////////////////////////////////////////////
// CX24981Patcher.cpp
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
#include "CX24981Patcher.h"

void CX24981Patcher::InitPatches()
{
	static int XGlowOffset[] =			{ 0x0002D1B4 };
	static int AniGlowOffset =			0x0005E6B5;
	static int XWallOffset[] =			{ 0x0002D1B0 };
	static int XLipHighlightsOffset =	0x0002D1AC;
	static int XLipOffset =				0x0002D1A8;
	static int XBOXTextOffset =			0x0002D1B8;
	static int DefXBEOffset =			0x2de5f;

	static AnimPatch xAnimPatch =
	{
		{ 0x0005B222, 0x0005b989, 0x0005c083, 0x0005c0ab },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0x90, 0x90 }, { 0x74, 0x09 } },
		{ { 0xE9, 0x0B, 0x00, 0x00, 0x00 }, { 0x75, 0x0E, 0xE8, 0x97, 0x9D } },
		{ { 0xE9, 0x02, 0x00, 0x00, 0x00 }, { 0x75, 0x05, 0xE8, 0xA9, 0x9D } }
	};

	static XLogoPatch xXLogoPatch =
	{
		{ 0x0005b56c, 0x0005c06d },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0xA7, 0xF7, 0xFF, 0xFF } },
		{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0x5A, 0xF3, 0xFF, 0xFF } },
	};

	static SplashLogoPatch xSplashLogoPatch =
	{
		0x0005a81c,
		{ { 0x90, 0x90 }, { 0x75, 0x0B } }
	};

	static TMarkPatch xTMarkPatch =
	{
		0x00060968,
		{ 0x20, 0x18 }
	};

	static ResetOnEjectPatch xResetOnEjectPatch =
	{
		0x23326,
		{	{ 0x83, 0x0D, 0xF8, 0xF5, 0x04, 0x80, 0x08, 0x6A, 0x01 },
			{ 0xA3, 0xF8, 0xF5, 0x04, 0x80, 0x90, 0x90, 0x6A, 0x00 } }
	};

	static DashPatch xDashPatch =
	{
		"4981LoadDashboard",
		0x2339F,
		{ 0x2de6c,  0x2de98, 0x2dec4 },
		{ 0x55, 0x8D, 0x6C, 0x24, 0x88, 0x81, 0xEC, 0xA0, 0x00, 0x00, 0x00, 0x80, 0x65, 0xD8, 0x00, 0x80, 0x65, 0x3C, 0x00, 0x53, 0x56, 0x57 },
		3,
		{ 14, 14, 14},
		0x2984
	};

	static LedPatch xLedPatch =
	{
		"4981LEDFan",
		0x8923,
		{0xE8, 0x3c, 0xC7, 0x00, 0x00}
	};

	static DiscardInitPatch xDiscardInitPatch =
	{
		"4981DiscardInit",
		0x8912,
		0x80051120,
		{0xE8, 0x3f, 0x93, 0x00, 0x00}
	};

	static LBA48Patch xLBA48Patch =
	{
		"4981lba48",
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

	static EepromPatch xEepromPatch =
	{
		"4981EepromShadow",
		0x6033,
		0x4F,
		{ 0x55, 0xB9 }
	};

	static EmbedHDKeyPatch xEmbedHDKeyPatch =
	{
		0x5A34D,
		{
			{
				0x50, 0xFF, 0x35, 0xA4, 0xF7, 0x04, 0x80, 0x0F, 0xB7, 0x05, 0xC0, 0xF7, 0x04, 0x80, 0x50, 0xFF, 
				0x35, 0xC4, 0xF7, 0x04, 0x80, 0x6A, 0x10, 0x68, 0xC8, 0xF4, 0x04, 0x80, 0xE8, 0x99, 0x98, 0xFC, 
				0xFF, 0x8D, 0x45, 0xD8, 0x50
			},
			{
				0x68, 0x40, 0x01, 0x0C, 0x80, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
				0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
				0x90, 0x90, 0x90, 0x90, 0x90
			}
		}
	};

	static unsigned char xID_Digest[] = { 0xdd, 0x1e, 0xa2, 0x56, 0x91, 0x23, 0xd1, 0xa2, 0x8a, 0x20, 0x52, 0x73, 0x46, 0x3d, 0x86, 0x00 };

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
	mResetOnEjectPatch		= &xResetOnEjectPatch;
	mDashPatch				= &xDashPatch;
	mDefXBEOffset			= &DefXBEOffset;
	mLedPatch				= &xLedPatch;
	mLBA48Patch				= &xLBA48Patch;
	mDiscardInitPatch		= &xDiscardInitPatch;
	mEepromPatch			= &xEepromPatch;
	mEmbedHDKeyPatch		= &xEmbedHDKeyPatch;

	mNativeSizeOfKernel		= 0xAd440;
	mpKernelName			= "X2 4981";

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
	mSupOpts.ResetOnEject			= 1;
	mSupOpts.EmbedEeprom			= 1;
	mSupOpts.EmbedHDKey				= 1;
};

void CX24981Patcher::GetSupportedOptions(BIOS_Supported_Options &SupOpts)
{
	SupOpts = mSupOpts;
}

int CX24981Patcher::Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	mpKernelBuffer = kernelBuffer;

	BIOSOptions->Supported = true;
	strncpy(BIOSOptions->Name, mpKernelName, 63);

	NoFloatReadColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor);
	ReadAnimation(BIOSOptions->Animation);
	ReadXLogo(BIOSOptions->XLogo);
	ReadSplashLogo(BIOSOptions->SplashLogo);
	ReadTradeMark(BIOSOptions->TrademarkSymbol);
	ReadDash(BIOSOptions->BootPart, BIOSOptions->DashName, BIOSOptions->DashNameLen, BIOSOptions->DefaultXbeName);
	BIOSOptions->DVDCheck = 0;
	BIOSOptions->ResetOnEject = memcmp(mpKernelBuffer + mResetOnEjectPatch->offset, mResetOnEjectPatch->data[0], 9);
	ReadLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed);
//	ReadLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);
	BIOSOptions->LBA48 = 1;
	BIOSOptions->PartitionOpt = (int) *(mpKernelBuffer + 0x2db09);
	BIOSOptions->IgnoreHDTable = 1;
	ReadEeprom(BIOSOptions->EmbedEeprom);
	ReadHDKey(BIOSOptions->EmbedHDKey, BIOSOptions->HDKey);

	ReadBootFlags(BIOSOptions->BootFlags);
	ReadKernelVersion(&BIOSOptions->Version);

	return 1;
}

int CX24981Patcher::Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	mpKernelBuffer = kernelBuffer;
/*
	short *textVerts = (short *) (kernelBuffer + 0x78060);

	for (int i = 0; i < 468; i++)
		textVerts[i] /= 2;
*/
	if (!ExpandKernel())
		return errWriteExpKernel;
	if (!NoFloatWriteColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor))
		return errWriteColor;
	if (!WriteAnimation(BIOSOptions->Animation))
		return errWriteAni;
	if (!WriteXLogo(BIOSOptions->XLogo))
		return errWriteXLogo;
	if (!WriteSplashLogo(BIOSOptions->SplashLogo))
		return errWriteSpLogo;
	if (!WriteTradeMark(BIOSOptions->TrademarkSymbol))
		return errWriteTM;
	if (!WriteEeprom(BIOSOptions->EmbedEeprom))
		return errWriteEeprom;
	if (!WriteHDKey(BIOSOptions->EmbedHDKey, BIOSOptions->HDKey))
		return errWriteHDKey;

	memcpy(mpKernelBuffer + mResetOnEjectPatch->offset, mResetOnEjectPatch->data[BIOSOptions->ResetOnEject], 9);

	if (mSupOpts.DashName)
	{
		if (!WriteDash(BIOSOptions->BootPart, BIOSOptions->DashName, BIOSOptions->DashNameLen, BIOSOptions->DefaultXbeName))
			return errWriteDash;
	}
	if (!WriteLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed))
		return errWriteLED;
	 *(mpKernelBuffer + 0x2db09) = (unsigned char) BIOSOptions->PartitionOpt;

	if (!WriteBootFlags(BIOSOptions->BootFlags))
		return errWriteBootFlags;

	return 0;
}

int CX24981Patcher::WritePreIDPatches(BIOS_Options *BIOSOptions)
{
	ShrinkKernel();
	NoFloatWriteColors(BIOSOptions->XLipHighlightsColor, BIOSOptions->XLipColor, BIOSOptions->XWallColor, BIOSOptions->XGlowColor, BIOSOptions->XboxColor);
	WriteAnimation(BIOSOptions->Animation);
	WriteXLogo(BIOSOptions->XLogo);
	WriteSplashLogo(BIOSOptions->SplashLogo);
	WriteTradeMark(BIOSOptions->TrademarkSymbol);
	WriteBootFlags(BIOSOptions->BootFlags);
	WriteHDKey(BIOSOptions->EmbedHDKey, 0);

	memcpy(mpKernelBuffer + mLedPatch->offset, mLedPatch->data, sizeof(mLedPatch->data));
	memcpy(mpKernelBuffer + mDashPatch->pHookOffset, mDashPatch->data, 22);

	strncpy((char *) (mpKernelBuffer + *mDefXBEOffset), "default.xbe", 13);

	memset(mpKernelBuffer + 0xf8, 0, 4);
	memset(mpKernelBuffer + 0x0148, 0, 2);
	memset(mpKernelBuffer + 0x02e4, 0, 4);
	memset(mpKernelBuffer + 0x0964, 0, 4);
	memset(mpKernelBuffer + 0x0bb4, 0, 1);
	memset(mpKernelBuffer + 0x0bb6, 0, 1);
	memset(mpKernelBuffer + 0x5784, 0, 1);
	memset(mpKernelBuffer + 0x02db09, 0, 1);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[0], "\\Device\\Harddisk0\\Partition2\\avalaunch.xbe", 44);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[1], "\\Device\\Harddisk0\\Partition2\\evoxdash.xbe", 44);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[2], "\\Device\\Harddisk0\\Partition2\\mxmdash.xbe", 44);

	memcpy(mpKernelBuffer + mResetOnEjectPatch->offset, mResetOnEjectPatch->data[0], 9);

	memset(mpKernelBuffer + mEepromPatch->functOffset, 0, mEepromPatch->functSize); //eeprom read function


	// flubber mod patches
	unsigned char FlubberPatchA[60] =
	{
		0xD9, 0x59, 0x48, 0xC7, 0x41, 0x44, 0x00, 0x00, 0x80, 0x3E, 0xD9, 0xE8, 0xD9, 0x59, 0x50, 0xC7, 
		0x41, 0x4C, 0x9A, 0x99, 0x19, 0x3E, 0xD9, 0xEE, 0x5E, 0xD9, 0x59, 0x54, 0xD9, 0xEE, 0xD9, 0x59, 
		0x58, 0xD9, 0xEE, 0xD9, 0x59, 0x5C, 0xD9, 0xE8, 0xD9, 0x59, 0x60, 0xD9, 0xE8, 0xD9, 0x59, 0x64, 
		0xD9, 0xE8, 0xD9, 0x59, 0x68, 0xC7, 0x41, 0x6C, 0x33, 0x33, 0x13, 0x40, 
	} ;
	memcpy(mpKernelBuffer + 0x5d9a4, FlubberPatchA, sizeof(FlubberPatchA));

	*(mpKernelBuffer + 0x5e427) = 0xf0;
	*(mpKernelBuffer + 0x5e430) = 0xf4;

	unsigned char FlubberPatchB[60] =
	{
		0x2C, 0xD9, 0x05, 0xB8, 0x2D, 0x01, 0x80, 0xD9, 0x56, 0x30, 0xD9, 0xE8, 0xD9, 0x5E, 0x34, 0xD9, 
		0xC1, 0xD9, 0x5E, 0x38, 0xD9, 0xE8, 0xD9, 0x5E, 0x3C, 0xD9, 0x56, 0x40, 0xD9, 0xE8, 0xD9, 0x5E, 
		0x44, 0xD9, 0xC1, 0xD9, 0x5E, 0x48, 0xD9, 0xE8, 0xD9, 0x5E, 0x4C, 0xD9, 0x5E, 0x50, 0xDD, 0xD8, 
		0xD9, 0xE8, 0xD9, 0x5E, 0x54, 0x5E, 0x5B, 0x5D, 0xC2, 0x24, 0x00, 0xC7, 
	} ;
	memcpy(mpKernelBuffer + 0x63f28, FlubberPatchB, sizeof(FlubberPatchB));
	// end flubber mod patches


//	FILE *fh = fopen("c:\\foo3.img", "wb");
//	fwrite(mpKernelBuffer, sizeof(char), mNativeSizeOfKernel, fh);
//	fclose(fh);
	return 1;
}

int CX24981Patcher::NoFloatReadColors(COLORREF &XLipHighlightsColor, COLORREF &XLipColor, COLORREF &XWallColor, COLORREF &XGlowColor, COLORREF &XboxColor)
{
	int i;
	BYTE redByte, greenByte, blueByte;

	// X wall color
	i = mXWallOffset[0];
	blueByte = mpKernelBuffer[i++];
	greenByte = mpKernelBuffer[i++];
	redByte = mpKernelBuffer[i];
	XWallColor = RGB(redByte, greenByte, blueByte);

	// X glow color
	i = mXGlowOffset[0];
	blueByte = mpKernelBuffer[i++];
	greenByte = mpKernelBuffer[i++];
	redByte = mpKernelBuffer[i];
	XGlowColor = RGB(redByte, greenByte, blueByte);

	// X lip highlight color
	i = mXLipHighlightsOffset;
	blueByte = mpKernelBuffer[i++];
	greenByte = mpKernelBuffer[i++];
	redByte = mpKernelBuffer[i];
	XLipHighlightsColor = RGB(redByte, greenByte, blueByte);

	// X lip color
	i = mXLipOffset;
	blueByte = mpKernelBuffer[i++];
	greenByte = mpKernelBuffer[i++];
	redByte = mpKernelBuffer[i];
	XLipColor = RGB(redByte, greenByte, blueByte);

	// "XBOX" color
	i = mXBOXTextOffset;
	blueByte = mpKernelBuffer[i++];
	greenByte = mpKernelBuffer[i++];
	redByte = mpKernelBuffer[i];
	XboxColor = RGB(redByte, greenByte, blueByte);

	return 1;
}

int CX24981Patcher::NoFloatWriteColors(COLORREF XLipHighlightsColor, COLORREF XLipColor, COLORREF XWallColor, COLORREF XGlowColor, COLORREF XboxColor)
{
	int i;

	// X wall color
	i = mXWallOffset[0];
	mpKernelBuffer[i++] = GetBValue(XWallColor);
	mpKernelBuffer[i++] = GetGValue(XWallColor);
	mpKernelBuffer[i] = GetRValue(XWallColor);	

	// X glow color
	i = mXGlowOffset[0];
	mpKernelBuffer[i++] = GetBValue(XGlowColor);
	mpKernelBuffer[i++] = GetGValue(XGlowColor);
	mpKernelBuffer[i] = GetRValue(XGlowColor);	

	// ani initial glow color
	i = mAniGlowOffset;
	mpKernelBuffer[i++] = GetBValue(XGlowColor);
	mpKernelBuffer[i++] = GetGValue(XGlowColor);
	mpKernelBuffer[i] = GetRValue(XGlowColor);	

	// X lip corner highlights
	i = mXLipHighlightsOffset;
	mpKernelBuffer[i++] = GetBValue(XLipHighlightsColor);
	mpKernelBuffer[i++] = GetGValue(XLipHighlightsColor);
	mpKernelBuffer[i] = GetRValue(XLipHighlightsColor);	

	// X lip color
	i = mXLipOffset;
	mpKernelBuffer[i++] = GetBValue(XLipColor);
	mpKernelBuffer[i++] = GetGValue(XLipColor);
	mpKernelBuffer[i] = GetRValue(XLipColor);

	// "XBOX" color
	i = mXBOXTextOffset;
	mpKernelBuffer[i++] = GetBValue(XboxColor);
	mpKernelBuffer[i++] = GetGValue(XboxColor);
	mpKernelBuffer[i] = GetRValue(XboxColor);

	return 1;
}