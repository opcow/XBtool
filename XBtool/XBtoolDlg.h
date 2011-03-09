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

// XBtoolDlg.h : header file
//

#pragma once

#include "ColorDlg\ColorButton.h"
#include ".\StringEdit.h"
#include "hyperlink.h"

// CXBtoolDlg dialog
class CXBtoolDlg : public CDialog
{
// Construction
public:
	CXBtoolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_XBtool_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	BOOL PreTranslateMessage(MSG* pMsg);
public:
	CColorButton	mLogoColorButton, mXLipHighlightsColorButton, mXLipColorButton;
	CColorButton	mXLogoInnerColorButton, mXLogoGlowColorButton;
	CHyperLink		mHomeHyperLink, mHelpHyperLink;
	CToolTipCtrl	m_ToolTips;

	afx_msg void OnBnClickedUnpackb();
	afx_msg void OnBnClickedPackb();
	afx_msg void OnBnClickedOpenb();
	afx_msg void OnBnClickedQuitb();
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedSaveasb();
	afx_msg void OnBnClickedClipb();
	afx_msg void OnBnClickedRadioSz1();
	afx_msg void OnBnClickedRadioSz2();
	afx_msg void OnBnClickedRadioSz4();
	afx_msg void OnBnClickedCheckLogo();
	int getOptionsFromDialog();
	void clearRadioButtons(void);
	void getFilePaths(void);
	void setOptionsInDialog(void);
	~CXBtoolDlg(void);

	//color picker messages
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);
    afx_msg LONG OnCloseUp(UINT lParam, LONG wParam);
    afx_msg LONG OnDropDown(UINT lParam, LONG wParam);
	//end color picker messages

	afx_msg void OnCbnSelchangeComboPre();
	afx_msg void OnBnClickedButtonSavpre();
	int loadPresetNames(void);
	afx_msg void OnBnClickedButtonDelPre();
	int loadConfigTypeNames(void);
	int getConfigFilename(CString *theConfigFile);
	int setLEDCycles(unsigned char theCycles);
	afx_msg void OnBnClickedRadioLedr();
	afx_msg void OnBnClickedRadioLedg();
	afx_msg void OnBnClickedRadioLedo();
	afx_msg void OnBnClickedRadioLedn();
	void enableCycles(bool enabled);
	afx_msg void OnBnClickedRadioLedc();
	afx_msg void OnBnClickedSizeButton();
	afx_msg void OnDeltaposSpinBoot1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBoot2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBoot3(NMHDR *pNMHDR, LRESULT *pResult);
	int bfmSetup(const char *indirname);
	int bfmRestore(const char *indirname);
	void enableDlgItems(void);
	afx_msg void OnBnClickedCheckBfmF();
	afx_msg void OnBnClickedCheckLba48();
	void SetComboWidth(CComboBox * combo);

	CStringEdit m_SpeedEdit;
	CStringEdit m_KeyEdit;
	void DisplayErrorMsg(int patchErr);
	afx_msg void OnBnClickedKeyCheck();
};
