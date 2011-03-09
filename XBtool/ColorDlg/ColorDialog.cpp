///////////////////////////////////////////////////////////////////////////
// ColorDialog.cpp - auto load/save of custom colors CColorDialog extension
// (c) 1997 Roger Onslow

#include "stdafx.h"
#include "ColorDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////
//
// CMyColorDialog property page

IMPLEMENT_DYNCREATE(CMyColorDialog, CColorDialog)

bool CMyColorDialog::c_NeedToInitCustColors = true;
COLORREF CMyColorDialog::c_CustColors[];
COLORREF CMyColorDialog::c_LastCustColors[];

#define SECTION _T("Custom Colors")

void CMyColorDialog::InitCustColors() {
     for (int i = 0; i < NCUSTCOLORS; i++) {
          CString entry; entry.Format("%d",i);
          c_LastCustColors[i] = c_CustColors[i] =
		::AfxGetApp()->GetProfileInt(SECTION,entry,RGB(255,255,255));
     }
     c_NeedToInitCustColors= false;
}

void CMyColorDialog::SaveCustColors() {
     for (int i = 0; i < NCUSTCOLORS; i++) {
          if (c_LastCustColors[i] != c_CustColors[i]) {
               CString entry; entry.Format("%d",i);
               if (c_CustColors[i] == RGB(255,255,255)) {
                    ::AfxGetApp()->WriteProfileString(SECTION,entry,NULL);
               } else {
                    ::AfxGetApp()->WriteProfileInt(SECTION, entry,c_CustColors[i]);
               }
               c_LastCustColors[i] = c_CustColors[i];
          }
     }
}

CMyColorDialog::CMyColorDialog( COLORREF clrInit, DWORD dwFlags, 
		CWnd* pParentWnd) : CColorDialog(clrInit,dwFlags,pParentWnd)
{
     //{{AFX_DATA_INIT(CMyColorDialog)
     //}}AFX_DATA_INIT
     if (c_NeedToInitCustColors) {
          InitCustColors();
     }
     m_cc.lpCustColors = c_CustColors;
}

int CMyColorDialog::DoModal() {
     int code = (int) CColorDialog::DoModal();
     SaveCustColors();
     return code;
}

void CMyColorDialog::DoDataExchange(CDataExchange* pDX) {
     // overridden (calls this base class)
     CColorDialog::DoDataExchange(pDX);
     //{{AFX_DATA_MAP(CMyColorDialog)
     //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyColorDialog, CColorDialog)
//{{AFX_MSG_MAP(CMyColorDialog)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()