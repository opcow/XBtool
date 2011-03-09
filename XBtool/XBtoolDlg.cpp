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

#include "stdafx.h"
#include "afxctl.h"
#include "XBtool.h"
#include ".\xbtooldlg.h"
#include "FileSel.h"
#include "..\XBFlashLib\XBFlashLib.h"
#include "..\XBpatch\XBpatch.h"
#include "..\XBpatch\CXBoxKernel.h"
#include "..\XBpatch\BIOSOptions.h"
#include "Presets.h"
#include <atlstr.h>
#include <io.h>
#include <direct.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CALLBACK SetSelProc (HWND hWnd,
						 UINT uMsg,
						 LPARAM lParam,
						 LPARAM lpData);

// ************************************* globals ****************************************

enum {
	errDashNameTooLong = 1,
	errDashLengthInvalid,
	errFanSpeedInvalid,
	errBFMSetupRename = 15,
	errBFMSetupCopy,
	errKeyLengthInvalid
};

unsigned char *kernelBuffer = NULL;
unsigned int kernelSize;
char partitions[] = { '2', '1', '6', '7' };

int theTypeSelection;
char biosID[33];
char *KernelMD5;

CButton *Size1Radio, *Size2Radio, *Size4Radio;
CButton	*CLEDRedRadio, *CLEDGreenRadio, *CLEDOrangeRadio;
CButton *CLEDCycleRadio, *CLEDOffRadio;
CButton	*RedChecks[4];
CButton *GrnChecks[4];
CButton	*AnimCheck, *XLogoCheck, *SplashLogoCheck, *TMCheck;
CButton	*DVDCheck, *ResetCheck, *EepromCheck, *KeyCheck;
CButton	*BFMCheck, *BFMFlagCheck;
CButton	*LBA48Check, *IgnoreHDTableCheck;

CSpinButtonCtrl	*SpinBoot[3];
CStatic			*StaticBoot[3], *SaticVer;
CEdit			*EditBoot[3], *EditDefXbe;
CStringEdit		*FanSpeedEdit, *HDKeyEdit;

CComboBox *TypeCombo, *PresetCombo, *PartitionOptsCombo;
CMenu* pSysMenu;

CString theAppPath, theAppName, theAppPathName;
CString presetFolderName = "Presets\\";
CString configFolderName = "Config\\";
CString bfmFolderName = "Config\\BFM\\";
CString tempFolderName = "tmp\\";
CString configFilename, logFilename;
CString biosFPName, biosDirName, biosFName;
CString configFiles[10];
CString thePath, profileFile, configPath, presetPath, tempPath, bfmPath;
char *fSelTitle = "Please select the BIOS file";
char *fSelFilter = "BIOS Files(*.bin)\0*.bin\0All Files(*.*)\0*.*\0\0";
char *fSaveTitle = "Save As...";
char *fPreSelFilter = "Preset Files(*.xbt)\0*.xbt\0All Files(*.*)\0*.*\0\0";
char *fPreSaveTitle = "Save Preset As...";
char *fCompileTitle = "Please select the BIOS kernel file";
char *fCompileFilter = "XBOX Kernel(xboxkrnl.img)\0xboxkrnl.img\0All Files(*.*)\0*.*\0\0";

const CString nullName = "Kernel Not Loaded";
CString errMsg;

CTime theTime;

char *bootFlagMsg = "Set kernel boot from media flag?\n\nSetting the flag will prevent the Xbox logo screen from appearing.\n(Choose no if your kernel should be identical to retail.)";

CXboxKernel		*XBKernel = 0;

DWORD	gdwAutoName = 0;
DWORD	gdwRomSize;
// *********************************** end globals **************************************

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

BOOL CAboutDlg::OnInitDialog()
{

	DWORD dwVerInfoSize;
	DWORD dwHnd;
	VS_FIXEDFILEINFO *pFixedInfo;	// pointer to fixed file info structure
	UINT    uVersionLen;			// Current length of full version string

	CString verStr;

	dwVerInfoSize = GetFileVersionInfoSize(theAppPathName, &dwHnd);

	void *pBuffer = new char [dwVerInfoSize];
	if (pBuffer == NULL)
			   return TRUE; 

	GetFileVersionInfo(theAppPathName, dwHnd, dwVerInfoSize, pBuffer);

	VerQueryValue(pBuffer,_T("\\"),(void**)&pFixedInfo,(UINT *)&uVersionLen);

	if(LOWORD (pFixedInfo->dwProductVersionLS))
		verStr.Format ("XBtool %u.%u.%u.b%u",
						HIWORD (pFixedInfo->dwProductVersionMS),
						LOWORD (pFixedInfo->dwProductVersionMS),
						HIWORD (pFixedInfo->dwProductVersionLS),
						LOWORD (pFixedInfo->dwProductVersionLS)); 
	else
		verStr.Format ("XBtool %u.%u.%u",
				HIWORD (pFixedInfo->dwProductVersionMS),
				LOWORD (pFixedInfo->dwProductVersionMS),
				HIWORD (pFixedInfo->dwProductVersionLS)); 


	SaticVer = (CStatic *) GetDlgItem(IDC_STATIC_VER);
	SaticVer->SetWindowText(verStr);

	delete [] pBuffer;

	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CXBtoolDlg dialog



CXBtoolDlg::CXBtoolDlg(CWnd* pParent /*=NULL*/)
: CDialog(CXBtoolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXBtoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_XTEXT_COLOR_BUTTON, mLogoColorButton);
	DDX_Control(pDX, IDC_XLIPHL_COLOR_BUTTON, mXLipHighlightsColorButton);
	DDX_Control(pDX, IDC_XLIP_COLOR_BUTTON, mXLipColorButton);
	DDX_Control(pDX, IDC_XGLOW_COLOR_BUTTON, mXLogoGlowColorButton);
	DDX_Control(pDX, IDC_XINNER_COLOR_BUTTON, mXLogoInnerColorButton);
	DDX_Control(pDX, IDC_EDIT_Speed, m_SpeedEdit);
	DDX_Control(pDX, IDC_EDIT_HDKEY, m_KeyEdit);

	DDX_Control(pDX, IDC_HYPERLINK_HOME, mHomeHyperLink);
	DDX_Control(pDX, IDC_HYPERLINK_HELP, mHelpHyperLink);
}

BEGIN_MESSAGE_MAP(CXBtoolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_QUITB, OnBnClickedQuitb)
	ON_BN_CLICKED(IDC_COMPB, OnBnClickedPackb)
	ON_BN_CLICKED(IDC_EXTB, OnBnClickedUnpackb)
	ON_BN_CLICKED(IDC_OPENB, OnBnClickedOpenb)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_SAVEASB, OnBnClickedSaveasb)
	ON_BN_CLICKED(IDC_CLIPB, OnBnClickedClipb)
	ON_BN_CLICKED(IDC_RADIO_SZ1, OnBnClickedRadioSz1)
	ON_BN_CLICKED(IDC_RADIO_SZ2, OnBnClickedRadioSz2)
	ON_BN_CLICKED(IDC_RADIO_SZ4, OnBnClickedRadioSz4)
	// Color Button Messages
	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
    ON_MESSAGE(CPN_CLOSEUP,      OnCloseUp)
    ON_MESSAGE(CPN_DROPDOWN,     OnDropDown)
	// End Color Button Messages

	ON_BN_CLICKED(IDC_CHECK_XLOGO, OnBnClickedCheckLogo)
	ON_CBN_SELCHANGE(IDC_COMBO_PRE, OnCbnSelchangeComboPre)
	ON_BN_CLICKED(IDC_BUTTON_SAVPRE, OnBnClickedButtonSavpre)
	ON_BN_CLICKED(IDC_BUTTON_DELPRE, OnBnClickedButtonDelPre)
	ON_BN_CLICKED(IDC_RADIO_LEDR, OnBnClickedRadioLedr)
	ON_BN_CLICKED(IDC_RADIO_LEDG, OnBnClickedRadioLedg)
	ON_BN_CLICKED(IDC_RADIO_LEDO, OnBnClickedRadioLedo)
	ON_BN_CLICKED(IDC_RADIO_LEDN, OnBnClickedRadioLedn)
	ON_BN_CLICKED(IDC_RADIO_LEDC, OnBnClickedRadioLedc)
	ON_BN_CLICKED(IDC_BUTTON_SZ, OnBnClickedSizeButton)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BOOT1, OnDeltaposSpinBoot1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BOOT2, OnDeltaposSpinBoot2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BOOT3, OnDeltaposSpinBoot3)
	ON_BN_CLICKED(IDC_CHECK_BFM_F, OnBnClickedCheckBfmF)
	ON_BN_CLICKED(IDC_CHECK_LBA48, OnBnClickedCheckLba48)
	ON_BN_CLICKED(IDC_KEY_CHECK, OnBnClickedKeyCheck)
END_MESSAGE_MAP()


// CXBtoolDlg message handlers

BOOL CXBtoolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		CString strAboutMenu, strAutoNameMenu;
		strAutoNameMenu.LoadString(IDS_AUTONAME);
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_AUTONAME, strAutoNameMenu);
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// start my inits

	if (XBPatchGetVersion() != 0x01050900)
	{
		MessageBox("Wrong XBPatch.dll version.\nXBtool will quit now.", "Error", MB_ICONSTOP);
		OnOK();
	}
	if (xbflashGetVersion() != 0x01010000)
	{
		MessageBox("Wrong xbflashlib.dll version.\nXBtool will quit now.", "Error", MB_ICONSTOP);
		OnOK();
	}

	HKEY ak = theApp.GetAppRegistryKey();
	CRegKey theAppKey;
	DWORD RegType;
	ULONG RegSz = sizeof(gdwAutoName);

	if(!theAppKey.Open(ak, 0, KEY_ALL_ACCESS))
	{
		if (theAppKey.QueryValue("AutoName", &RegType, &gdwAutoName, &RegSz) == ERROR_FILE_NOT_FOUND)
            theAppKey.SetDWORDValue("AutoName", 0);
		if (gdwAutoName > 1 || gdwAutoName < 0)
			gdwAutoName = 0;
		if (theAppKey.QueryValue("DefaultSize", &RegType, &gdwRomSize, &RegSz) == ERROR_FILE_NOT_FOUND)
            theAppKey.SetDWORDValue("DefaultSize", 0x40000);
		theAppKey.Close();
	}
	if (gdwAutoName)
		pSysMenu->CheckMenuItem(IDM_AUTONAME, MF_CHECKED | MF_BYCOMMAND);
	else
		pSysMenu->CheckMenuItem(IDM_AUTONAME, MF_UNCHECKED | MF_BYCOMMAND);

	mLogoColorButton.SetDefaultText("Default");
	mXLipHighlightsColorButton.SetDefaultText("Default");
	mXLipColorButton.SetDefaultText("Default");
	mXLogoInnerColorButton.SetDefaultText("Default");
	mXLogoGlowColorButton.SetDefaultText("Default");

	mLogoColorButton.SetDefaultColor(DefLogoColor);
	mXLipColorButton.SetDefaultColor(DefXLipColor);
	mXLipHighlightsColorButton.SetDefaultColor(DefXLipHighlightsColor);
	mXLogoInnerColorButton.SetDefaultColor(DefXLogoInnerColor);
	mXLogoGlowColorButton.SetDefaultColor(DefXLogoGlowColor);

	mLogoColorButton.SetTrackSelection(true);
	mXLipColorButton.SetTrackSelection(true);
	mXLipHighlightsColorButton.SetTrackSelection(true);
	mXLogoInnerColorButton.SetTrackSelection(true);
	mXLogoGlowColorButton.SetTrackSelection(true);
	
	for (int i = 0; i < 3; i++)
	{
		SpinBoot[i] = (CSpinButtonCtrl *) GetDlgItem(IDC_SPIN_BOOT1 + i);
		SpinBoot[i]->SetRange(0, 3);
		StaticBoot[i] = (CStatic *) GetDlgItem(IDC_STATIC_BOOT1 + i);
		EditBoot[i] = (CEdit *) GetDlgItem(IDC_EDIT_BOOT1 + i);
	}
	EditDefXbe = (CEdit *) GetDlgItem(IDC_EDIT_DEFXBE);
	EditDefXbe->SetLimitText(11);

	FanSpeedEdit = (CStringEdit *) GetDlgItem(IDC_EDIT_Speed);
	FanSpeedEdit->SetMaxLength(3);
	FanSpeedEdit->SetLegal("0123456789");

	HDKeyEdit = (CStringEdit *) GetDlgItem(IDC_EDIT_HDKEY);
	HDKeyEdit->SetMaxLength(40);
	HDKeyEdit->SetLegal("0123456789ABCDEFabcdef");

	Size1Radio = (CButton *) GetDlgItem(IDC_RADIO_SZ1);
	Size2Radio = (CButton *) GetDlgItem(IDC_RADIO_SZ2);
	Size4Radio = (CButton *) GetDlgItem(IDC_RADIO_SZ4);

	CLEDRedRadio = (CButton *) GetDlgItem(IDC_RADIO_LEDR);
	CLEDGreenRadio = (CButton *) GetDlgItem(IDC_RADIO_LEDG);
	CLEDOrangeRadio = (CButton *) GetDlgItem(IDC_RADIO_LEDO);
	CLEDOffRadio = (CButton *) GetDlgItem(IDC_RADIO_LEDN);
	CLEDCycleRadio = (CButton *) GetDlgItem(IDC_RADIO_LEDC);

	AnimCheck = (CButton *) GetDlgItem(IDC_CHECK_ANI);
	XLogoCheck = (CButton *) GetDlgItem(IDC_CHECK_XLOGO);
	SplashLogoCheck = (CButton *) GetDlgItem(IDC_CHECK_SPLASHLOGO);
	TMCheck = (CButton *) GetDlgItem(IDC_CHECK_TM);
	DVDCheck = (CButton *) GetDlgItem(IDC_DVD_CHECK);
	ResetCheck  = (CButton *) GetDlgItem(IDC_RESET_CHECK);
	EepromCheck  = (CButton *) GetDlgItem(IDC_EEPROM_CHECK);
	KeyCheck  = (CButton *) GetDlgItem(IDC_KEY_CHECK);

	for (int i = 0; i < 4; i++)
		RedChecks[i] = (CButton *) GetDlgItem(IDC_CHECK_REDC1 + i);
	for (int i = 0; i < 4; i++)
		GrnChecks[i] = (CButton *) GetDlgItem(IDC_CHECK_GRNC1 + i);

	BFMCheck = (CButton *) GetDlgItem(IDC_CHECK_BFM);
	BFMCheck->EnableWindow(0);
	BFMFlagCheck = (CButton *) GetDlgItem(IDC_CHECK_BFM_F);

	LBA48Check = (CButton *) GetDlgItem(IDC_CHECK_LBA48);
	IgnoreHDTableCheck = (CButton *) GetDlgItem(IDC_CHECK_IgnoreHDTable);
	IgnoreHDTableCheck->EnableWindow(0);

	TypeCombo = (CComboBox *) GetDlgItem(IDC_COMBO_TYPE);
	PresetCombo = (CComboBox *) GetDlgItem(IDC_COMBO_PRE);
	PartitionOptsCombo = (CComboBox *) GetDlgItem(IDC_COMBO_PartitionOpts);
	PartitionOptsCombo->AddString("Use Standard Partition Table");
	PartitionOptsCombo->AddString("Partition 6 Takes Rest of the Drive");
	PartitionOptsCombo->AddString("Partition 6 Takes up to 137GB / Partition 7 Takes Rest");
	PartitionOptsCombo->AddString("Partition 6 Takes up to 137GB / No Partition 7");
	SetComboWidth(PartitionOptsCombo);

	XBKernel = new CXboxKernel();

	getFilePaths();

	if ( (_access(bfmPath + "bfm.ini", 0x00) != -1) &&
		(_access(bfmPath + "bfm_2bl.img", 0x00) != -1))//&&		(_access(bfmPath + "bfm_remainder.img", 0x00) != -1) )
		BFMCheck->EnableWindow(1);

	unlink(logFilename);

	loadPresetNames();
	loadConfigTypeNames();
	TypeCombo->SetCurSel(0);

	theTypeSelection = 0;

	enableDlgItems();
	setOptionsInDialog();

	SetDlgItemText(IDC_STATIC_NAME, nullName);



	///////////////////////////////////////////////
	m_ToolTips.Create(this);
	m_ToolTips.AddTool((CWnd *) &mLogoColorButton, "color of the large text below the X"); 
	m_ToolTips.AddTool((CWnd *) &mXLipColorButton, "color of the lip around the edge of the X"); 
	m_ToolTips.AddTool((CWnd *) &mXLipHighlightsColorButton, "color of the highlights on the lip at the points of the X"); 
	m_ToolTips.AddTool((CWnd *) &mXLogoInnerColorButton, "color of the of the inner walls of the X"); 
	m_ToolTips.AddTool((CWnd *) &mXLogoGlowColorButton, "color of the inner light emanating from inside the X"); 
	m_ToolTips.AddTool(AnimCheck, "uncheck this to not show the flubber animation"); 
	m_ToolTips.AddTool(XLogoCheck, "uncheck this to not show the big \'X\' logo (if off animation will be forced off)"); 
	m_ToolTips.AddTool(SplashLogoCheck, "uncheck this to not show the text logo which fades in as the dash loads"); 
	m_ToolTips.AddTool(TMCheck, "uncheck this to not show the small \'TM\' symbols on the logo screen"); 
	m_ToolTips.AddTool(DVDCheck, "check this to allow booting with no DVD drive attached"); 
	m_ToolTips.AddTool(ResetCheck, "check this to allow the Xbox to reset when the DVD tray ejects"); 
	m_ToolTips.AddTool(EepromCheck, "check this to embed an eeprom image in the kernel (loaded from \"Patch Data\\eeprom.bin\")"); 
	m_ToolTips.AddTool(KeyCheck, "check this to embed your hard drive key in the kernel"); 
	m_ToolTips.AddTool(HDKeyEdit, "enter your hard drive key here (40 characters using digits 0-9 and letters a-f)"); 
	m_ToolTips.AddTool(BFMFlagCheck, "check this to set the bootable from media flag (this will not make the ROM file bootable from media)"); 
	m_ToolTips.AddTool(LBA48Check, "check this to allow full access to hard drives larger than 137GB"); 
	m_ToolTips.AddTool(IgnoreHDTableCheck, "check this to force the partition table to be read from the kernel (normally you'll leave this unchecked)"); 
	m_ToolTips.AddTool((CButton *)PartitionOptsCombo, "choose the partition scheme used when LBA48 is enabled (see documentation for more info)"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_CLIPB), "copy the MD5 sum to the clipboard (used for BIOS identification in evox.ini)"); 
	m_ToolTips.AddTool(Size1Radio, "the ROM file will be saved as a 256k file or its minimum size"); 
	m_ToolTips.AddTool(Size2Radio, "the ROM file will be saved as a 512k file or its minimum size if greater than 512k"); 
	m_ToolTips.AddTool(Size4Radio, "the ROM file will be saved as a 1MB file"); 
	m_ToolTips.AddTool(FanSpeedEdit, "speed of the fan expressed as a percentage of maximum (the default speed is 20%)"); 
	m_ToolTips.AddTool(CLEDRedRadio, "make the power LED red"); 
	m_ToolTips.AddTool(CLEDGreenRadio, "make the power LED green"); 
	m_ToolTips.AddTool(CLEDOrangeRadio, "make the power LED orange (this turns on red and green at the same time)"); 
	m_ToolTips.AddTool(CLEDOffRadio, "the power LED will be off"); 
	m_ToolTips.AddTool(CLEDCycleRadio, "cycle LED colors as per the selected stages"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_BUTTON_SAVPRE), "save the color settings as a preset"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_BUTTON_DELPRE), "delete the current preset"); 
	m_ToolTips.AddTool(BFMCheck, "make the ROM file bootable from media (see documentation for the setup required for this to work)"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_SAVEASB), "save the ROM file"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_OPENB), "load a ROM file into XBtool"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_EXTB), "unpack a ROM file (decompress the ROM file and break it into its constituent parts)");
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_COMPB), "repack a previously unpacked ROM file"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_QUITB), "quit XBtool"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_BUTTON_SZ), "resize a ROM file (you don't need to load the file first and no minimun size check is done)"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_COMBO_TYPE), "Type of ROM file (uses selected configuration file from the \"Config\" folder when loading\\unpacking)"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_EDIT_DEFXBE), "enter the name of the executable to auto-run when a DVD\\CD is loaded"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_EDIT_BOOT1), "enter the path and name of the 1st dashboard the Xbox should to try to load (backslashes are allowed)"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_EDIT_BOOT2), "enter the path and name of the dashboard the Xbox should to try to load if the 1st attempt fails (backslashes are allowed)"); 
	m_ToolTips.AddTool((CButton *)GetDlgItem(IDC_EDIT_BOOT3), "enter the path and name of the dashboard the Xbox should to try to load if the 2nd attempt fails (backslashes are allowed)"); 

	///////////////////////////////////////////////
	return TRUE;  // return TRUE  unless you set the focus to a control

}

BOOL CXBtoolDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN: 
	case WM_LBUTTONUP: 
	case WM_MOUSEMOVE:

		m_ToolTips.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CXBtoolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == IDM_AUTONAME)
	{
		gdwAutoName = !gdwAutoName;
		if (gdwAutoName)
			pSysMenu->CheckMenuItem(IDM_AUTONAME, MF_CHECKED | MF_BYCOMMAND);
		else
			pSysMenu->CheckMenuItem(IDM_AUTONAME, MF_UNCHECKED | MF_BYCOMMAND);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXBtoolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXBtoolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CXBtoolDlg::OnBnClickedQuitb(){

	OnOK();
}

void CXBtoolDlg::OnBnClickedPackb()
{
	int prevPos = 0, curPos = 0;
	int bfmRepackFlag, err = 0;
	CString token, parentPath, parentDir;
	CXboxKernel *XBKernel_pack;

	getOptionsFromDialog();
	CString saveFPName, saveDirName, saveFName;

	if (getFileSel(&biosFPName, &biosDirName, &biosFName, fCompileTitle, fCompileFilter, this->m_hWnd))
	{
		if (!gdwAutoName)
		{
			if (!putFileSel(&saveFPName, &saveDirName, &saveFName, fSaveTitle, fSelFilter, this->m_hWnd))
				return;
		}
		else
		{
			saveFPName = biosDirName;
			saveFPName.TrimRight('\\');
			if (saveFPName.Find(':') == (saveFPName.GetLength() - 1))
				saveFPName += "\\xboxrom.bin";
			else
				saveFPName += ".bin";
		}
		getConfigFilename(&configFilename);

		theTime = CTime::GetCurrentTime();
		open_log(logFilename, theTime.Format( " == %c == Pack ==\n" ));

		bfmRepackFlag = BFMCheck->GetCheck();
		if (bfmRepackFlag)
		{
			if (err = bfmSetup(biosDirName))		// copy bfm files
			{
				DisplayErrorMsg(err);
				return;
			}
			XBKernel_pack = new CXboxKernel(biosFPName);

			if (AfxMessageBox(bootFlagMsg, MB_YESNO | MB_ICONQUESTION, 0) == IDYES)
				XBKernel_pack->mOpts.BootFlags = 0x03;
			else
				XBKernel_pack->mOpts.BootFlags = 0x00;

			if (!read_xbconfig_file(configPath + "BFM\\bfm.ini"))
			{
				MessageBox("Error reading BFM config file.", "Error", MB_ICONSTOP);
				delete XBKernel_pack;
				bfmRestore(biosDirName);		// restore normal 2bl
				close_log();
				return;
			}

			XBKernel_pack->WriteBootFlags();
			delete XBKernel_pack;
		}
		else
		{
			if (!read_xbconfig_file(configFilename))
			{
				MessageBox("Error reading config file.", "Error", MB_ICONSTOP);
				close_log();
				return;
			}
		}
		if (!do_xbpack(biosDirName, saveFPName, gdwRomSize))
		{
			MessageBox("BIOS packing failed. See logfile for more information.", "Error", MB_ICONSTOP);
		}
		else
			SetDlgItemText(IDC_STATIC_INFO_BOX, "Packed " + biosFName);

		if (bfmRepackFlag)
			bfmRestore(biosDirName);		// restore normal 2bl
		close_log();
	}
}

void CXBtoolDlg::OnBnClickedUnpackb()
{
	CString biosFPName, biosDirName, biosFName;

	if (getFileSel(&biosFPName, &biosDirName, &biosFName, fSelTitle, fSelFilter, this->m_hWnd))
	{
		biosDirName += biosFName;
		biosDirName.MakeLower();
		biosDirName.TrimRight(".bin");
		getConfigFilename(&configFilename);

		theTime = CTime::GetCurrentTime();
		open_log(logFilename, theTime.Format( " == %c == Unpack ==\n" ));

		if (!read_xbconfig_file(configFilename))
		{
			MessageBox ( "Open config failed.", "Error", MB_ICONSTOP);
		}
		else
		{
		if (!do_xbunpack(biosFPName, biosDirName))
			MessageBox("BIOS unpacking failed. See logfile for more information.", "Error", MB_ICONSTOP);
		else
			SetDlgItemText(IDC_STATIC_INFO_BOX, "Unpacked " + biosFName);
		close_log();
		}
	}
}

// ------------------------------------------------------------------------

int CALLBACK SetSelProc (HWND hWnd,
						 UINT uMsg,
						 LPARAM lParam,
						 LPARAM lpData)
{
	if (uMsg==BFFM_INITIALIZED)
	{
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData );
	}
	return 0;
}

void CXBtoolDlg::OnBnClickedOpenb()
{
	GetDlgItem(IDC_OPENB)->EnableWindow(false);
	if (kernelBuffer)
	{
		freeKernelBuffer(kernelBuffer);
		kernelBuffer = NULL;
		GetDlgItem(IDC_SAVEASB)->EnableWindow(false);
	}
	if (getFileSel(&biosFPName, &biosDirName, &biosFName, fSelTitle, fSelFilter, this->m_hWnd))
	{
		GetDlgItem(IDC_SAVEASB)->EnableWindow(false);
		getConfigFilename(&configFilename);
		theTime = CTime::GetCurrentTime();
		open_log(logFilename, theTime.Format( " == %c == Open ==\n" ));
		if(read_xbconfig_file(configFilename))
		{
			if (!do_xbunpack(biosFPName, tempPath))
			{
				SetDlgItemText(IDC_STATIC_INFO_BOX, "Kernel not loaded.");
				kernelBuffer = NULL;
				if(XBKernel)
					delete XBKernel;
				XBKernel = new CXboxKernel();
				enableDlgItems();
				setOptionsInDialog();
				SetDlgItemText(IDC_STATIC_INFO, "Extraction Failed");
				SetDlgItemText(IDC_STATIC_NAME, "No Kernel");
				MessageBox("Kernel extraction failed. See logfile for more information.", "Error", MB_ICONSTOP);
			}
			else
			{
				if(XBKernel)
					delete XBKernel;
				XBKernel = new CXboxKernel(tempPath + "xboxkrnl.img");
				PresetCombo->SetCurSel(0);
				if(XBKernel->IsSupported())
					GetDlgItem(IDC_SAVEASB)->EnableWindow(true);
				else
					GetDlgItem(IDC_SAVEASB)->EnableWindow(false);
				SetDlgItemText(IDC_STATIC_INFO, "Loaded " + biosFName);
				enableDlgItems();
				setOptionsInDialog();
			}
		}
		else
			MessageBox("Error reading config file.", "Error", MB_ICONSTOP);

		if(getIDString(biosFPName, biosID))
			SetDlgItemText(IDC_STATIC_BMD5, biosID);
		close_log();
	}
	GetDlgItem(IDC_OPENB)->EnableWindow(true);
}

CXBtoolDlg::~CXBtoolDlg(void)
{
	if (kernelBuffer)
		freeKernelBuffer(kernelBuffer);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(tempPath + "*.*");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory())
			continue;
		unlink(finder.GetFilePath());
	}
	finder.Close();
	rmdir(tempPath);

	if(XBKernel)
		delete XBKernel;

	HKEY ak = theApp.GetAppRegistryKey();
	CRegKey theAppKey;
	ULONG RegSz = sizeof(gdwAutoName);

	if(!theAppKey.Open(ak, 0, KEY_ALL_ACCESS))
	{
		theAppKey.SetDWORDValue("AutoName", gdwAutoName);
		theAppKey.SetDWORDValue("DefaultSize", gdwRomSize);
		theAppKey.Close();
	}

	CDialog::~CDialog();
}

int CXBtoolDlg::getOptionsFromDialog()
{
	int err = 0;

	XBKernel->mOpts.XboxColor = mLogoColorButton.GetColor();
	XBKernel->mOpts.XLipColor = mXLipColorButton.GetColor();
	XBKernel->mOpts.XLipHighlightsColor = mXLipHighlightsColorButton.GetColor();
	XBKernel->mOpts.XWallColor = mXLogoInnerColorButton.GetColor();
	XBKernel->mOpts.XGlowColor = mXLogoGlowColorButton.GetColor();

	XBKernel->mOpts.LEDColor = 0; // init it
	for (int i = 0; i < 4; i++)
	{
		if (RedChecks[i]->GetCheck())
			XBKernel->mOpts.LEDColor |= 0x80 >> i;
	}
	for (int i = 0; i < 4; i++)
	{
		if (GrnChecks[i]->GetCheck())
			XBKernel->mOpts.LEDColor |= 0x08 >> i;
	}

	XBKernel->mOpts.BootPart[0] = partitions[SpinBoot[0]->GetPos() & 0x000000FF];
	XBKernel->mOpts.BootPart[1] = partitions[SpinBoot[1]->GetPos() & 0x000000FF];	
	XBKernel->mOpts.BootPart[2] = partitions[SpinBoot[2]->GetPos() & 0x000000FF];

	CString resToken;
	int curPos;

	if (XBKernel->mSupportedOpts.DashName)
	{
		for (int i = 0; i < 3; i++)
		{
			EditBoot[i]->GetWindowText(XBKernel->mOpts.DashName[i]);
			curPos = 0;
			do
			{
				resToken = XBKernel->mOpts.DashName[i].Tokenize("\\", curPos);
				if (resToken.GetLength() > 42)
				{
					err = errDashNameTooLong;
					errMsg.Format("File not saved.\nFile or folder name '%s' in dash option too long.\nPlease limit names to 42 characters or less.", resToken, i + 1);
				}
			}
			while (resToken != "");
		}
		if (!XBKernel->mOpts.DashName[0].GetLength())
		{
			err = errDashLengthInvalid;
			errMsg = "Dash option one must have a valid dash name.\nFile not saved.";
		}
	}

	EditDefXbe->GetWindowText(XBKernel->mOpts.DefaultXbeName);

	XBKernel->mOpts.Animation = AnimCheck->GetCheck();
	XBKernel->mOpts.XLogo = XLogoCheck->GetCheck();
	XBKernel->mOpts.SplashLogo = SplashLogoCheck->GetCheck();
	XBKernel->mOpts.TrademarkSymbol = TMCheck->GetCheck();
	XBKernel->mOpts.DVDCheck = DVDCheck->GetCheck();
	XBKernel->mOpts.ResetOnEject = ResetCheck->GetCheck();
	XBKernel->mOpts.EmbedEeprom = EepromCheck->GetCheck();

	char keyText[41], keyByte[3], *end;

	if (KeyCheck->GetCheck())
	{
		XBKernel->mOpts.EmbedHDKey = 1;
		if (HDKeyEdit->LineLength(0) == 40)
		{
			HDKeyEdit->GetLine(0, keyText, 40);
			keyByte[2] = 0;
			int i;
			for (i = 0; i < 20; i++)
			{
				strncpy(keyByte, &keyText[i * 2], 2);
				XBKernel->mOpts.HDKey[i] = (unsigned char) strtoul(keyByte, &end, 16);
			}
			for (; i < 32; i++)
				XBKernel->mOpts.HDKey[i] = 0;
		}
		else
		{
			err = errKeyLengthInvalid;
			errMsg = "The HD Key must be exactly 40 characters.\nFile not saved.";
		}
	}
	else
		XBKernel->mOpts.EmbedHDKey = 0;

	if(BFMFlagCheck->GetCheck())
		XBKernel->mOpts.BootFlags = 0x03;
	else
		XBKernel->mOpts.BootFlags = 0x00;

	XBKernel->mOpts.LBA48 = LBA48Check->GetCheck();
	XBKernel->mOpts.PartitionOpt = PartitionOptsCombo->GetCurSel();
	XBKernel->mOpts.IgnoreHDTable = IgnoreHDTableCheck->GetCheck();

	char speed[5];
	FanSpeedEdit->GetLine(0, speed, 3);
	XBKernel->mOpts.FanSpeed = (int) strtoul(speed, &end, 10);

	if (XBKernel->mOpts.FanSpeed < 20 || XBKernel->mOpts.FanSpeed > 100)
	{
		err = errFanSpeedInvalid;
		errMsg.Format("Fan speed %d%% is out of range. Valid range is 20%% - 100%%.", XBKernel->mOpts.FanSpeed);
	}
    
	return err;
}

void CXBtoolDlg::OnCbnSelchangeComboType()
{
		theTypeSelection = TypeCombo->GetCurSel();
}
void CXBtoolDlg::OnBnClickedSaveasb()
{
	CString saveFPName, saveDirName, saveFName;
	CString oldBiosDirName; // for saving the current bios files location
	int bfmRepackFlag, err = 0;

	if (err = getOptionsFromDialog())
	{
		MessageBox(errMsg, "Error", MB_ICONSTOP);
		return;
	}

	getConfigFilename(&configFilename);

	theTime = CTime::GetCurrentTime();
	open_log(logFilename, theTime.Format( " == %c == Save ==\n" ));

	bfmRepackFlag = BFMCheck->GetCheck();
	if (bfmRepackFlag)
	{
		if(err = bfmSetup(tempPath))		// copy bfm files
		{
			DisplayErrorMsg(err);
			close_log();
			return;
		}
		if (AfxMessageBox(bootFlagMsg, MB_YESNO | MB_ICONQUESTION, 0) == IDYES)
			XBKernel->mOpts.BootFlags = 0x03;
		else
			XBKernel->mOpts.BootFlags = 0x00;

		if (!read_xbconfig_file(configPath + "BFM\\bfm.ini"))
		{
			MessageBox("Error reading BFM config file.", "Error", MB_ICONSTOP);
			bfmRestore(tempPath);		// restore normal 2bl
			close_log();
			return;
		}
	}
	else
	{
		if (!read_xbconfig_file(configFilename))
		{
			MessageBox("Error reading config file.", "Error", MB_ICONSTOP);
			close_log();
			return;
		}
	}
	if(err = XBKernel->Write())
	{
		DisplayErrorMsg(err);
		if (bfmRepackFlag)
			bfmRestore(tempPath);		// restore normal 2bl
		close_log();
		return;
	}
	SetDlgItemText(IDC_STATIC_BMD5, biosID);

	if (putFileSel(&saveFPName, &saveDirName, &saveFName, fSaveTitle, fSelFilter, this->m_hWnd))
	{
		if(!do_xbpack(tempPath, saveFPName, gdwRomSize))
			MessageBox("BIOS packing failed. See logfile for more information.", "Error", MB_ICONSTOP);
		else
			if(getIDString(saveFPName, biosID))	// get the new MD5
				SetDlgItemText(IDC_STATIC_BMD5, biosID);
	}
	if (bfmRepackFlag)
		bfmRestore(tempPath);		// restore normal 2bl
	close_log();
}

void CXBtoolDlg::OnBnClickedClipb()
{
	CString md5Clip;

	md5Clip = "0x";
	md5Clip += biosID;

	md5Clip.MakeLower();

	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, md5Clip.GetLength()+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(md5Clip));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
	

}

void CXBtoolDlg::OnBnClickedRadioSz1()
{
	gdwRomSize = 0x40000;
}

void CXBtoolDlg::OnBnClickedRadioSz2()
{
		gdwRomSize = 0x80000;
}

void CXBtoolDlg::OnBnClickedRadioSz4()
{
		gdwRomSize = 0x100000;
}

void CXBtoolDlg::clearRadioButtons(void)
{
	for (int i = 1060; i < 1070; i++)
		CheckDlgButton(i, 0);
}

void CXBtoolDlg::getFilePaths(void)
{
	CFileFind finder;
	TCHAR* pstrExePath = theAppPathName.GetBuffer(MAX_PATH);

	::GetModuleFileName (0, pstrExePath, MAX_PATH);
	theAppPathName.ReleaseBuffer ();

	finder.FindFile(theAppPathName);
	finder.FindNextFile();
	theAppName = finder.GetFileName();
	finder.Close();

	theAppPath = theAppPathName;
	theAppPath.Truncate(theAppPath.GetLength() - theAppName.GetAllocLength());

	logFilename = theAppPath + "log.txt";
	configPath	= theAppPath + configFolderName;
	presetPath	= theAppPath + presetFolderName;
	tempPath	= theAppPath + tempFolderName;
	bfmPath	= theAppPath + bfmFolderName;

	if(_access(presetPath, 0x00) == -1)
		mkdir(presetPath);
	if(_access(configPath, 0x00) == -1)
		mkdir(configPath);
}

void CXBtoolDlg::setOptionsInDialog(void)
{
	CString KernelInfoStr;
	KernelInfoStr.Format("%s (%d.%02d.%d.%02d)", XBKernel->mOpts.Name,
		XBKernel->mOpts.Version.xbox, XBKernel->mOpts.Version.major,
		XBKernel->mOpts.Version.minor, XBKernel->mOpts.Version.rev);
	SetDlgItemText(IDC_STATIC_NAME, KernelInfoStr);

	clearRadioButtons();

	HYPERLINKCOLORS LinkColors;
	mHomeHyperLink.ModifyLinkStyle(0L, CHyperLink::StyleUseHover);
	mHelpHyperLink.ModifyLinkStyle(0L, CHyperLink::StyleUseHover);
	mHomeHyperLink.ModifyLinkStyle(0L, CHyperLink::StyleNoActiveColor);
	mHelpHyperLink.ModifyLinkStyle(0L, CHyperLink::StyleNoActiveColor);
	mHomeHyperLink.GetColors(LinkColors);
	LinkColors.crVisited = LinkColors.crLink;
	mHomeHyperLink.SetColors(LinkColors);
	mHelpHyperLink.SetColors(LinkColors);
//	mHomeHyperLink.SetWindowText("XBtool Website");
//	mHelpHyperLink.SetWindowText("Help");
	mHomeHyperLink.SetURL("http://home.alltel.net/nghtshd/xbtool.html");
	mHelpHyperLink.SetURL("http://home.alltel.net/nghtshd/XBtoolDoc.html");
	
	mLogoColorButton.SetColor(XBKernel->mOpts.XboxColor);
	mXLipColorButton.SetColor(XBKernel->mOpts.XLipColor);
	mXLipHighlightsColorButton.SetColor(XBKernel->mOpts.XLipHighlightsColor);
	mXLogoInnerColorButton.SetColor(XBKernel->mOpts.XWallColor);
	mXLogoGlowColorButton.SetColor(XBKernel->mOpts.XGlowColor);

	switch(XBKernel->mOpts.LEDColor)
	{
	case 0xF0:
		CLEDRedRadio->SetCheck(1);
		enableCycles(false);
		break;
	case 0x0F:
		CLEDGreenRadio->SetCheck(1);
		enableCycles(false);
		break;
	case 0xFF:
		CLEDOrangeRadio->SetCheck(1);
		enableCycles(false);
		break;
	case 0x00:
		CLEDOffRadio->SetCheck(1);
		enableCycles(false);
		break;
	default:
		CLEDCycleRadio->SetCheck(1);
		enableCycles(true);
		break;
	}
	setLEDCycles(XBKernel->mOpts.LEDColor);

	for (int i = 0; i < 3 && XBKernel->mSupportedOpts.DashName; i++)
	{
		switch(XBKernel->mOpts.BootPart[i])
		{
		case 0x32:
			SpinBoot[i]->SetPos(0);
			StaticBoot[i]->SetWindowText("C:\\");
			break;
		case 0x31:
			SpinBoot[i]->SetPos(1);
			StaticBoot[i]->SetWindowText("E:\\");
			break;
		case 0x36:
			SpinBoot[i]->SetPos(2);
			StaticBoot[i]->SetWindowText("F:\\");
			break;
		case 0x37:
			SpinBoot[i]->SetPos(2);
			StaticBoot[i]->SetWindowText("G:\\");
			break;
		default:
			XBKernel->mOpts.BootPart[i] = 0x32;
			SpinBoot[i]->SetPos(0);
			StaticBoot[i]->SetWindowText("C:\\");
			break;
		}
		EditBoot[i]->SetWindowText(XBKernel->mOpts.DashName[i]);
		EditBoot[i]->SetLimitText(XBKernel->mOpts.DashNameLen[i]);
	}

	EditDefXbe->SetWindowText(XBKernel->mOpts.DefaultXbeName);

	AnimCheck->SetCheck(XBKernel->mOpts.Animation);
	XLogoCheck->SetCheck(XBKernel->mOpts.XLogo);
	SplashLogoCheck->SetCheck(XBKernel->mOpts.SplashLogo);
	TMCheck->SetCheck(XBKernel->mOpts.TrademarkSymbol);
	DVDCheck->SetCheck(XBKernel->mOpts.DVDCheck);
	ResetCheck->SetCheck(XBKernel->mOpts.ResetOnEject);
	EepromCheck->SetCheck(XBKernel->mOpts.EmbedEeprom);

	char keyText[41];
	if (XBKernel->mOpts.EmbedHDKey)
	{
		KeyCheck->SetCheck(1);
		int i;
		for (i = 0; i < 20; i++)
			sprintf(&keyText[i * 2], "%02X", XBKernel->mOpts.HDKey[i]);
//			_ultoa((unsigned long) XBKernel->mOpts.HDKey[i], &keyText[i * 2], 16);

		HDKeyEdit->SetText(keyText);
		HDKeyEdit->EnableWindow(true);
	}
	else
	{
		KeyCheck->SetCheck(0);
		HDKeyEdit->SetText("");
		HDKeyEdit->EnableWindow(false);
	}

	BFMFlagCheck->SetCheck(XBKernel->mOpts.BootFlags == 0x03);
	LBA48Check->SetCheck(XBKernel->mOpts.LBA48);
	PartitionOptsCombo->SetCurSel(XBKernel->mOpts.PartitionOpt);
	//PartitionOptsCombo->EnableWindow(XBKernel->mOpts.LBA48);
	IgnoreHDTableCheck->SetCheck(XBKernel->mOpts.IgnoreHDTable);

	switch (gdwRomSize)
	{
	case 0x100000:
		Size4Radio->SetCheck(1);
		break;
	case 0x80000:
		Size2Radio->SetCheck(1);
		break;
	default:
		Size1Radio->SetCheck(1);
		gdwRomSize = 0x40000;
		break;
	}

	char speed[5];
	if (XBKernel->mOpts.FanSpeed < 20)
		XBKernel->mOpts.FanSpeed = 20;
	else if (XBKernel->mOpts.FanSpeed > 100)
		XBKernel->mOpts.FanSpeed = 100;
	_itoa(XBKernel->mOpts.FanSpeed, speed, 10);
	FanSpeedEdit->SetText(speed);
}

void CXBtoolDlg::OnBnClickedCheckLogo()
{
	if(!XLogoCheck->GetCheck())
	{
		CheckDlgButton(IDC_CHECK_ANI, 0);
		AnimCheck->EnableWindow(false);
	}
	else
		AnimCheck->EnableWindow(true);
}

void CXBtoolDlg::OnCbnSelchangeComboPre()
{
	CString thePresetName, thePresetFile;
	getOptionsFromDialog();
	if (!PresetCombo->GetCurSel())
	{
		XBKernel->Read();
		GetDlgItem(IDC_BUTTON_DELPRE)->EnableWindow(false);
	}
	else
	{
		PresetCombo->GetLBText(PresetCombo->GetCurSel(), thePresetName);
		thePresetFile = presetPath + thePresetName + ".xbt";
		loadPreset(&thePresetFile, &XBKernel->mOpts);
		GetDlgItem(IDC_BUTTON_DELPRE)->EnableWindow(true);
	}
	setOptionsInDialog();
}

void CXBtoolDlg::OnBnClickedButtonSavpre()
{
	CString saveFPName, presetName;
	if (putPreFileSel(&presetPath, &saveFPName, &presetName, fPreSaveTitle, fPreSelFilter, this->m_hWnd))
	{
		getOptionsFromDialog();
		if(savePreset(&saveFPName, &XBKernel->mOpts))
		{
			loadPresetNames();
			PresetCombo->SetCurSel(PresetCombo->FindString(0, presetName));
			GetDlgItem(IDC_BUTTON_DELPRE)->EnableWindow(true);
		}
	}
}

int CXBtoolDlg::loadPresetNames(void)
{
	GetDlgItem(IDC_BUTTON_DELPRE)->EnableWindow(false);
	PresetCombo->ResetContent();

	CFileFind finder;
	BOOL bWorking = finder.FindFile(presetPath + "*.xbt");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory())
			continue;
		PresetCombo->AddString(finder.GetFileTitle());
	}
	finder.Close();
	PresetCombo->InsertString(0, "Current BIOS");
	PresetCombo->SetCurSel(0);

	SetComboWidth(PresetCombo);

	return 0;
}

int CXBtoolDlg::loadConfigTypeNames(void)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(configPath + "*.ini");
	int i = 0;

	while (bWorking && (i < 10))
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory())
			continue;
		TypeCombo->AddString(finder.GetFileTitle());
		configFiles[i++] = finder.GetFilePath();
	}
	finder.Close();
	if(!TypeCombo->GetCount())
	{
		MessageBox("No config files found.\nXBtool will quit now.", "Error", MB_ICONSTOP);
		OnOK();
	}
	TypeCombo->SetCurSel(0);

	SetComboWidth(TypeCombo);

	return 0;
}

int CXBtoolDlg::getConfigFilename(CString *theConfigFile)
{
	*theConfigFile = configFiles[TypeCombo->GetCurSel()];

	return 0;
}

void CXBtoolDlg::OnBnClickedButtonDelPre()
{
	CString delMsg = "Really Delete: ";
	CString delPre;

	PresetCombo->GetLBText(PresetCombo->GetCurSel(),delPre );
	delMsg += delPre;
	if (AfxMessageBox(delMsg, MB_YESNO, 0) == IDYES)
	{
		GetDlgItem(IDC_BUTTON_DELPRE)->EnableWindow(false);
		delPre = presetPath + delPre + ".xbt";
		unlink(delPre);
		PresetCombo->DeleteString(PresetCombo->GetCurSel());
		PresetCombo->SetCurSel(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Color Picker message handlers

LONG CXBtoolDlg::OnSelEndOK(UINT /*lParam*/, LONG /*wParam*/)
{
	COLORREF	colorRef;
	int			redColor, greenColor, blueColor;

    TRACE0("Selection ended OK\n");

	colorRef = mLogoColorButton.GetColor();
	redColor = GetRValue(colorRef);
	greenColor = GetGValue(colorRef);
	blueColor = GetBValue(colorRef);

    return TRUE;
}

LONG CXBtoolDlg::OnSelEndCancel(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Selection cancelled\n");
    return TRUE;
}

LONG CXBtoolDlg::OnSelChange(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Selection changed\n");
    return TRUE;
}

LONG CXBtoolDlg::OnCloseUp(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Colour picker close up\n");
    return TRUE;
}

LONG CXBtoolDlg::OnDropDown(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Colour picker drop down\n");
    return TRUE;
}


int CXBtoolDlg::setLEDCycles(unsigned char theCycles)
{
	unsigned char redMask = 0x80;
	unsigned char grnMask = 0x08;
	int i;

	for(i = 0; i < 4; i++)
	{
		if (theCycles & redMask)
			RedChecks[i]->SetCheck(BST_CHECKED);
		else
			RedChecks[i]->SetCheck(BST_UNCHECKED);
		redMask >>= 1;
	}
	for(i = 0; i < 4; i++)
	{
		if (theCycles & grnMask)
			GrnChecks[i]->SetCheck(BST_CHECKED);
		else
			GrnChecks[i]->SetCheck(BST_UNCHECKED);
		grnMask >>= 1;
	}

	return 0;
}

void CXBtoolDlg::OnBnClickedRadioLedr()
{
	setLEDCycles(0xF0);
	enableCycles(false);
}

void CXBtoolDlg::OnBnClickedRadioLedg()
{
	setLEDCycles(0x0F);
	enableCycles(false);
}

void CXBtoolDlg::OnBnClickedRadioLedo()
{
	setLEDCycles(0xFF);
	enableCycles(false);
}

void CXBtoolDlg::OnBnClickedRadioLedn()
{
	setLEDCycles(0x00);
	enableCycles(false);
}

void CXBtoolDlg::OnBnClickedRadioLedc()
{
	enableCycles(true);
}

void CXBtoolDlg::enableCycles(bool enabled)
{
	int i;
	for (i = 0; i < 4; i++)
		RedChecks[i]->EnableWindow(enabled);
	for (i = 0; i < 4; i++)
		GrnChecks[i]->EnableWindow(enabled);
	return;
}


void CXBtoolDlg::OnBnClickedSizeButton()
{
	CString biosFPName, biosDirName, biosFName;
	CString saveFPName, saveDirName, saveFName;

	if (getFileSel(&biosFPName, &biosDirName, &biosFName, fSelTitle,
		fSelFilter, this->m_hWnd))
	{
		if (putFileSel(&saveFPName, &saveDirName, &saveFName, fSaveTitle, fSelFilter, this->m_hWnd))
		{
			if(resizeBIOS(biosFPName, saveFPName, gdwRomSize))
				SetDlgItemText(IDC_STATIC_INFO_BOX, "Resized " + biosFName);
		}
	}
}

void CXBtoolDlg::OnDeltaposSpinBoot1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;

	if(pNMUpDown->iDelta == 1)
	{
		switch(pNMUpDown->iPos)
		{
		case 0:
			SetDlgItemText(IDC_STATIC_BOOT1, "E:\\");
			break;
		case 1:
			SetDlgItemText(IDC_STATIC_BOOT1, "F:\\");
			break;
		case 2:
			SetDlgItemText(IDC_STATIC_BOOT1, "G:\\");
			break;
		}
	}
	else
	{
		switch(pNMUpDown->iPos)
		{
		case 3:
			SetDlgItemText(IDC_STATIC_BOOT1, "F:\\");
			break;
		case 2:
			SetDlgItemText(IDC_STATIC_BOOT1, "E:\\");
			break;
		case 1:
			SetDlgItemText(IDC_STATIC_BOOT1, "C:\\");
			break;
		}
	}
}

void CXBtoolDlg::OnDeltaposSpinBoot2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;

	if(pNMUpDown->iDelta == 1)
	{
		switch(pNMUpDown->iPos)
		{
		case 0:
			SetDlgItemText(IDC_STATIC_BOOT2, "E:\\");
			break;
		case 1:
			SetDlgItemText(IDC_STATIC_BOOT2, "F:\\");
			break;
		case 2:
			SetDlgItemText(IDC_STATIC_BOOT2, "G:\\");
			break;
		}
	}
	else
	{
		switch(pNMUpDown->iPos)
		{
		case 2:
			SetDlgItemText(IDC_STATIC_BOOT2, "F:\\");
			break;
		case 1:
			SetDlgItemText(IDC_STATIC_BOOT2, "E:\\");
			break;
		case 0:
			SetDlgItemText(IDC_STATIC_BOOT2, "C:\\");
			break;
		}
	}
}

void CXBtoolDlg::OnDeltaposSpinBoot3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	*pResult = 0;

	if(pNMUpDown->iDelta == 1)
	{
		switch(pNMUpDown->iPos)
		{
		case 0:
			SetDlgItemText(IDC_STATIC_BOOT3, "E:\\");
			break;
		case 1:
			SetDlgItemText(IDC_STATIC_BOOT3, "F:\\");
			break;
		case 2:
			SetDlgItemText(IDC_STATIC_BOOT3, "G:\\");
			break;
		}
	}
	else
	{
		switch(pNMUpDown->iPos)
		{
		case 2:
			SetDlgItemText(IDC_STATIC_BOOT3, "F:\\");
			break;
		case 1:
			SetDlgItemText(IDC_STATIC_BOOT3, "E:\\");
			break;
		case 0:
			SetDlgItemText(IDC_STATIC_BOOT3, "C:\\");
			break;
		}
	}
}

int CXBtoolDlg::bfmSetup(const char *indirname)
{
	CString inDir = indirname;
	CString bfmDir = configPath + "BFM\\";

	unlink(inDir + "2bl.bak");
//	unlink(inDir + "remainder.bak");

	if (rename(inDir + "2bl.img", inDir + "2bl.bak"))
		return errBFMSetupRename;
//	if (rename(inDir + "remainder.img", inDir + "remainder.bak"))
//		return errBFMSetupRename;
	if (!CopyFile(bfmDir + "bfm_2bl.img", inDir + "2bl.img", false))
		return errBFMSetupCopy;
//	if (!CopyFile(bfmDir + "bfm_remainder.img", inDir + "remainder.img", false))
//		return errBFMSetupCopy;
	if (!CopyFile(inDir + "xboxkrnl.img", inDir + "xboxkrnl.bak", false))
		return errBFMSetupCopy;

	return 0;
}
int CXBtoolDlg::bfmRestore(const char *indirname)
{
	CString inDir = indirname;
	CString bfmDir = configPath + "BFM\\";

	unlink(inDir + "2bl.img");
//	unlink(inDir + "remainder.img");
	unlink(inDir + "xboxkrnl.img");
	rename(inDir + "2bl.bak", inDir + "2bl.img");
//	rename(inDir + "remainder.bak", inDir + "remainder.img");
	rename(inDir + "xboxkrnl.bak", inDir + "xboxkrnl.img");

	return 1;
}

void CXBtoolDlg::enableDlgItems(void)
{
	int i;

	mLogoColorButton.EnableWindow(XBKernel->mSupportedOpts.XboxColor);
	mXLipColorButton.EnableWindow(XBKernel->mSupportedOpts.XLipColor);
	mXLipHighlightsColorButton.EnableWindow(XBKernel->mSupportedOpts.XLipHighlightsColor);
	mXLogoInnerColorButton.EnableWindow(XBKernel->mSupportedOpts.XWallColor);
	mXLogoGlowColorButton.EnableWindow(XBKernel->mSupportedOpts.XLipColor);

	CLEDRedRadio->EnableWindow(XBKernel->mSupportedOpts.LEDColor);
	CLEDGreenRadio->EnableWindow(XBKernel->mSupportedOpts.LEDColor);
	CLEDOrangeRadio->EnableWindow(XBKernel->mSupportedOpts.LEDColor);
	CLEDOffRadio->EnableWindow(XBKernel->mSupportedOpts.LEDColor);
	CLEDCycleRadio->EnableWindow(XBKernel->mSupportedOpts.LEDColor);
	
	for (i = 0; i < 4; i++)
	{
		RedChecks[i]->EnableWindow(XBKernel->mSupportedOpts.LEDColor);
		GrnChecks[i]->EnableWindow(XBKernel->mSupportedOpts.LEDColor);
	}

	for (i = 0; i < 3; i++)
	{

		SpinBoot[i]->EnableWindow(XBKernel->mSupportedOpts.BootPart);
		StaticBoot[i]->EnableWindow(XBKernel->mSupportedOpts.BootPart);
		EditBoot[i]->EnableWindow(XBKernel->mSupportedOpts.DashName);
	}
	EditDefXbe->EnableWindow(XBKernel->mSupportedOpts.DefaultXbeName);


	AnimCheck->EnableWindow(XBKernel->mSupportedOpts.Animation);
	XLogoCheck->EnableWindow(XBKernel->mSupportedOpts.XLogo);
	SplashLogoCheck->EnableWindow(XBKernel->mSupportedOpts.SplashLogo);
	TMCheck->EnableWindow(XBKernel->mSupportedOpts.TrademarkSymbol);
	DVDCheck->EnableWindow(XBKernel->mSupportedOpts.DVDCheck);
	BFMFlagCheck->EnableWindow(XBKernel->mSupportedOpts.BootFlags);
	ResetCheck->EnableWindow(XBKernel->mSupportedOpts.ResetOnEject);
	EepromCheck->EnableWindow(XBKernel->mSupportedOpts.EmbedEeprom);
	KeyCheck->EnableWindow(XBKernel->mSupportedOpts.EmbedHDKey);
	HDKeyEdit->EnableWindow(KeyCheck->GetCheck());
	LBA48Check->EnableWindow(XBKernel->mSupportedOpts.LBA48);
	IgnoreHDTableCheck->EnableWindow(XBKernel->mSupportedOpts.IgnoreHDTable && XBKernel->mOpts.LBA48);
	PartitionOptsCombo->EnableWindow(XBKernel->mSupportedOpts.PartitionOpt && XBKernel->mOpts.LBA48);
	FanSpeedEdit->EnableWindow(XBKernel->mSupportedOpts.FanSpeed);
}

void CXBtoolDlg::OnBnClickedCheckBfmF()
{
	if (BFMFlagCheck->GetCheck())
		if (AfxMessageBox("Warning: if this box is checked then the kernel will not boot when loaded from ROM.",
			MB_OKCANCEL, 0) == IDCANCEL)
			BFMFlagCheck->SetCheck(0);
}

void CXBtoolDlg::OnBnClickedCheckLba48()
{
	if (LBA48Check->GetCheck())
	{
		PartitionOptsCombo->EnableWindow(1);
		IgnoreHDTableCheck->EnableWindow(1);
	}
	else
	{
		PartitionOptsCombo->EnableWindow(0);
		IgnoreHDTableCheck->EnableWindow(0);
	}
}

void CXBtoolDlg::SetComboWidth(CComboBox * combo)
{
	// Find the longest string in the combo box.
	CString      str;
	CSize      sz;
	int      dx = 0;
	TEXTMETRIC   tm;
	CDC*      pDC = combo->GetDC();
	CFont*      pFont = combo->GetFont();

	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm);

	for (int i = 0; i < combo->GetCount(); i++)
	{
		combo->GetLBText(i, str);
		sz = pDC->GetTextExtent(str);

		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;

		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	combo->ReleaseDC(pDC);

	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);

	// Set the width of the list box so that every item is completely visible.
	combo->SetDroppedWidth(dx);
}

void CXBtoolDlg::DisplayErrorMsg(int theErr)
{
	static char * errMsg[] = { "No Error.",
		"Error writing color patch.\nFile not saved.",
		"Error writing animation patch.\nFile not saved.",
		"Error writing X logo patch.\nFile not saved.",
		"Error writing splash logo patch.\nFile not saved.",
		"Error writing TM patch.\nFile not saved.",
		"Error writing dashboard patch.\nFile not saved.",
		"Error writing DVD check.\nFile not saved.",
		"Error writing LED color patch.\nMake sure all patch files are in the Patch Data folder.\nFile not saved.",
		"Error writing LBA48 patch.\nMake sure all patch files are in the Patch Data folder.\nFile not saved.",
		"Error writing boot flags.\nFile not saved.",
		"Error expanding kernel.\nMake sure all patch files are in the Patch Data folder.\nFile not saved.",
		"Error writing kernel file.\nMake sure the kernel file is not in use by another app.\nFile not saved.",
		"Error writing embedded eeprom patch.\nDid you place the \"eeprom.bin\" file in the Patch Data folder?\nFile not saved.",
		"Error writing hard drive key.\nFile not saved.",
		"Error renaming files for BFM conversion.",
		"Error copying files for BFM conversion."
	};

	MessageBox(errMsg[theErr]);
}

void CXBtoolDlg::OnBnClickedKeyCheck()
{
	// TODO: Add your control notification handler code here
	HDKeyEdit->EnableWindow(KeyCheck->GetCheck());
}
