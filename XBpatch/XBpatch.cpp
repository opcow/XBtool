//////////////////////////////////////////////////////////////////////////////
// XBPatch.cpp
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
// Patching and IDing functions.

#include "stdafx.h"
#include <atlstr.h>
#include "XBpatch.h"
#include "MD5\MD5.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

unsigned long XBPatchGetVersion()
{
	return 0x01050900;
}

// MD5 a BIOS file (just for Evox MD5 generation--otherwise serves no purpose)
int getIDString(const char *fname, char *biosID)
{
	FILE *fh;
	const int bufsize = 0x40000;
	size_t size, totalRead = 0;
	unsigned char *buf;
	unsigned char digest[16];
	MD5_CTX ctx;

	if ((buf = (unsigned char *) malloc(bufsize)) == NULL)
		return 0;

	if (fh = fopen(fname, "rb"))
	{
		MD5Init(&ctx);

		while (size = fread(buf, sizeof(char), bufsize, fh))
		{
			MD5Update(&ctx, buf, (int) size);
			totalRead += size;
			if (size < bufsize || totalRead == 0x40000)
				break;
		}
		free(buf);
		MD5Final(digest, &ctx);
		// format the BIOS ID string in ascii hex representation
		sprintf(biosID, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], 
			digest[7], digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], 
			digest[14], digest[15]);

		return 1;
	}
	return 0;
}

// take the ascii RC4 and translate it into a 16 byte array
int RC4StrToArray(CString RC4Str, unsigned char *RC4)
{
	CString token;
	int i = 0, pos = 0;

	RC4Str.Replace("0x", "");	// get rid of the 0x

	if (token = RC4Str.Tokenize(" ", pos))	// now bytes are separated by only spaces
	{										// so tokenize with space as separator
		for (i = 0; i < 16 && token != ""; i++)
		{
			RC4[i] = (unsigned char) strtoul(token, '\0', 16);
			token= RC4Str.Tokenize(" ",pos);
		}
	}
	if (i == 16)
		return 1;
	else
		return 0; // failed to get 16 chars
}

void initBIOSOptions(BIOS_Options *BIOSOptions)
{
	strcpy(BIOSOptions->Name, "Unsupported Kernel");
	BIOSOptions->XLipHighlightsColor = DefXLipHighlightsColor;
	BIOSOptions->XLipColor = DefXLipColor;
	BIOSOptions->XWallColor = DefXLogoInnerColor;
	BIOSOptions->XGlowColor = DefXLogoGlowColor;
	BIOSOptions->XboxColor = DefLogoColor;
	BIOSOptions->Animation = 1;
	BIOSOptions->XLogo = 1;
	BIOSOptions->SplashLogo = 1;
	BIOSOptions->TrademarkSymbol = 1;
	BIOSOptions->BootPart[0] = 0x32;
	BIOSOptions->BootPart[1] = 0xFF;
	BIOSOptions->BootPart[2] = 0xFF;
	BIOSOptions->DashName[0] = "xboxdash.xbe";
	BIOSOptions->DashName[1] = "";
	BIOSOptions->DashName[2] = "";
	BIOSOptions->DashNameLen[0] = 12;
	BIOSOptions->DashNameLen[1] = 0;
	BIOSOptions->DashNameLen[2] = 0;
	BIOSOptions->DefaultXbeName = "default.xbe";
	BIOSOptions->DVDCheck = 1;
	BIOSOptions->LEDColor = 0x0F;
	BIOSOptions->BootFlags = 0;
	BIOSOptions->LBA48 = 0;
	BIOSOptions->PartitionOpt = 0;
	BIOSOptions->IgnoreHDTable = 0;
	BIOSOptions->ResetOnEject = 0;
	BIOSOptions->EmbedEeprom = 0;
	BIOSOptions->EmbedHDKey = 0;

	BIOSOptions->Supported = false;
	return;
}

int resizeBIOS(const char *biosFPName, const char *saveFPName, int theRomSize)
{
	FILE *fh;
	int oldRomSize;
	char *buf;
	CString newName;

	fh = fopen(biosFPName, "rb");
	fseek(fh, 0L, SEEK_END);
	oldRomSize = ftell(fh);
	if (!oldRomSize || (oldRomSize & (oldRomSize - 1)))
	{
		fclose(fh);
		::MessageBox ( NULL, "Invalid file size.", "Error", MB_ICONSTOP);
		return 0;
	}
	if ( (buf = (char *) malloc(oldRomSize)) == NULL )
	{
		fclose(fh);
		::MessageBox ( NULL, "Couldn't allocate memory for ROM.", "Error", MB_ICONSTOP);
		return 0;
	}
	fseek(fh, -oldRomSize, SEEK_END);
	if ((fread(buf, sizeof(char), oldRomSize, fh) != oldRomSize))
	{
		free(buf);
		fclose(fh);
		::MessageBox ( NULL, "Error reading ROM file.", "Error", MB_ICONSTOP);
		return 0;
	}
	fclose(fh);

	fh = fopen(saveFPName, "wb");

	if (theRomSize > oldRomSize)
	{
		for (int i = theRomSize / oldRomSize; i > 0; i--)
			if (fwrite(buf, sizeof(char), oldRomSize, fh) != oldRomSize)
			{
				free(buf);
				fclose(fh);
				::MessageBox ( NULL, "Error writing ROM file.", "Error", MB_ICONSTOP);
				return 0;
			}
	}
	else
		if (fwrite(buf + (oldRomSize - theRomSize), sizeof(char), theRomSize, fh) != theRomSize)
		{
			free(buf);
			fclose(fh);
			::MessageBox ( NULL, "Error writing ROM file.", "Error", MB_ICONSTOP);
			return 0;
		}

	free(buf);
	fclose(fh);
	return 1;
}
