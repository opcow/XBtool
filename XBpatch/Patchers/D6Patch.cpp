//////////////////////////////////////////////////////////////////////////////
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
//D6Patch.cpp

#include "stdafx.h"
#include <atlstr.h>
#include "..\XBpatch.h"
#include "Patcher.h"

const int	kBootNum = 2;
const long	kAddressAdjust = 0x80010000;
static char *partitonStr = "\\Device\\Harddisk0\\Partition2\\";

///////////////////////////////////////////////////////////////////////////////////////
// colors
const static int XGlowOffset[] =			{ 0x0004C104, 0x0004C10E, 0x0004C117 };
const static int XWallOffset[] =			{ 0x0004C121, 0x0004C12A, 0x0004C134 };
const static int XLipHighlightsOffset =		0x0004B772;
const static int XLipOffset =				0x0004B777;
const static int XBOXTextOffset =			0x0004C2C4;

// Animation
const static anim Anim =
{
	{ 0x00046B61, 0x000472C8, 0x000479C2, 0x000479EA },
	{ { 0x90, 0x90 }, { 0x74, 0x09 } },
	{ { 0x90, 0x90 }, { 0x74, 0x09 } },
	{ { 0xE9, 0x0B, 0x00, 0x00, 0x00 }, { 0x75, 0x0E, 0xE8, 0xA4, 0x9C } },
	{ { 0xE9, 0x02, 0x00, 0x00, 0x00 }, { 0x75, 0x05, 0xE8, 0xB6, 0x9C } }
};
// X logo
const static xlogo XLogo =
{
	{ 0x00046EAB, 0x000479AC },
	{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0xA7, 0xF7, 0xFF, 0xFF } },
	{ { 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0xE8, 0x5A, 0xF3, 0xFF, 0xFF } },
};
// splash logo
const static splashlogo SplashLogo =
{
	0x00045476,
	{ { 0x90, 0x90 }, { 0x75, 0x0B } }
};

const static tmark TMark =
{
	0x0004C1C2,
	{ 0x20, 0x18 }
};

const static dash Dash =
{
	{ 0x0001F2D6, 0x0001EC8D, 0x0001F498 },
	{ 12, 18, 11}
};

const static led LED =
{
	{ 0x00005811, 0x00005816, 0x0000581F },
	{ { 0x90, 0x90, 0x90, 0x90 }, { 0xA8, 0x20, 0x74, 0x11 } },
	0xE1,
	{ { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, { 0x6A, 0x15, 0xE8, 0xFD, 0xFC, 0x03, 0x00 } },
};
const static dvd DVD =
{
	{ 0x00045D6E, 0x00045DC2, 0x00045DE0, 0x00045DFF },
	{ { 0xE9, 0x31, 0x00, 0x00, 0x00 }, { 0x75, 0x34, 0xBE, 0x58, 0x02 } },
	{ { 0xE9, 0x09, 0x00, 0x00, 0x00 }, { 0x7D, 0x0C, 0x8A, 0x4D, 0xFF } },
	{ { 0x90, 0x90 }, { 0x7C, 0xE2 } },
	{ { 0xE9, 0x09, 0x00, 0x00, 0x00 }, { 0x7D, 0x0C, 0xE8, 0x62, 0xE5 } }
};
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
static int Put_Patches(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{

	int opt, i;

	union {
		unsigned long	lv;
		float			fv;
	} u_PatchData;

	// X wall color
	u_PatchData.fv = (float) GetRValue(BIOSOptions->XWallColor) / 255;

	i = XWallOffset[0];
	kernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
	kernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

	u_PatchData.fv = (float) GetGValue(BIOSOptions->XWallColor) / 255;

	i = XWallOffset[1];
	kernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
	kernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

	u_PatchData.fv = (float) GetBValue(BIOSOptions->XWallColor) / 255;

	i = XWallOffset[2];
	kernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
	kernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

	// X glow color
	u_PatchData.fv = (float) GetRValue(BIOSOptions->XGlowColor) / 255;

	i = XGlowOffset[0];
	kernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
	kernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

	u_PatchData.fv = (float) GetGValue(BIOSOptions->XGlowColor) / 255;

	i = XGlowOffset[1];
	kernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
	kernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;

	u_PatchData.fv = (float) GetBValue(BIOSOptions->XGlowColor) / 255;

	i = XGlowOffset[2];
	kernelBuffer[i++] = (char) u_PatchData.lv & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 8) & 0x000000FF;
	kernelBuffer[i++] = (char) (u_PatchData.lv >> 16) & 0x000000FF;
	kernelBuffer[i] = (char) (u_PatchData.lv >> 24) & 0x000000FF;
	
	// X lip corner highlights
	i = XLipHighlightsOffset;
	kernelBuffer[i++] = GetBValue(BIOSOptions->XLipHighlightsColor);
	kernelBuffer[i++] = GetGValue(BIOSOptions->XLipHighlightsColor);
	kernelBuffer[i] = GetRValue(BIOSOptions->XLipHighlightsColor);	

	// X lip color
	i = XLipOffset;
	kernelBuffer[i++] = GetBValue(BIOSOptions->XLipColor);
	kernelBuffer[i++] = GetGValue(BIOSOptions->XLipColor);
	kernelBuffer[i] = GetRValue(BIOSOptions->XLipColor);

	// "XBOX" color
	i = XBOXTextOffset;
	kernelBuffer[i++] = GetBValue(BIOSOptions->XboxColor);
	kernelBuffer[i++] = GetGValue(BIOSOptions->XboxColor);
	kernelBuffer[i] = GetRValue(BIOSOptions->XboxColor);

	// Animation
	opt = BIOSOptions->Animation;
	memcpy(kernelBuffer + Anim.offset[0], Anim.data1[opt], sizeof(Anim.data1[0]));
	memcpy(kernelBuffer + Anim.offset[1], Anim.data2[opt], sizeof(Anim.data2[0]));
	memcpy(kernelBuffer + Anim.offset[2], Anim.data3[opt], sizeof(Anim.data3[0]));
	memcpy(kernelBuffer + Anim.offset[3], Anim.data4[opt], sizeof(Anim.data4[0]));

	// X logo
	opt = BIOSOptions->XLogo;
	memcpy(kernelBuffer + XLogo.offset[0], XLogo.data1[opt], sizeof(XLogo.data1[0]));
	memcpy(kernelBuffer + XLogo.offset[1], XLogo.data2[opt], sizeof(XLogo.data2[0]));

	// splash logo
	opt = BIOSOptions->SplashLogo;
	memcpy(kernelBuffer + SplashLogo.offset, SplashLogo.data[opt], sizeof(SplashLogo.data[0]));

	// TMs
	opt = BIOSOptions->TrademarkSymbol;
	kernelBuffer[TMark.offset] = TMark.data[opt];

	// dash options
	for (i = 0; i < kBootNum; i++)
	{
		strcpy((char*) (kernelBuffer + *((unsigned long *) (kernelBuffer + Dash.pOffset[i]))) - kAddressAdjust, partitonStr);
		strncpy((char*) (kernelBuffer + (*((unsigned long *) (kernelBuffer + Dash.pOffset[i]))) - kAddressAdjust + 29), BIOSOptions->DashName[i].GetString(), Dash.len[i]);
		*((kernelBuffer + 27 + *((unsigned long *) (kernelBuffer + Dash.pOffset[i]))) - kAddressAdjust) = BIOSOptions->BootPart[i];
	}
	// default DVD options
	strncpy((char*) (kernelBuffer + (*((unsigned long *) (kernelBuffer + Dash.pOffset[i]))) - kAddressAdjust + 15), BIOSOptions->DefaultXbeName.GetString(), Dash.len[i]);

	// DVD check
	opt = BIOSOptions->DVDCheck;
	memcpy(kernelBuffer + DVD.offset[0], DVD.data1[opt], sizeof(DVD.data1[0]));
	memcpy(kernelBuffer + DVD.offset[1], DVD.data2[opt], sizeof(DVD.data2[0]));
	memcpy(kernelBuffer + DVD.offset[2], DVD.data3[opt], sizeof(DVD.data3[0]));
	memcpy(kernelBuffer + DVD.offset[3], DVD.data4[opt], sizeof(DVD.data4[0]));

	// LED color
	if (BIOSOptions->LEDColor == 0x0F)
	{
		memcpy(kernelBuffer + LED.offset[0], LED.data1[1], sizeof(LED.data1[0]));
		kernelBuffer[LED.offset[1]] = LED.data2;
		memcpy(kernelBuffer + LED.offset[2], LED.data3[1], sizeof(LED.data3[0]));
	}
	else
	{
		memcpy(kernelBuffer + LED.offset[0], LED.data1[0], sizeof(LED.data1[0]));
		kernelBuffer[LED.offset[1]] = BIOSOptions->LEDColor;
		memcpy(kernelBuffer + LED.offset[2], LED.data3[0], sizeof(LED.data3[0]));
	}

	float scale = (float ) strtod(BIOSOptions->XScale, '\0');
	float *scalePtr = (float *) (kernelBuffer + 0x2984);
	*scalePtr++ *= (scale / 100);
	*scalePtr *= (scale / 100);

	patchKernelBootFlags(kernelBuffer, &BIOSOptions->BootFlags, 1);

	return 1;
}

static int Get_Patches(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions)
{
	BYTE redByte, greenByte, blueByte;
	int i;
	union {
		unsigned long	lv;
		float			fv;
	} u_PatchData;

	// X wall color
	i = XWallOffset[0];
	u_PatchData.lv = (unsigned char) kernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i]) << 24;

	redByte = (BYTE) (255 * u_PatchData.fv);

	i = XWallOffset[1];
	u_PatchData.lv = (unsigned char) kernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i]) << 24;

	greenByte = (BYTE) (255 * u_PatchData.fv);

	i = XWallOffset[2];
	u_PatchData.lv = (unsigned char) kernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 24;

	blueByte = (BYTE) (255 * u_PatchData.fv);

	BIOSOptions->XWallColor = RGB(redByte, greenByte, blueByte);

	// X glow color
	i = XGlowOffset[0];
	u_PatchData.lv = (unsigned char) kernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i]) << 24;

	redByte = (BYTE) (255 * u_PatchData.fv);

	i = XGlowOffset[1];
	u_PatchData.lv = (unsigned char) kernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i]) << 24;

	greenByte = (BYTE) (255 * u_PatchData.fv);

	i = XGlowOffset[2];
	u_PatchData.lv = (unsigned char) kernelBuffer[i++];
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 8;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i++]) << 16;
	u_PatchData.lv |= ((unsigned char) kernelBuffer[i]) << 24;

	blueByte = (BYTE) (255 * u_PatchData.fv);

	BIOSOptions->XGlowColor = RGB(redByte, greenByte, blueByte);

	// X lip highlight color
	i = XLipHighlightsOffset;
	blueByte = kernelBuffer[i++];
	greenByte = kernelBuffer[i++];
	redByte = kernelBuffer[i];
	BIOSOptions->XLipHighlightsColor = RGB(redByte, greenByte, blueByte);


	// X lip color
	i = XLipOffset;
	blueByte = kernelBuffer[i++];
	greenByte = kernelBuffer[i++];
	redByte = kernelBuffer[i];
	BIOSOptions->XLipColor = RGB(redByte, greenByte, blueByte);

	// "XBOX" color
	i = XBOXTextOffset;
	blueByte = kernelBuffer[i++];
	greenByte = kernelBuffer[i++];
	redByte = kernelBuffer[i];
	BIOSOptions->XboxColor = RGB(redByte, greenByte, blueByte);


	// Animation
	if(Anim.data1[0][0] != kernelBuffer[*Anim.offset])
		BIOSOptions->Animation = 1;
	else
		BIOSOptions->Animation = 0;

	// X logo
	if(XLogo.data1[0][0] != kernelBuffer[*XLogo.offset])
		BIOSOptions->XLogo = 1;
	else
		BIOSOptions->XLogo = 0;

	// splash logo
	if(SplashLogo.data[0][0] != kernelBuffer[SplashLogo.offset])
		BIOSOptions->SplashLogo = 1;
	else
		BIOSOptions->SplashLogo = 0;

	// TM
	if(TMark.data[0] != kernelBuffer[TMark.offset])
		BIOSOptions->TrademarkSymbol = 1;
	else
		BIOSOptions->TrademarkSymbol = 0;

	// dash options
	for (i = 0; i < kBootNum; i++)
	{
		BIOSOptions->BootPart[i] = *(kernelBuffer + 27 + *((unsigned long *) (kernelBuffer + Dash.pOffset[i]))- kAddressAdjust);
		BIOSOptions->DashName[i] = (char *) (kernelBuffer + 29 + *((unsigned long *) (kernelBuffer + Dash.pOffset[i]))- kAddressAdjust);
		BIOSOptions->DashNameLen[i] = Dash.len[i];
	}
	// default xbe for DVD
	BIOSOptions->DefaultXbeName = (char *) (kernelBuffer + 15 + *((unsigned long *) (kernelBuffer + Dash.pOffset[i]))- kAddressAdjust);

	// DVD check
	if(DVD.data1[0][0] != kernelBuffer[*DVD.offset])
		BIOSOptions->DVDCheck = 1;
	else
		BIOSOptions->DVDCheck = 0;

	// LED color
	if (*LED.data1[0] != kernelBuffer[*LED.offset])
		BIOSOptions->LEDColor = 0x0F;
	else
		BIOSOptions->LEDColor = kernelBuffer[LED.offset[1]];

	patchKernelBootFlags(kernelBuffer, &BIOSOptions->BootFlags, 0);

	return 1;
}

static void Get_Supported_Patches(BIOS_Supported_Options *SupOpts)
{
	SupOpts->Animation				= 1;
	SupOpts->BootPart				= 1;
	SupOpts->DashName				= 1;
	SupOpts->DefaultXbeName			= 1;
	SupOpts->DVDCheck				= 1;
	SupOpts->LEDColor				= 1;
	SupOpts->SplashLogo				= 1;
	SupOpts->TrademarkSymbol		= 1;
	SupOpts->XboxColor				= 1;
	SupOpts->XGlowColor				= 1;
	SupOpts->XLipColor				= 1;
	SupOpts->XLipHighlightsColor	= 1;
	SupOpts->XLogo					= 1;
	SupOpts->XScale					= 1;
	SupOpts->XWallColor				= 1;
	SupOpts->BootFlags				= 1;
}

int evox_D6_Patch(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions, bool mode)
{
	if (mode)
		return Put_Patches(kernelBuffer, BIOSOptions);

	Get_Supported_Patches(&BIOSOptions->SupportedOpts);
	return Get_Patches(kernelBuffer, BIOSOptions);
}
