//////////////////////////////////////////////////////////////////////////////
// FileSel.cpp
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
// file selection functions

#include "stdafx.h"
#include "filesel.h"

int getFileSel(CString *biosFPName, CString *biosDirName, CString *biosFName, 
			   char *title, char *filter, HWND winHnd)
{

	OPENFILENAME	ofn;  // common dialog box structure

	char ftitle[500]={"\0"};
	char nname[1024]={"\0"};
	char dtitle[500]={"\0"};
	char *infilename;

	WIN32_FIND_DATA fileInfo;

	char fdrive[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME], fext[_MAX_EXT];

	// Initialize ofn

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = winHnd;
	ofn.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	ofn.lpstrFile = nname;
	ofn.nMaxFile = sizeof(nname);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = ftitle;
	ofn.lpstrTitle = title;
	ofn.nMaxFileTitle = sizeof(ftitle);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST |
		OFN_FILEMUSTEXIST |
		OFN_EXPLORER |
		OFN_HIDEREADONLY;

	if(GetOpenFileName(&ofn))
	{
		FindFirstFile(ofn.lpstrFile, &fileInfo);
		infilename = (char*)malloc(strlen(fileInfo.cAlternateFileName)+1);
		strcpy(infilename,fileInfo.cAlternateFileName);

		_splitpath(ofn.lpstrFile, fdrive, fdir, fname, fext);

		*biosDirName = fdrive;
		*biosDirName += fdir;
		*biosFPName = ofn.lpstrFile;
		*biosFName = fname;
		*biosFName += fext;

		return 1; //sucess

	}

	return 0; //failed
}

int putFileSel(CString *biosFPName, CString *biosDirName, CString *biosFName, 
			   char *title, char *filter, HWND winHnd)
{

	OPENFILENAME	ofn;  // common dialog box structure

	char ftitle[500]={"\0"};
	char nname[1024]={"\0"};
	char dtitle[500]={"\0"};
	char *infilename;

	WIN32_FIND_DATA fileInfo;

	char fdrive[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME], fext[_MAX_EXT];

	// Initialize ofn

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = winHnd;
	ofn.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	ofn.lpstrFile = nname;
	ofn.nMaxFile = sizeof(nname);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = ftitle;
	ofn.lpstrTitle = title;
	ofn.nMaxFileTitle = sizeof(ftitle);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST |
		OFN_FILEMUSTEXIST |
		OFN_EXPLORER |
		OFN_HIDEREADONLY;

	if(GetSaveFileName(&ofn))
	{
		FindFirstFile(ofn.lpstrFile, &fileInfo);
		infilename = (char*)malloc(strlen(fileInfo.cAlternateFileName)+1);
		strcpy(infilename,fileInfo.cAlternateFileName);

		_splitpath(ofn.lpstrFile, fdrive, fdir, fname, fext);

		*biosDirName = fdrive;
		*biosDirName += fdir;
		*biosFPName = ofn.lpstrFile;
		*biosFName = fname;

		if (strcmp(fext, ".bin"))
			*biosFPName += ".bin";
	
		return 1; //sucess

	}

	return 0; //failed
}

int putPreFileSel(CString *initialDir, CString *preFPName, CString *presetName,
				  char *title, char *filter, HWND winHnd)
{

	OPENFILENAME	ofn;  // common dialog box structure

	char ftitle[500]={"\0"};
	char nname[1024]={"\0"};
	char dtitle[500]={"\0"};
	char *infilename;

	WIN32_FIND_DATA fileInfo;

	char fdrive[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME], fext[_MAX_EXT];

	// Initialize ofn

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = winHnd;
	ofn.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	ofn.lpstrFile = nname;
	ofn.nMaxFile = sizeof(nname);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = ftitle;
	ofn.lpstrTitle = title;
	ofn.nMaxFileTitle = sizeof(ftitle);
	ofn.lpstrInitialDir = *initialDir;
	ofn.Flags = OFN_PATHMUSTEXIST |
		OFN_FILEMUSTEXIST |
		OFN_EXPLORER |
		OFN_HIDEREADONLY;

	if(GetSaveFileName(&ofn))
	{
		FindFirstFile(ofn.lpstrFile, &fileInfo);
		infilename = (char*)malloc(strlen(fileInfo.cAlternateFileName)+1);
		strcpy(infilename,fileInfo.cAlternateFileName);

		_splitpath(ofn.lpstrFile, fdrive, fdir, fname, fext);

		*preFPName = ofn.lpstrFile;
		*presetName = fname;

		if (!strcmp(fext, ""))
			*preFPName += ".xbt";
	
		return 1; //sucess

	}

	return 0; //failed
}