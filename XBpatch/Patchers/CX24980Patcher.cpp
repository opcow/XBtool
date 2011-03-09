#include "stdafx.h"
#include <atlstr.h>
#include "CX24981Patcher.h"

void CX24981Patcher::InitPatches()
{
	static int XGlowOffset[] =			{ 0x0002D1B4 };
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

	static DashPatch xDashPatch =
	{
		"4981LoadDashboard",
		0x2339F,
		{ 0x2de6c,  0x2de98, 0x2dec4 },
		{ 0x55, 0x8D, 0x6C, 0x24, 0x88, 0x81, 0xEC, 0xA0, 0x00, 0x00, 0x00, 0x80, 0x65, 0xD8, 0x00, 0x80, 0x65, 0x3C, 0x00, 0x53, 0x56, 0x57 },
		3,
		{ 14, 14, 14},
		0x24cb
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

	static unsigned char xID_Digest[] = { 0x97, 0x77, 0xa2, 0x7f, 0x4f, 0x24, 0x36, 0x1f, 0x58, 0x12, 0x94, 0x72, 0x63, 0xee, 0x5b, 0x19 };
 

	mID_Digest = xID_Digest;

	mAddressAdjust = 0x80010000;
	mPartitonStr = "\\Device\\Harddisk0\\Partition2\\";

	mXGlowOffset			= XGlowOffset;
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
	BIOSOptions->DVDCheck = 1;
	ReadLed(BIOSOptions->LEDColor, BIOSOptions->FanSpeed);
//	ReadLBA48(BIOSOptions->LBA48, BIOSOptions->PartitionOpt, BIOSOptions->IgnoreHDTable);
	BIOSOptions->LBA48 = 1;
	BIOSOptions->PartitionOpt = (int) *(mpKernelBuffer + 0x2db09);
	BIOSOptions->IgnoreHDTable = 1;

	ReadBootFlags(BIOSOptions->BootFlags);

	return 1;
}

int CX24981Patcher::Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	mpKernelBuffer = kernelBuffer;

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

	memcpy(mpKernelBuffer + mLedPatch->offset, mLedPatch->data, sizeof(mLedPatch->data));
	memcpy(mpKernelBuffer + mDashPatch->pHookOffset, mDashPatch->data, 22);

	strncpy((char *) (mpKernelBuffer + *mDefXBEOffset), "default.xbe", 13);

	memset(mpKernelBuffer + 0xf8, 0, 4);
	memset(mpKernelBuffer + 0x0148, 0, 2);
	memset(mpKernelBuffer + 0x02e4, 0, 4);
	memset(mpKernelBuffer + 0x0964, 0, 4);
	memset(mpKernelBuffer + 0x0bb6, 0, 1);
	memset(mpKernelBuffer + 0x5784, 0, 1);
	memset(mpKernelBuffer + 0x02db09, 0, 1);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[0], "\\Device\\Harddisk0\\Partition2\\evoXdash.xbe", 44);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[1], "\\Device\\Harddisk0\\Partition2\\avadash.xbe", 44);
	memcpy(mpKernelBuffer + mDashPatch->pOffset[2], "\\Device\\Harddisk0\\Partition2\\mxmdash.xbe", 44);

//	FILE *fh = fopen("c:\\foo3.img", "wb");
//	fwrite(mpKernelBuffer, sizeof(char), mNativeSizeOfKernel, fh);
//	fclose(fh);
	return 1;
}

int CX24981Patcher::NoFloatReadColors(COLORREF &XLipHighlightsColor, COLORREF &XLipColor, COLORREF &XWallColor, COLORREF &XGlowColor, COLORREF &XboxColor)
{
	int i;
	BYTE redByte, greenByte, blueByte;
	union {
		unsigned long	lv;
		float			fv;
	} u_PatchData;

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
	union {
		unsigned long	lv;
		float			fv;
	} u_PatchData;

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