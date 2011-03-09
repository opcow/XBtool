//*****************************************************************************
//	header file
//*****************************************************************************
//
//	FILE NAME:	StringEdit.h
//

#if !defined(AFX_STRINGEDIT_H__A8199B9F_CB7C_12E1_ADB8_000000000000__INCLUDED_)
#define AFX_STRINGEDIT_H__A8199B9F_CB7C_12E1_ADB8_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CStringEdit : public CEdit
{
public:
// Construction
	CStringEdit();

//	Destructor
	virtual ~CStringEdit();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStringEdit)
	//}}AFX_VIRTUAL

// Implementation
	void SetFixedLen(BOOL bFixedLen);				// set fixed length flag
	BOOL GetFixedLen(void) { return(m_bFixedLen); }	// read back the fixed length flag
	void SetMode(BOOL bMode);						// sets edit/status mode
	BOOL GetMode(void) { return(m_bMode); }			// reads back the mode flag
	void SetAlertActive(UINT nSeconds);				// start active alert mode 
	void SetAlertInactive(void);					// End the alert mode 
	void SetMaxLength(int nMax);					// method to set the maximum string length
	int	 GetMaxLength(void) { return(m_nMax); }		// method to read back the maximum string length
	void SetLegal(LPCSTR lpszLegal);			    // sets the string of legal characters
	void GetLegal(CString& strLegal) { strLegal = m_strLegal; }	   // gets back the legal string
	void SetIllegal(LPCSTR lpszIllegal);			// sets the string of illegal characters
	void GetIllegal(CString& strIllegal) { strIllegal = m_strIllegal; }	   // gets back the illlegal string
	void SetStatus(LPCSTR lpszStatus);				// set status string
	void GetStatus(CString& strStatus) { strStatus = m_strStatus; }	   // gets back the status string
	void SetAlert(LPCSTR lpszAlert);				// set alert string
	void GetAlert(CString& strAlert) { strAlert = m_strAlert; }	   // gets back the alert string
	void SetText(LPCSTR lpszText);					// plug text into control  
	void GetText(CString& strText);					// take text out of control

protected:
	// Generated message map functions
	//{{AFX_MSG(CStringEdit)
	afx_msg BOOL OnUpdate();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
//  Implementation
	BOOL	ValidateText(CString& strText);	// returns TRUE if text validates OK
//	Attributes
	BOOL	m_bFixedLen;	//  Flag to control the return of a string padded with 
							//  spaces to m_nMax length. This is ignored if the
							//  maximum length is set to <=0.
	int		m_nMax;			//	Maximum number characters in the edit string
	BOOL	m_bMode;		//  Flag to control the display mode to show status
							//  if true and the edit text if in the false state
	BOOL	m_bAlert;		//  Flag to control overide Alert Mode if true
	CString	m_strLegal;		//  string of legal input characters
	CString m_strIllegal;	//  string of illegal characters.
	CString m_strLastValid; //  buffer of last valid string entry
	CString m_strStatus;	//  buffer to hold Status text
	CString m_strAlert;		//  buffer to hold Alert text
	COLORREF m_clrText;		//  holds color of edit text
	COLORREF m_clrEdBkgnd;	//  Color of the Edit Mode background
	COLORREF m_clrStBkgnd;	//  Color of the Status Mode background
	COLORREF m_clrAlBkgnd;	//  Color of the Alert Mode background
	CBrush   m_brEdBkgnd;	//	Brush for the Edit Mode background
	CBrush	 m_brStBkgnd;   //	Brush for the Status Mode background
	CBrush	 m_brAlBkgnd;   //	Brush for the Alert Mode background
	UINT	 m_nTimerID;	//  Timer number for the alert timer
	UINT	 m_nTimerCntr;	//  Timer count down register.
	UINT	 m_nTimerEvent;	//  Event ID for the timer.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRINGEDIT_H__A8199B9F_CB7C_12E1_ADB8_000000000000__INCLUDED_)
