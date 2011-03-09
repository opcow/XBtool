//////////////////////////////////////////////////////////////////////////////
// CXboxKernel.cpp
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
#include "stdafx.h"
#include <stdio.h>
#include <atlstr.h>
#include "XBpatch.h"
#include "CXboxKernel.h"

CXboxKernel::CXboxKernel()
{
	mpKernelPatcher = &UnknownPatcher;
	mpKernelBuffer = 0;
	InitOpts();
	strncpy(mOpts.Name, "Kernel Not Loaded", 63);
	Read();
}
CXboxKernel::CXboxKernel(const char *kernelFile)
{
	FILE *fh;
	unsigned int filesize;

	if (!(fh = fopen(kernelFile, "rb")))
		return;

	if (fseek(fh, 0L, SEEK_END))
		return;

	filesize = ftell(fh);

	if (!filesize)
	{
		fclose(fh);
		return;
	}

	if (!(mpKernelBuffer = new unsigned char[filesize + 0x8000]))
	{
		fclose(fh);
		return;
	}

	fseek(fh, 0L, SEEK_SET);
	if ((fread(mpKernelBuffer, 1, filesize, fh) != filesize))
	{
		delete [] mpKernelBuffer;
		fclose(fh);
		return;
	}

	fclose(fh);

	mSKernelFile = kernelFile;
	mKernelSize = filesize;

	memset(mpKernelBuffer + filesize, 0x50, 0x8000);

	UnknownPatcher.InitPatches();
	X24981Patcher.InitPatches();
	X24979Patcher.InitPatches();
	X24977Patcher.InitPatches();
	X24976Patcher.InitPatches();
	EvoxD6Patcher.InitPatches();
	EvoxM7Patcher.InitPatches();
	X24978Patcher.InitPatches();

	GetPatcher();
	InitOpts();
	Read();
}

CXboxKernel::~CXboxKernel()
{
	if (mpKernelBuffer)
		delete [] mpKernelBuffer;
}

void CXboxKernel::GetPatcher()
{
	if (X24981Patcher.KernelID(mpKernelBuffer))
		mpKernelPatcher = &X24981Patcher;
	else if (X24979Patcher.KernelID(mpKernelBuffer))
		mpKernelPatcher = &X24979Patcher;
	else if (X24977Patcher.KernelID(mpKernelBuffer))
		mpKernelPatcher = &X24977Patcher;
	else if (X24978Patcher.KernelID(mpKernelBuffer))
		mpKernelPatcher = &X24978Patcher;
	else if (X24976Patcher.KernelID(mpKernelBuffer))
		mpKernelPatcher = &X24976Patcher;
	else if (EvoxD6Patcher.KernelID(mpKernelBuffer))
		mpKernelPatcher = &EvoxD6Patcher;
	else if (EvoxM7Patcher.KernelID(mpKernelBuffer))
		mpKernelPatcher = &EvoxM7Patcher;
	else
		mpKernelPatcher = &UnknownPatcher;
}

int CXboxKernel::Read()
{
	mpKernelPatcher->Read(mpKernelBuffer, &mOpts);
	mpKernelPatcher->GetSupportedOptions( mSupportedOpts);

	return 0;
}

bool CXboxKernel::IsSupported()
{
	return mOpts.Supported;
}

int CXboxKernel::Write()
{
	FILE *fh;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) (mpKernelBuffer + *(unsigned long *) (mpKernelBuffer + 0x3c));
	int err;

	if(err = mpKernelPatcher->Write(mpKernelBuffer, &mOpts))
		return err;
	mKernelSize = pNTHeader->OptionalHeader.SizeOfImage;

	if (!(fh = fopen(mSKernelFile, "wb")))
		return errKernelWrite;
	fwrite(mpKernelBuffer, sizeof(char), mKernelSize, fh);
	fclose(fh);

	return 0;
}

int CXboxKernel::WriteBootFlags()
{
	FILE *fh;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) (mpKernelBuffer + *(unsigned long *) (mpKernelBuffer + 0x3c));
	int err;

	mpKernelPatcher->WriteBootFlags(mOpts.BootFlags);

	mKernelSize = pNTHeader->OptionalHeader.SizeOfImage;

	if (!(fh = fopen(mSKernelFile, "wb")))
		return errKernelWrite;
	fwrite(mpKernelBuffer, sizeof(char), mKernelSize, fh);
	fclose(fh);

	return 0;
}

void CXboxKernel::InitOpts()
{
	// init the options
//	strcpy(mOpts.Name, "Unsupported Kernel");
	mOpts.XLipHighlightsColor = DefXLipHighlightsColor;
	mOpts.XLipColor = DefXLipColor;
	mOpts.XWallColor = DefXLogoInnerColor;
	mOpts.XGlowColor = DefXLogoGlowColor;
	mOpts.XboxColor = DefLogoColor;
	mOpts.Animation = 1;
	mOpts.XLogo = 1;
	mOpts.SplashLogo = 1;
	mOpts.TrademarkSymbol = 1;
	mOpts.BootPart[0] = 0x32;
	mOpts.BootPart[1] = 0xFF;
	mOpts.BootPart[2] = 0xFF;
	mOpts.DashName[0] = "xboxdash.xbe";
	mOpts.DashName[1] = "";
	mOpts.DashName[2] = "";
	mOpts.DashNameLen[0] = 12;
	mOpts.DashNameLen[1] = 0;
	mOpts.DashNameLen[2] = 0;
	mOpts.DefaultXbeName = "default.xbe";
	mOpts.DVDCheck = 1;
	mOpts.LEDColor = 0x0F;
	mOpts.BootFlags = 0;
	mOpts.LBA48 = 0;
	mOpts.PartitionOpt = 2;
	mOpts.IgnoreHDTable = 0;
	mOpts.FanSpeed = 20;
	mOpts.ResetOnEject = 1;
	mOpts.EmbedEeprom = 0;
	mOpts.EmbedHDKey = 0;

}