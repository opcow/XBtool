#include "CX24976Patcher.h"


class CX24981Patcher
	: public CX24976Patcher {

public:
	virtual XBPATCH_API void GetSupportedOptions(BIOS_Supported_Options &SupOpts);
	virtual XBPATCH_API int Read(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions);
	virtual XBPATCH_API int Write(unsigned char *kernelBuffer, BIOS_Options *BIOSOptions);
	XBPATCH_API CX24981Patcher() {};

	void InitPatches();
	int WritePreIDPatches(BIOS_Options *BIOSOptions);
	int NoFloatReadColors(COLORREF &XLipHighlightsColor, COLORREF &XLipColor, COLORREF &XWallColor, COLORREF &XGlowColor, COLORREF &XboxColor);
	int NoFloatWriteColors(COLORREF XLipHighlightsColor, COLORREF XLipColor, COLORREF XWallColor, COLORREF XGlowColor, COLORREF XboxColor);
};
