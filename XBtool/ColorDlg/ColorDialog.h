// ColorDialog.h : header file
// (c) 1997 Roger Onslow

#ifndef _CMyColorDialog_
#define _CMyColorDialog_

///////////////////////////////////////////////////////////////////////////
//
// CMyColorDialog dialog

class CMyColorDialog : public CColorDialog {
     DECLARE_DYNCREATE(CMyColorDialog);
     // Construction
public:
     CMyColorDialog( COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd*
pParentWnd = NULL );
     // Statics
protected:
     enum { NCUSTCOLORS = 16 };
     static COLORREF c_CustColors[NCUSTCOLORS];
     static COLORREF c_LastCustColors[NCUSTCOLORS];
     static bool c_NeedToInitCustColors;
protected:
     static void InitCustColors();
     static void SaveCustColors();
     // Dialog Data
protected:
     //{{AFX_DATA(CMyColorDialog)
     //}}AFX_DATA
     // Overrides
protected:
     // ClassWizard generate virtual function overrides
     //{{AFX_VIRTUAL(CMyColorDialog)
public:
     virtual int DoModal();
protected:
     virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
     //}}AFX_VIRTUAL
     // Implementation
protected:
     // Generated message map functions
     //{{AFX_MSG(CMyColorDialog)
     //}}AFX_MSG
     DECLARE_MESSAGE_MAP()
};

#endif