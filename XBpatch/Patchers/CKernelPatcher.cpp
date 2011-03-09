//////////////////////////////////////////////////////////////////////////////
// CKernelPatcher.cpp
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
#include "CKernelPatcher.h"
#include "..\MD5\MD5.h"

void CKernelPatcher::GetSupportedOptions(BIOS_Supported_Options &SupOpts)
{
	mSupOpts.Animation				= 0;
	mSupOpts.BootPart				= 0;
	mSupOpts.DashName				= 0;
	mSupOpts.ExtendedDashNames		= 0;
	mSupOpts.DefaultXbeName			= 0;
	mSupOpts.DVDCheck				= 0;
	mSupOpts.LEDColor				= 0;
	mSupOpts.SplashLogo				= 0;
	mSupOpts.TrademarkSymbol		= 0;
	mSupOpts.XboxColor				= 0;
	mSupOpts.XGlowColor				= 0;
	mSupOpts.XLipColor				= 0;
	mSupOpts.XLipHighlightsColor	= 0;
	mSupOpts.XLogo					= 0;
	mSupOpts.XWallColor				= 0;
	mSupOpts.BootFlags				= 0;
	mSupOpts.LBA48					= 0;
	mSupOpts.IgnoreHDTable			= 0;
	mSupOpts.PartitionOpt			= 0;
	mSupOpts.FanSpeed				= 0;
	mSupOpts.ResetOnEject			= 0;
	mSupOpts.EmbedEeprom			= 0;
	mSupOpts.EmbedHDKey				= 0;

	SupOpts = mSupOpts;
    }

int CKernelPatcher::ReadColors(COLORREF &XLipHighlightsColor, COLORREF &XLipColor, COLORREF &XWallColor, COLORREF &XGlowColor, COLORREF &XboxColor)
{
	int i;
	BYTE redByte, greenByte, blueByte;
	union {
		unsigned long	lv;
		float			fv;
	} u_PatchData;

	// X wall color
	i = mXWallOffset[0];
	u_PatchData.lv = (unsigned char) mpKernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i]) << 24;

	redByte = (BYTE) (255 * u_PatchData.fv);

	i = mXWallOffset[1];
	u_PatchData.lv = (unsigned char) mpKernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i]) << 24;

	greenByte = (BYTE) (255 * u_PatchData.fv);

	i = mXWallOffset[2];
	u_PatchData.lv = (unsigned char) mpKernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 24;

	blueByte = (BYTE) (255 * u_PatchData.fv);

	XWallColor = RGB(redByte, greenByte, blueByte);

	// X glow color
	i = mXGlowOffset[0];
	u_PatchData.lv = (unsigned char) mpKernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i]) << 24;

	redByte = (BYTE) (255 * u_PatchData.fv);

	i = mXGlowOffset[1];
	u_PatchData.lv = (unsigned char) mpKernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i]) << 24;

	greenByte = (BYTE) (255 * u_PatchData.fv);

	i = mXGlowOffset[2];
	u_PatchData.lv = (unsigned char) mpKernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) mpKernelBuffer[i]) << 24;

	blueByte = (BYTE) (255 * u_PatchData.fv);

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

int CKernelPatcher::ReadAnimation(int & Animation)
{
	if(mAnimPatch->data1[0][0] != mpKernelBuffer[*mAnimPatch->offset])
		Animation = 1;
	else
		Animation = 0;

	return 1;
}
int CKernelPatcher::ReadXLogo(int & XLogo)
{
	if(mXLogoPatch->data1[0][0] != mpKernelBuffer[*mXLogoPatch->offset])
		XLogo = 1;
	else
		XLogo = 0;

	return 1;
}
int CKernelPatcher::ReadSplashLogo(int & SplashLogo)
{
	if(mSplashLogoPatch->data[0][0] != mpKernelBuffer[mSplashLogoPatch->offset])
		SplashLogo = 1;
	else
		SplashLogo = 0;

	return 1;
}
int CKernelPatcher::ReadTradeMark(int & TrademarkSymbol)
{
	if(mTMarkPatch->data[0] != mpKernelBuffer[mTMarkPatch->offset])
		TrademarkSymbol = 1;
	else
		TrademarkSymbol = 0;

	return 1;
}

int CKernelPatcher::ReadDash(unsigned char * BootPart, CString * DashName, int * DashNameLen, CString & DefaultXbeName)
{
	int i, offset, kernelSize;

	kernelSize = GetKernelSize();

	if (kernelSize >= mNativeSizeOfKernel + 0x2C00 && mSupOpts.ExtendedDashNames
		&& *((unsigned long *) (mpKernelBuffer + mNativeSizeOfKernel + 0x2BFC)) ==  0x02120001)
	{
		for (i = 0, offset = mNativeSizeOfKernel + mDashPatch->pExtOffset; i < 3; i++, offset += 158)
		{
			BootPart[i] = *(mpKernelBuffer + 27 + offset);
			DashName[i] = (char *) (mpKernelBuffer + 29 + offset);
			DashNameLen[i] = 128;
		}
	}
	else
	{
		for (i = 0; i < mDashPatch->numOpts; i++)
		{
			if (mDashPatch->pOffset[i])
			{
				BootPart[i] = *(mpKernelBuffer + 27 + mDashPatch->pOffset[i]);
				DashName[i] = (char *) (mpKernelBuffer + 29 + mDashPatch->pOffset[i]);
			}
			if (mSupOpts.ExtendedDashNames)
				DashNameLen[i] = 128;
			else
				DashNameLen[i] = mDashPatch->nameLen[i];
		}
	}
	// default xbe for DVD
	DefaultXbeName = (char *) (mpKernelBuffer + *mDefXBEOffset);

	return 1;
}

int CKernelPatcher::ReadDvd(int & DVDCheck)
{
	if(mDvdPatch->data1[0][0] != mpKernelBuffer[*mDvdPatch->offset])
		DVDCheck = 1;
	else
		DVDCheck = 0;

	return 1;
}

int CKernelPatcher::ReadLed(unsigned char & LEDColor, int & FanSpeed)
{
	if (GetKernelSize() < mNativeSizeOfKernel + 0x8000)
	{
		LEDColor = 0x0F;
		FanSpeed = 20;
	}
	else
	{
		LEDColor = mpKernelBuffer[mNativeSizeOfKernel + 0x2280];
		FanSpeed = 2 * mpKernelBuffer[mNativeSizeOfKernel + 0x2281];
	}
	return 1;
}

int CKernelPatcher::WriteColors(COLORREF XLipHighlightsColor, COLORREF XLipColor, COLORREF XWallColor, COLORREF XGlowColor, COLORREF XboxColor)
{
	union {
		unsigned long	lv;
		float			fv;
	} u_PatchData;

	int i;

	if (XWallColor != DefXLogoInnerColor)
	{
		// X wall color
		u_PatchData.fv = (float) GetRValue(XWallColor) / 255;

		i = mXWallOffset[0];
		mpKernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
		mpKernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

		u_PatchData.fv = (float) GetGValue(XWallColor) / 255;

		i = mXWallOffset[1];
		mpKernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
		mpKernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

		u_PatchData.fv = (float) GetBValue(XWallColor) / 255;

		i = mXWallOffset[2];
		mpKernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
		mpKernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;
	}

	if (XGlowColor != DefXLogoGlowColor)
	{
		// X glow color
		u_PatchData.fv = (float) GetRValue(XGlowColor) / 255;

		i = mXGlowOffset[0];
		mpKernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
		mpKernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

		u_PatchData.fv = (float) GetGValue(XGlowColor) / 255;

		i = mXGlowOffset[1];
		mpKernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
		mpKernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

		u_PatchData.fv = (float) GetBValue(XGlowColor) / 255;

		i = mXGlowOffset[2];
		mpKernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
		mpKernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
		mpKernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;
	}

	// animation initial glow (make it same as X glow)
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

int CKernelPatcher::WriteAnimation(int Animation)
{
	int opt = Animation;
	memcpy(mpKernelBuffer + mAnimPatch->offset[0], mAnimPatch->data1[opt], sizeof(mAnimPatch->data1[0]));
	memcpy(mpKernelBuffer + mAnimPatch->offset[1], mAnimPatch->data2[opt], sizeof(mAnimPatch->data2[0]));
	memcpy(mpKernelBuffer + mAnimPatch->offset[2], mAnimPatch->data3[opt], sizeof(mAnimPatch->data3[0]));
	memcpy(mpKernelBuffer + mAnimPatch->offset[3], mAnimPatch->data4[opt], sizeof(mAnimPatch->data4[0]));

	return 1;
}

int CKernelPatcher::WriteXLogo(int XLogo)
{
	memcpy(mpKernelBuffer + mXLogoPatch->offset[0], mXLogoPatch->data1[XLogo], sizeof(mXLogoPatch->data1[0]));
	memcpy(mpKernelBuffer + mXLogoPatch->offset[1], mXLogoPatch->data2[XLogo], sizeof(mXLogoPatch->data2[0]));

	return 1;
}

int CKernelPatcher::WriteSplashLogo(int SplashLogo)
{
	memcpy(mpKernelBuffer + mSplashLogoPatch->offset, mSplashLogoPatch->data[SplashLogo], sizeof(mSplashLogoPatch->data[0]));

	return 1;
}

int CKernelPatcher::WriteTradeMark(int TrademarkSymbol)
{
	mpKernelBuffer[mTMarkPatch->offset] = mTMarkPatch->data[TrademarkSymbol];

	return 1;
}

int CKernelPatcher::WriteDash(unsigned char * BootPart, CString * DashName, int * DashNameLen, CString & DefaultXbeName)
{
	CString PatchData;
	FILE *fh;
	int i, offset, bs;

	if (mSupOpts.ExtendedDashNames)
	{
		if (GetKernelSize() == mNativeSizeOfKernel)
			return 0;
		else
		{
			TCHAR* pstrExePath = PatchData.GetBuffer(MAX_PATH);

			GetModuleFileName (0, pstrExePath, MAX_PATH);
			PatchData.ReleaseBuffer();

			bs = PatchData.ReverseFind('\\');
			PatchData.Truncate(bs + 1);
			PatchData += "Patch Data\\";
			PatchData += mDashPatch->name;

			if (!(fh = fopen(PatchData, "rb")))
				return 0;

			fread(mpKernelBuffer + mNativeSizeOfKernel + 0x2400, sizeof(char), 0x800, fh);
			fclose(fh);
		}
		for (i = 0, offset = mNativeSizeOfKernel + mDashPatch->pExtOffset; i < 3; i++, offset += 158)
		{
			*(mpKernelBuffer + 27 + offset) = BootPart[i];
			strncpy((char *) (mpKernelBuffer + 29 + offset), DashName[i].GetString(), 128);
		}
		*((WORD *)	(mpKernelBuffer + mDashPatch->pHookOffset)) = 0x3d81;
		*((DWORD *)	(mpKernelBuffer + mDashPatch->pHookOffset + 2)) = mDiscardInitPatch->startOfInitSect;
		*((DWORD *)	(mpKernelBuffer + mDashPatch->pHookOffset + 6)) = 0xcafebabe;
		*((WORD *)	(mpKernelBuffer + mDashPatch->pHookOffset + 10)) = 0x0574;
		*((BYTE *)	(mpKernelBuffer + mDashPatch->pHookOffset + 12)) = 0xe9;
		*((DWORD *)	(mpKernelBuffer + mDashPatch->pHookOffset + 13)) = (mNativeSizeOfKernel + 0x2400) - (mDashPatch->pHookOffset + 17);
		*((BYTE *)	(mpKernelBuffer + mDashPatch->pHookOffset + 17)) = 0xe9;
		*((DWORD *)	(mpKernelBuffer + mDashPatch->pHookOffset + 18)) = ((mDiscardInitPatch->startOfInitSect - 0x80010000) + 0x2400) - (mDashPatch->pHookOffset + 22);

	}
	else
	{
		for (i = 0; i < mDashPatch->numOpts; i++)
		{
			*(mpKernelBuffer + 27 + mDashPatch->pOffset[i]) = BootPart[i];
			strncpy((char *) (mpKernelBuffer + 29 + mDashPatch->pOffset[i]), DashName[i].GetBuffer(), mDashPatch->nameLen[i]);
		}
	}
	// default DVD executable
	strncpy((char *) (mpKernelBuffer + *mDefXBEOffset), DefaultXbeName.GetString(), 11);

	return 1;
}

int CKernelPatcher::WriteDvd(int DVDCheck)
{
	int opt = DVDCheck;
	memcpy(mpKernelBuffer + mDvdPatch->offset[0], mDvdPatch->data1[opt], sizeof(mDvdPatch->data1[0]));
	memcpy(mpKernelBuffer + mDvdPatch->offset[1], mDvdPatch->data2[opt], sizeof(mDvdPatch->data2[0]));
	memcpy(mpKernelBuffer + mDvdPatch->offset[2], mDvdPatch->data3[opt], sizeof(mDvdPatch->data3[0]));
	memcpy(mpKernelBuffer + mDvdPatch->offset[3], mDvdPatch->data4[opt], sizeof(mDvdPatch->data4[0]));

	return 1;
}

int CKernelPatcher::WriteLed(unsigned char LEDColor, int FanSpeed)
{
	CString PatchData;
	FILE *fh;
	int bs;


	if (GetKernelSize() == mNativeSizeOfKernel + 0x8000)
	{
		TCHAR* pstrExePath = PatchData.GetBuffer(MAX_PATH);

		GetModuleFileName (0, pstrExePath, MAX_PATH);
		PatchData.ReleaseBuffer();

		bs = PatchData.ReverseFind('\\');
		PatchData.Truncate(bs + 1);
		PatchData += "Patch Data\\";
		PatchData += mLedPatch->name;

		if (!(fh = fopen(PatchData, "rb")))
			return 0;

		fread(mpKernelBuffer + mNativeSizeOfKernel + 0x2200, sizeof(char), 0x200, fh);
		fclose(fh);

		*((BYTE *)	(mpKernelBuffer + mLedPatch->offset)) = 0xe8;
		*((DWORD *)	(mpKernelBuffer + mLedPatch->offset + 1)) = ((mDiscardInitPatch->startOfInitSect - 0x80010000) + 0x2300) - (mLedPatch->offset + 5);
		mpKernelBuffer[mNativeSizeOfKernel + 0x2280] = LEDColor;
		mpKernelBuffer[mNativeSizeOfKernel + 0x2281] = FanSpeed / 2;
	}

	return 1;
}

PIMAGE_SECTION_HEADER CKernelPatcher::GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader)	// 'T' == PIMAGE_NT_HEADERS 
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		DWORD size = section->Misc.VirtualSize;
		if ( 0 == size )
			size = section->SizeOfRawData;
			
        // Is the RVA within this section?
        if ( (rva >= section->VirtualAddress) && 
             (rva < (section->VirtualAddress + size)))
            return section;
    }
    
    return 0;
}

LPVOID CKernelPatcher::GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase ) // 'T' = PIMAGE_NT_HEADERS 
{
	PIMAGE_SECTION_HEADER pSectionHdr;
	INT delta;
		
	pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
	if ( !pSectionHdr )
		return 0;

	delta = (INT)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
	return (PVOID) ( imageBase + rva - delta );
}

unsigned char * CKernelPatcher::GetExportOffset(int exportNum)
{
	PIMAGE_NT_HEADERS pNTHeaders;
	PIMAGE_EXPORT_DIRECTORY pExportDir;
	DWORD exportsStartRVA, exportsEndRVA;
	unsigned long *expTable, expOffset;

	pNTHeaders = (PIMAGE_NT_HEADERS) (mpKernelBuffer + *(unsigned long *) (mpKernelBuffer + 0x3c));
    
    exportsStartRVA = pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    exportsEndRVA = pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		
    pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetPtrFromRVA(exportsStartRVA, pNTHeaders, mpKernelBuffer);
	expTable = (unsigned long *) (mpKernelBuffer + pExportDir->AddressOfFunctions);
	expOffset = expTable[exportNum - 1];

	return (mpKernelBuffer + expOffset);
}

int CKernelPatcher::ReadKernelVersion(KernelVersion * Version)
{
	unsigned char *foo = GetExportOffset(324);
	memcpy(Version, GetExportOffset(324), sizeof(KernelVersion));

	return 1;
}

int CKernelPatcher::WriteBootFlags(unsigned char BootFlags)
{
	*(GetExportOffset(321) - 8) = BootFlags;

	return 1;
}
int CKernelPatcher::ReadBootFlags(unsigned char & BootFlags)
{
	BootFlags = *(GetExportOffset(321) - 8);

	return 1;
}

int CKernelPatcher::ExpandKernel()
{
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_SECTION_HEADER section;

	CString PatchData;
	FILE *fh;
	int bs;

	pNTHeader = (PIMAGE_NT_HEADERS) (mpKernelBuffer + *(unsigned long *) (mpKernelBuffer + 0x3c));
	section = IMAGE_FIRST_SECTION(pNTHeader);

		// add the 32k stuff even if we aren't using it
	if(pNTHeader->OptionalHeader.SizeOfImage == mNativeSizeOfKernel)
	{
		pNTHeader->OptionalHeader.SizeOfImage += 0x8000;
		int SectionsFound = 0;
		for ( unsigned int i = 1; i <= pNTHeader->FileHeader.NumberOfSections && SectionsFound != 2; i++, section++ )
		{
			if (!memcmp(section->Name, "IDEXPRDT", 8))
			{
				section->SizeOfRawData += 0x8000;
				section->Misc.VirtualSize = section->SizeOfRawData;
				section->Characteristics = 0xE8000020;
				SectionsFound++;
			}
			if (!memcmp(section->Name, "INIT", 5))
			{
				section->PointerToRawData += 0x8000;
				section->VirtualAddress += 0x8000;
				SectionsFound++;
			}
		}
	}
	if (pNTHeader->OptionalHeader.SizeOfImage != mNativeSizeOfKernel + 0x8000)
		return 0;

	memset(mpKernelBuffer + mNativeSizeOfKernel, 0x50, 0x8000);

	TCHAR* pstrExePath = PatchData.GetBuffer(MAX_PATH);

	GetModuleFileName (0, pstrExePath, MAX_PATH);
	PatchData.ReleaseBuffer();

	bs = PatchData.ReverseFind('\\');
	PatchData.Truncate(bs + 1);
	PatchData += "Patch Data\\";
	PatchData += mDiscardInitPatch->name;

	if (!(fh = fopen(PatchData, "rb")))
		return 0;

	fread(mpKernelBuffer + mNativeSizeOfKernel, sizeof(char), 0x200, fh);
	fclose(fh);

//	memcpy(mpKernelBuffer + mDiscardInitPatch->offset, mDiscardInitPatch->data[0], sizeof(mDiscardInitPatch->data[0]));

	*((BYTE *)	(mpKernelBuffer + mDiscardInitPatch->offset)) = 0xe9;
	*((DWORD *)	(mpKernelBuffer + mDiscardInitPatch->offset + 1)) = (mNativeSizeOfKernel + 0x10) - (mDiscardInitPatch->offset + 5);

	return 1;
}

int CKernelPatcher::ShrinkKernel()
{
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_SECTION_HEADER section;

	pNTHeader = (PIMAGE_NT_HEADERS) (mpKernelBuffer + *(unsigned long *) (mpKernelBuffer + 0x3c));
	section = IMAGE_FIRST_SECTION(pNTHeader);

	// shrink kernel
	if (pNTHeader->OptionalHeader.SizeOfImage == mNativeSizeOfKernel + 0x8000)
	{
		pNTHeader->OptionalHeader.SizeOfImage = mNativeSizeOfKernel;

		int SectionsFound = 0;
		for ( unsigned int i = 1; i <= pNTHeader->FileHeader.NumberOfSections && SectionsFound != 2; i++, section++ )
		{
			if (!memcmp(section->Name, "IDEXPRDT", 8))
			{
				section->SizeOfRawData -= 0x8000;
				section->Misc.VirtualSize = mLBA48Patch->IDEXPRDT_Vsz;
				section->Characteristics = 0xC8000040;
				SectionsFound++;
			}
			if (!memcmp(section->Name, "INIT", 5))
			{
				section->PointerToRawData -= 0x8000;
				section->VirtualAddress -= 0x8000;
				SectionsFound++;
			}
		}
	}
	// undo patches
	memcpy(mpKernelBuffer + mDiscardInitPatch->offset, mDiscardInitPatch->data, sizeof(mDiscardInitPatch->data));

	return 1;
}

int CKernelPatcher::WriteLBA48(int LBA48, int PartitionOpt, int IgnoreHDTable)
{
	CString PatchData;
	FILE *fh;
	int bs, i;

	// patch the hook points if LBA48
	if (LBA48)
	{
		for (i = 0; i < 4; i++)
		{
			*((WORD *)	(mpKernelBuffer + mLBA48Patch->offset[i])) = 0x3d81;
			*((DWORD *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 2)) = mDiscardInitPatch->startOfInitSect;
			*((DWORD *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 6)) = 0xcafebabe;
			*((WORD *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 10)) = 0x0574;
			*((BYTE *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 12)) = 0xe9;
			*((DWORD *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 13)) = (mNativeSizeOfKernel + mLBA48Patch->jumpDest[i]) - (mLBA48Patch->offset[i] + 17);
			*((BYTE *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 17)) = 0xe9;
			*((DWORD *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 18)) = ((mDiscardInitPatch->startOfInitSect - 0x80010000) + mLBA48Patch->jumpDest[i]) - (mLBA48Patch->offset[i] + 22);
		}
		while (i < 6)
		{
			*((BYTE *)	(mpKernelBuffer + mLBA48Patch->offset[i])) = 0xe9;
			*((DWORD *)	(mpKernelBuffer + mLBA48Patch->offset[i] + 1)) = (mNativeSizeOfKernel + mLBA48Patch->jumpDest[i]) - (mLBA48Patch->offset[i] + 5);
			i++;
		}

		TCHAR* pstrExePath = PatchData.GetBuffer(MAX_PATH);

		GetModuleFileName (0, pstrExePath, MAX_PATH);
		PatchData.ReleaseBuffer();

		bs = PatchData.ReverseFind('\\');
		PatchData.Truncate(bs + 1);
		PatchData += "Patch Data\\";
		PatchData += mLBA48Patch->name;

		if (!(fh = fopen(PatchData, "rb")))
			return 0;

		fread(mpKernelBuffer + mNativeSizeOfKernel + 0x200, sizeof(char), 0x2000, fh);
		fclose(fh);

		mpKernelBuffer[mNativeSizeOfKernel + 0x204] = PartitionOpt;
		mpKernelBuffer[mNativeSizeOfKernel + 0x205] = IgnoreHDTable;
	}
	else
	{
		memcpy(mpKernelBuffer + mLBA48Patch->offset[0], mLBA48Patch->data1, sizeof(mLBA48Patch->data1));
		memcpy(mpKernelBuffer + mLBA48Patch->offset[1], mLBA48Patch->data2, sizeof(mLBA48Patch->data2));
		memcpy(mpKernelBuffer + mLBA48Patch->offset[2], mLBA48Patch->data3, sizeof(mLBA48Patch->data3));
		memcpy(mpKernelBuffer + mLBA48Patch->offset[3], mLBA48Patch->data4, sizeof(mLBA48Patch->data4));
		memcpy(mpKernelBuffer + mLBA48Patch->offset[4], mLBA48Patch->data5, sizeof(mLBA48Patch->data5));
		memcpy(mpKernelBuffer + mLBA48Patch->offset[5], mLBA48Patch->data6, sizeof(mLBA48Patch->data6));
	}
	return 1;
}

int CKernelPatcher::ReadLBA48(int & LBA48, int & PartitionOpt, int & IgnoreHDTable)
{
	PIMAGE_NT_HEADERS pNTHeader;
	pNTHeader = (PIMAGE_NT_HEADERS) (mpKernelBuffer + *(unsigned long *) (mpKernelBuffer + 0x3c));

	if(!memcmp(mLBA48Patch->data1, (mpKernelBuffer + mLBA48Patch->offset[0]), sizeof(mLBA48Patch->offset[0])))
	{
		LBA48 = 0;
		PartitionOpt = 2;
		IgnoreHDTable = 0;
	}
	else
	{
		LBA48 = 1;
		PartitionOpt = mpKernelBuffer[mNativeSizeOfKernel + 0x204];
		IgnoreHDTable = mpKernelBuffer[mNativeSizeOfKernel + 0x205];
	}

	return 1;
}

int CKernelPatcher::ReadEeprom(int & EmbedEeprom)
{
	if(*(mpKernelBuffer + mEepromPatch->functOffset) == mEepromPatch->data[0])
		EmbedEeprom = 0;
	else
		EmbedEeprom = 1;
	return 1;
}

int CKernelPatcher::WriteEeprom(int & EmbedEeprom)
{
	CString PatchData, Eeprom;
	FILE *fh;
	int bs;


	if (GetKernelSize() == mNativeSizeOfKernel + 0x8000)
	{
		TCHAR* pstrExePath = PatchData.GetBuffer(MAX_PATH);

		GetModuleFileName (0, pstrExePath, MAX_PATH);
		PatchData.ReleaseBuffer();

		bs = PatchData.ReverseFind('\\');
		PatchData.Truncate(bs + 1);
		PatchData += "Patch Data\\";
		Eeprom = PatchData;
		PatchData += mEepromPatch->name;
		Eeprom += "eeprom.bin";

		if (!(fh = fopen(PatchData, "rb")))
			return 0;

		if(EmbedEeprom)
			if(fseek(fh, (long) mEepromPatch->functSize, SEEK_SET))
				return 0;

		fread(mpKernelBuffer + mEepromPatch->functOffset, sizeof(char), mEepromPatch->functSize, fh);
		fclose(fh);

		if(EmbedEeprom)
		{
			if (!(fh = fopen(Eeprom, "rb")))
				return 0;

			fread(mpKernelBuffer + mNativeSizeOfKernel + 0x2C00, sizeof(char), 0x100, fh);
			fclose(fh);
		}
	}
	return 1;
}

int CKernelPatcher::ReadHDKey(int & EmbedHDKey, unsigned char * key)
{
	if (*(mpKernelBuffer + mEmbedHDKeyPatch->offset) == *mEmbedHDKeyPatch->data[0])
		EmbedHDKey = 0;
	else
	{
		if (GetKernelSize() == mNativeSizeOfKernel + 0x8000)
		{
			EmbedHDKey = 1;
			memcpy(key, mpKernelBuffer + mNativeSizeOfKernel + 0x2D00, 0x20);
		}
		else
			return 0;
	}
	return 1;
}

int CKernelPatcher::WriteHDKey(int & EmbedHDKey, unsigned char * key)
{
	int kSz = GetKernelSize();

	if (EmbedHDKey)
	{
		if (kSz == mNativeSizeOfKernel + 0x8000)
		{
			memcpy(mpKernelBuffer + mEmbedHDKeyPatch->offset, mEmbedHDKeyPatch->data[1], sizeof(mEmbedHDKeyPatch->data[0]));
			memcpy(mpKernelBuffer + mNativeSizeOfKernel + 0x2D00, key, 0x20);
		}
		else
			return 0;
	}
	else
	{
		memcpy(mpKernelBuffer + mEmbedHDKeyPatch->offset, mEmbedHDKeyPatch->data[0], sizeof(mEmbedHDKeyPatch->data[0]));
		if (kSz == mNativeSizeOfKernel + 0x8000)
			memset((char *) mpKernelBuffer + mNativeSizeOfKernel + 0x2D00, 0x50, 0x20);
	}
	return 1;
}

int CKernelPatcher::KernelID(unsigned char * kernelBuffer)
{
	unsigned char digest[16];
	MD5_CTX ctx;

	PIMAGE_NT_HEADERS pNTHeader;
	pNTHeader = (PIMAGE_NT_HEADERS) (kernelBuffer + *(unsigned long *) (kernelBuffer + 0x3c));

//	BIOS_Options opts = {	"", 0, 0, 0, 0, 0, 1, 1, 1, 1, '2', '2', '2', ".", ".", ".", 0, 0, 0,
//		".", 1, 0x0f, 0, 0, 0, 0, 0};

	BIOS_Options opts;

	opts.XLipHighlightsColor = 0;
	opts.XLipColor = 0;
	opts.XWallColor = 0;
	opts.XGlowColor = 0;
	opts.XboxColor = 0;
	opts.Animation = 1;
	opts.XLogo = 1;
	opts.SplashLogo = 1;
	opts.TrademarkSymbol = 1;
	opts.BootPart[0] = '2';
	opts.BootPart[1] = '2';
	opts.BootPart[2] = '2';
	opts.DashName[0] = ".";
	opts.DashName[1] = ".";
	opts.DashName[2] = ".";
	opts.DashNameLen[0] = 0;
	opts.DashNameLen[1] = 0;
	opts.DashNameLen[2] = 0;
	opts.DefaultXbeName = ".";
	opts.DVDCheck = 1;
	opts.LEDColor = 0x0F;
	opts.BootFlags = 0;
	opts.LBA48 = 0;
	opts.PartitionOpt = 0;
	opts.IgnoreHDTable = 0;
	opts.FanSpeed = 0;
	opts.ResetOnEject = 1;
	opts.EmbedEeprom = 0;
	opts.EmbedHDKey = 0;

	if (mNativeSizeOfKernel > pNTHeader->OptionalHeader.SizeOfImage)
		return 0;

	if ((mpKernelBuffer = (unsigned char *) malloc(pNTHeader->OptionalHeader.SizeOfImage)) == 0)
		return 0;

	memcpy(mpKernelBuffer, kernelBuffer, pNTHeader->OptionalHeader.SizeOfImage);

	WritePreIDPatches(&opts);

	MD5Init(&ctx);
	MD5Update(&ctx, mpKernelBuffer + pNTHeader->OptionalHeader.SizeOfHeaders,
		mNativeSizeOfKernel - pNTHeader->OptionalHeader.SizeOfHeaders);
	MD5Final(digest, &ctx);
	free(mpKernelBuffer);

	if (memcmp(mID_Digest, digest, 16))
		return 0;

	return 1;
}

int CKernelPatcher::GetKernelSize()
{
	PIMAGE_NT_HEADERS pNTHeader;

	pNTHeader = (PIMAGE_NT_HEADERS) (mpKernelBuffer + *(unsigned long *) (mpKernelBuffer + 0x3c));

	return pNTHeader->OptionalHeader.SizeOfImage;

}
