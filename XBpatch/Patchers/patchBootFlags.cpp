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
//patchBootFlags.cpp

#include "stdafx.h"
#include <Windows.h>

#ifdef XBPATCH_EXPORTS
#define XBPATCH_API __declspec(dllexport)
#else
#define XBPATCH_API __declspec(dllimport)
#endif

PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader);
LPVOID GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase );

XBPATCH_API
int patchKernelBootFlags(unsigned char *kernelBuffer, unsigned char *flag, int rwFlag)
{
	PIMAGE_NT_HEADERS pNTHeaders;
	PIMAGE_EXPORT_DIRECTORY pExportDir;
	DWORD exportsStartRVA, exportsEndRVA;
	unsigned long *expTable, bfmPatch;

	pNTHeaders = (PIMAGE_NT_HEADERS) (kernelBuffer + *(unsigned long *) (kernelBuffer + 0x3c));
    
    exportsStartRVA = pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    exportsEndRVA = pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		
    pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetPtrFromRVA(exportsStartRVA, pNTHeaders, kernelBuffer);
	expTable = (unsigned long *) (kernelBuffer + pExportDir->AddressOfFunctions);
	bfmPatch = expTable[320];

	if (rwFlag)
		*((kernelBuffer + bfmPatch) - 8) = *flag;
	else
		*flag = *((kernelBuffer + bfmPatch) - 8);

	return 1;
}

PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader)	// 'T' == PIMAGE_NT_HEADERS 
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

LPVOID GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase ) // 'T' = PIMAGE_NT_HEADERS 
{
	PIMAGE_SECTION_HEADER pSectionHdr;
	INT delta;
		
	pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
	if ( !pSectionHdr )
		return 0;

	delta = (INT)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
	return (PVOID) ( imageBase + rva - delta );
}