//////////////////////////////////////////////////////////////////////////////
// CUnknownPatcher.cpp
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
#include "CUnknownPatcher.h"

int CUnknownPatcher::Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	mpKernelBuffer = kernelBuffer;
	BIOSOptions = BIOSOptions;

	BIOSOptions->Supported = false;
	strncpy(BIOSOptions->Name, "Unsupported Kernel", 63);

	if(kernelBuffer)
	{
		ReadBootFlags(BIOSOptions->BootFlags);
		ReadKernelVersion(&BIOSOptions->Version);
	}
	return 1;
}


int CUnknownPatcher::Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	mpKernelBuffer = kernelBuffer;
	BIOSOptions = BIOSOptions;

	if (!WriteBootFlags(BIOSOptions->BootFlags))
		return errWriteBootFlags;

	return 0;
}

void CUnknownPatcher::GetSupportedOptions(BIOS_Supported_Options &SupOpts)
{
	mSupOpts.Animation				= 0;
	mSupOpts.BootPart				= 0;
	mSupOpts.DashName				= 0;
	mSupOpts.ExtendedDashNames		= 0;
	mSupOpts.DefaultXbeName			= 0;
	mSupOpts.DVDCheck				= 0;
	mSupOpts.LEDColor				= 1;
	mSupOpts.SplashLogo				= 0;
	mSupOpts.TrademarkSymbol		= 0;
	mSupOpts.XboxColor				= 1;
	mSupOpts.XGlowColor				= 1;
	mSupOpts.XLipColor				= 1;
	mSupOpts.XLipHighlightsColor	= 1;
	mSupOpts.XLogo					= 0;
	mSupOpts.XWallColor				= 1;
	mSupOpts.BootFlags				= 1;
	mSupOpts.LBA48					= 0;
	mSupOpts.IgnoreHDTable			= 0;
	mSupOpts.PartitionOpt			= 0;
	mSupOpts.FanSpeed				= 0;
	mSupOpts.ResetOnEject			= 0;
	mSupOpts.EmbedEeprom			= 0;
	mSupOpts.EmbedHDKey				= 0;

	SupOpts = mSupOpts;
}

