//////////////////////////////////////////////////////////////////////////////
// XBtool - Xbox BIOS pack/unpack/patch tool
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
// Presets.cpp
// preset loading and saving functions

#include "stdafx.h"
#include "..\XBpatch\XBpatch.h"
#include "Presets.h"
#include "MD5\MD5.h"

// save presets to a file
int savePreset(const CString *presetFile, BIOS_Options *BIOSOptions)
{
	const int kMaxLine = 9;			// each preset will be 8 bytes plus terminating null
	const int kLines = 13;			// number of preset items

	FILE *fh;
	size_t bytesWritten;

	CString MD5Str;
	char buf[kLines][kMaxLine];
	char *ver = "XBT3";

	unsigned char digest[16];
	MD5_CTX ctx;

	if (fh = fopen(*presetFile, "wb"))
	{
		memset(buf, 0, sizeof(buf));

		int i = 0;
		ltoa( BIOSOptions->XLipHighlightsColor, buf[i++], 16);
		ltoa( BIOSOptions->XLipColor, buf[i++], 16);
		ltoa( BIOSOptions->XGlowColor, buf[i++], 16);
		ltoa( BIOSOptions->XWallColor, buf[i++], 16);
		ltoa( BIOSOptions->XboxColor, buf[i++], 16);
		ltoa( BIOSOptions->Animation, buf[i++], 16);
		ltoa( BIOSOptions->XLogo, buf[i++], 16);
		ltoa( BIOSOptions->SplashLogo, buf[i++], 16);
		ltoa( BIOSOptions->TrademarkSymbol, buf[i++], 16);
		ltoa(0, buf[i++], 16);
		ltoa(0, buf[i++], 16);
		ltoa( BIOSOptions->LEDColor, buf[i], 16);

		MD5Init(&ctx);
		MD5Update(&ctx, (unsigned char *) buf, sizeof(buf));
		MD5Final(digest, &ctx);

		bytesWritten = fwrite(ver, sizeof(char), 5, fh);
		bytesWritten = fwrite(buf, sizeof(char), sizeof(buf), fh);
		bytesWritten= fwrite(digest, sizeof(char), sizeof(digest), fh);

		fclose(fh);
		return 1; // success
	}
	return 0; // filie didn't open
}

int loadPreset(const CString *presetFile, BIOS_Options *BIOSOptions)
{

	const int kMaxLine = 9;
	const int kLines = 13;			// number of preset items

	FILE *fh;
	size_t bytesRead;

	CString MD5Str;
	char buf[kLines][kMaxLine];
	char ver[5];

	unsigned char digest[16], savedDigest[16];
	MD5_CTX ctx;

	MD5Init(&ctx);

	if (fh = fopen(*presetFile, "rb"))
	{
		memset(buf, 0, sizeof(buf));

		bytesRead = fread(ver, sizeof(char), sizeof(ver), fh);

		if(ver[3] == '1')
			bytesRead = fread(buf, sizeof(char), sizeof(buf) - 9, fh);
		else
			bytesRead = fread(buf, sizeof(char), sizeof(buf), fh);
		bytesRead = fread(savedDigest, sizeof(char), sizeof(savedDigest), fh);
		fclose(fh);

		MD5Init(&ctx);
		if(ver[3] == '1')
			MD5Update(&ctx, (unsigned char *) buf, sizeof(buf) - 9);
		else
			MD5Update(&ctx, (unsigned char *) buf, sizeof(buf));
		MD5Final(digest, &ctx);

		if ( (memcmp(digest, savedDigest, sizeof(digest)) != 0) )
			return 0;

		int i = 0;
		BIOSOptions->XLipHighlightsColor = strtoul(buf[i++], '\0', 16);
		BIOSOptions->XLipColor = strtoul(buf[i++], '\0', 16);
		BIOSOptions->XGlowColor = strtoul(buf[i++], '\0', 16);
		BIOSOptions->XWallColor = strtoul(buf[i++], '\0', 16);
		BIOSOptions->XboxColor = strtoul(buf[i++], '\0', 16);
		BIOSOptions->Animation = strtoul(buf[i++], '\0', 16);
		BIOSOptions->XLogo = strtoul(buf[i++], '\0', 16);
		BIOSOptions->SplashLogo = strtoul(buf[i++], '\0', 16);
		BIOSOptions->TrademarkSymbol = strtoul(buf[i++], '\0', 16);
		i++;
		i++; //skip partition and DVD stuff
		BIOSOptions->LEDColor = (unsigned char) strtoul(buf[i], '\0', 16);

		if(ver[3] != '3')
		{
			switch(BIOSOptions->BootPart[0])
			{
			case 'C':
				BIOSOptions->BootPart[0] = 0x32;
				break;

			case 'E':
				BIOSOptions->BootPart[0] = 0x31;
				break;

			case 'F':
				BIOSOptions->BootPart[0] = 0x36;
				break;

			default:
				BIOSOptions->BootPart[0] = 0x32;
				break;
			}
		}
		return 1; // success
	}
	return 0; // file didn't open
}