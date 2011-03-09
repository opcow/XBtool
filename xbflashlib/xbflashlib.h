// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XBFLASHLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XBFLASHLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XBFLASHLIB_EXPORTS
#define XBFLASHLIB_API __declspec(dllexport)
#else
#define XBFLASHLIB_API __declspec(dllimport)
#endif

// This class is exported from the xbflashlib.dll
class XBFLASHLIB_API Cxbflashlib {
public:
	Cxbflashlib(void);
	// TODO: add your methods here.
};

////////////////////////////////////////////

extern FILE *logFile;

XBFLASHLIB_API int
open_log(const char *logFileName, const char *openText);

XBFLASHLIB_API void
close_log();

XBFLASHLIB_API int
read_xbconfig_file(const char *cfgfname);

XBFLASHLIB_API
int do_xbpack(const char *indirname, const char *romfname, unsigned int romsize);

XBFLASHLIB_API int
do_xbunpack(const char *romfname, const char *outdirname);

XBFLASHLIB_API void
freeKernelBuffer(unsigned char *kernelBuffer);

XBFLASHLIB_API unsigned long
xbflashGetVersion();

