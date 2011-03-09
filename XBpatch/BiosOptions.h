//////////////////////////////////////////////////////////////////////////////
// BiosOptions.h
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

enum patchErrors
{
	errWriteColor = 1,
	errWriteAni,
	errWriteXLogo,
	errWriteSpLogo,
	errWriteTM,
	errWriteDash,
	errWriteDVD,
	errWriteLED,
	errWriteLBA48,
	errWriteBootFlags,
	errWriteExpKernel,
	errKernelWrite,
	errWriteEeprom,
	errWriteHDKey
};

typedef struct
{
	int				XLipHighlightsColor;	// colorref for the color of the lip around the X
	int				XLipColor;				// the spot color on the X
	int				XWallColor;				// the walls in the X
	int				XGlowColor;				// the glow within the X
	int				XboxColor;				// the word "XBOX"
	int				Animation;				// flubber animation toggle
	int				XLogo;					// big X
	int				SplashLogo;				// little graphic (e.g. Microsoft) before the dash loads
	int				TrademarkSymbol;		// the teensy TM symbols 
	int				BootPart;				// boot partitions
	int				DashName;				// names for dashboards
	int				ExtendedDashNames;
	int				DefaultXbeName;			// default.xbe
	int				DVDCheck;				// DVD check flag
	int				LEDColor;
	int				BootFlags;
	int				LBA48;
	int				FanSpeed;
	int				PartitionOpt;
	int				IgnoreHDTable;
	int				ResetOnEject;
	int				EmbedEeprom;
	int				EmbedHDKey;

} BIOS_Supported_Options;

typedef struct
{
	unsigned short xbox;
	unsigned short major;
	unsigned short minor;
	unsigned char rev;

} KernelVersion;

// this structure holds all the options for a BIOS and some identifying data
typedef struct
{
	char			Name[64];
	COLORREF		XLipHighlightsColor;	// colorref for the color of the lip around the X
	COLORREF		XLipColor;				// the spot color on the X
	COLORREF		XWallColor;				// the walls in the X
	COLORREF		XGlowColor;				// the glow within the X
	COLORREF		XboxColor;				// the word "XBOX"
	int				Animation;				// flubber animation toggle
	int				XLogo;					// big X
	int				SplashLogo;				// little graphic (e.g. Microsoft) before the dash loads
	int				TrademarkSymbol;		// the teensy TM symbols 
	unsigned char	BootPart[3];			// boot partitions
	CString			DashName[3];			// names for dashboards
	int				DashNameLen[3];			// length of dash name
	CString			DefaultXbeName;			// default.xbe
	int				DVDCheck;				// DVD check flag
	unsigned char	LEDColor;
	unsigned char	BootFlags;
	int				LBA48;
	int				PartitionOpt;
	int				IgnoreHDTable;
	int				FanSpeed;
	bool			Supported;
	KernelVersion	Version;
	int				ResetOnEject;
	int				EmbedEeprom;
	int				EmbedHDKey;
	unsigned char	HDKey[32];
} BIOS_Options;

struct AnimPatch
{
	int offset[4];
	unsigned char data1[2][2];
	unsigned char data2[2][2];
	unsigned char data3[2][5];
	unsigned char data4[2][5];
};

struct XLogoPatch
{
	int offset[2];
	unsigned char data1[2][5];
	unsigned char data2[2][5];
};

struct SplashLogoPatch
{
	int offset;
	unsigned char data[2][2];
};

struct TMarkPatch
{
	int offset;
	unsigned char data[2];
};

struct DashPatch
{
	char *name;
	int pHookOffset;
	long pOffset[3];
	unsigned char data[22];
	int numOpts;
	int nameLen[3];
	long pExtOffset;
};


struct LedPatch
{
	char *name;
	int offset;
	unsigned char data[5];
};

struct DvdPatch
{
	int offset[4];
	unsigned char data1[2][5];
	unsigned char data2[2][5];
	unsigned char data3[2][2];
	unsigned char data4[2][5];
};

struct DiscardInitPatch
{
	char *name;
	int offset;
	DWORD startOfInitSect;
	unsigned char data[5];
};

struct LBA48Patch
{
	char *name;
	int offset[6];
	int jumpDest[6];
	unsigned char data1[22];
	unsigned char data2[22];
	unsigned char data3[22];
	unsigned char data4[22];
	unsigned char data5[5];
	unsigned char data6[5];
	unsigned int IDEXPRDT_Vsz;
};

struct ResetOnEjectPatch
{
	int offset;
	unsigned char data[2][9];
};

struct EepromPatch
{
	char *name;
	int functOffset;
	int functSize;
	unsigned char data[2];
};

struct EmbedHDKeyPatch
{
	int offset;
	unsigned char data[2][37];
};
