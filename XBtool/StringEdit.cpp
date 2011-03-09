////////////////////////////////////////////////////////////////////////////////
//	implementation file
////////////////////////////////////////////////////////////////////////////////
//
//	FILE NAME:	StringEdit.cpp
//
//	Include Files
//
#include "stdafx.h"
#include "StringEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit constructor
//

CStringEdit::CStringEdit() :
m_nMax(0),			 // Initialize for default longest string
m_bFixedLen(FALSE),	 // Default length is variable
m_bMode(FALSE),		 // Default to the edit mode
m_bAlert(FALSE),	 // Default no alert active
m_strLegal(_T("")),  // Initialize for all characters legal
m_strIllegal(_T("")),  // Initialize for no illegal
m_strLastValid(_T("")),   // Initialize the last valid string
m_strStatus(_T("")),	// Initialize the status string off
m_strAlert(_T(""))		// Initialize the alert string off
{
	m_clrText=RGB(0,0,0);	// make black text color
	m_clrEdBkgnd=RGB(255,255,255); // white background edit mode
	m_clrStBkgnd=RGB(255,255,  0); // yellow background status mode
	m_clrAlBkgnd=RGB(255,  0,  0); // red background alert mode
	m_brEdBkgnd.CreateSolidBrush(m_clrEdBkgnd); // make edit mode brush
	m_brStBkgnd.CreateSolidBrush(m_clrStBkgnd); // make status mode brush
	m_brAlBkgnd.CreateSolidBrush(m_clrAlBkgnd); // make alert mode brush


	// clear the timer control variables
	m_nTimerID = 0;
	m_nTimerCntr = 0;
	m_nTimerEvent = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit destructor
//

CStringEdit::~CStringEdit()
{
}

////////////////////////////////////////////////////////////////////////////////
//
//	BEGIN_MESSAGE_MAP and END_MESSAGE_MAP macros
//

BEGIN_MESSAGE_MAP(CStringEdit, CEdit)
	//{{AFX_MSG_MAP(CStringEdit)
	ON_CONTROL_REFLECT_EX(EN_UPDATE, OnUpdate)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// function to get rid of the timer resource
// that we attach to the 
void CStringEdit::OnDestroy() 
{
	// get rid of the timer
	if(m_nTimerID)
	{
		KillTimer(m_nTimerID);
	}
}

// function to manage the timer resouce to get us out of 
// alert mode soon enough
void CStringEdit::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == m_nTimerEvent)
	{
		if(m_bAlert)		// is alert mode active
		{
			if(m_nTimerCntr)	// counter still active?
			{
				m_nTimerCntr--;		// decrement timer
				if(m_nTimerCntr == 0)	// just went to zero
				{
					SetAlertInactive();		// end alert state					
				}
			}
		}
	}
	else
	{
		CEdit::OnTimer(nIDEvent);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetFixedLen
//
//  This member function is called to set flag that controls if the
//  return string is fixed length. If this flag is set true via this
//	function then the control will return a string that is padded
//  to the right with spaces if the text in the control is shorter
//	than the maxlength of text set.
//

void CStringEdit::SetFixedLen(BOOL bFixedLen)
{ 
	m_bFixedLen = bFixedLen;	 // set fixed length flag
	OnUpdate();
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetMaxLength
//
//  This member function is called to set the maximum width of
//  text that can be entered in the control
//

void CStringEdit::SetMaxLength(int nMax)
{ 
	m_nMax = nMax;				// set the maximum control width
	OnUpdate();
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetMode
//
//  This member function is called to set the mode the control
//  operates in. The mode falue of FASLE puts the edit control into 
//	normal edit configuration, In the TRUE state the control turns
//  into a STATUS display showing the status text string. In status mode
//  the control ceases operation as an edit control, will not accept
//  input and will not perform validation on the test in the control.
//

void CStringEdit::SetMode(BOOL bMode)
{
	m_bMode=bMode;					// save local copy of the mode flag

	if(!m_bAlert)
	{
		if(bMode)			// going to Status mode
		{
			SetWindowText(m_strStatus);		// plug in the status text
		}
		else				// going to Edit mode
		{
			SetWindowText(m_strLastValid);	// plug in last known text
		}
	}
	OnUpdate();
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetAlertActive
//
//  This member function is called to set the mode the control
//  operates into the Alert mode. This sets the m_bAlert flag 
//  variable and then starts the timer for the requested time. The
//  window text is set to the Alert Text string. 

void CStringEdit::SetAlertActive(UINT nSeconds)
{
	m_nTimerCntr=nSeconds*3;	// set number of 1/3 second ticks
	m_bAlert=TRUE;				// set flag to indicate in the mode
	SetWindowText(m_strAlert);	// show the alert mode string

	if(m_nTimerID == 0)			// has timer been setup yet?
	{
		// setup a timer in here for support of the alert mode
		m_nTimerEvent=GetDlgCtrlID();	// used ID of this object for event ID
		m_nTimerID=SetTimer(m_nTimerEvent,333,NULL);	// setup timer for 1/3 seconds
	}

	OnUpdate();
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetAlertInactive
//
//  This member function is called to set the mode the control
//  out of the Alert mode. Clears the alert time period counter
//	and the alert mode flag m_bAlert.. 

void CStringEdit::SetAlertInactive(void)
{
	m_nTimerCntr=0;				// clear the timer
	m_bAlert=FALSE;				// end the mode
	SetMode(m_bMode);			// reset the mode back
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetLegal
//
//	function will set the control up with the desired string. 
//  of legal characters. The entry string contains only those
//	characters than can be entered into the control 
//

void CStringEdit::SetLegal(LPCSTR lpszLegal)
{
	m_strLegal = lpszLegal;			// save legal string into member string

	// if new legal string does not validate text the clear last valid 
	if(ValidateText(m_strLastValid) == FALSE)
	{
		m_strLastValid.Empty();
	}

	if((m_bMode == FALSE) && (m_bAlert == FALSE))	// in edit mode
	{
		OnUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetIllegal
//
//	function will set the control up with the desired string. 
//  of illegal characters. The entry string contains characters
//	that specifically cannot be entered into the control 
//

void CStringEdit::SetIllegal(LPCSTR lpszIllegal)
{
	m_strIllegal = lpszIllegal;		// save illegal string into member string

	// if new illegal string does not validate text the clear last valid 
	if(ValidateText(m_strLastValid) == FALSE)
	{
		m_strLastValid.Empty();
	}

	if((m_bMode == FALSE) && (m_bAlert == FALSE))	// in edit mode
	{
		OnUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetStatus
//
//	function will set the control up with the desired status 
//  text string. The entry string contains what is displayed in
//	control when it is in status mode.
//

void CStringEdit::SetStatus(LPCSTR lpszStatus)
{
	m_strStatus = lpszStatus;		// save status string into member string

	if((m_bMode == TRUE) && (m_bAlert == FALSE)) // in status mode but not alert mode
	{
		SetWindowText(m_strStatus);
		OnUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetAlert
//
//	function will set the control up with the desired alert 
//  text string. The entry string contains what is displayed in
//	control when it is in alert mode.
//

void CStringEdit::SetAlert(LPCSTR lpszAlert)
{
	m_strAlert = lpszAlert;		// save alert string into member string

	if(m_bAlert == TRUE)				// in alert mode
	{
		SetWindowText(m_strAlert);
		OnUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.SetText
//
//	function will set the control up with the desired string. 
//  If the string is invalid it is truncated at the first 
//  Illegal character or trimmed to the maximum length. If the
//	control is in the Status mode this simply just validates
//  the string and leaves the validated result in the m_strLastValid 
//  string variable.

void CStringEdit::SetText(LPCSTR lpszText)
{
	CString strText = lpszText;		// copy input text to local space
	ValidateText(strText);			// validate and adjust string if necessary

	if((m_bMode == FALSE) && (m_bAlert == FALSE)) // in edit mode
	{
		SetWindowText(strText);		// place into the control
		OnUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.GetText
//
//	function to fetch out the string from the control and return 
//  it.	If string is too long it is truncated and if too short and
//  the fixed length flag is set the it is padded to the right with
//	spaces. If the control is in STATUS mode then this will process
//  the string from the m_strLastValid buffer instead,
//

void CStringEdit::GetText(CString& strText)
{
	int nLen;
	CString strWinText;
	CString strPadding;

	if((m_bMode) || (m_bAlert))			// status mode or alert mode
	{
		strWinText=m_strLastValid;		// use last valid string.
	}
	else
	{					// edit mode
		GetWindowText(strWinText);		// get whole string from control
	}

	// get length of the text
    nLen=strWinText.GetLength();

	if(m_nMax > 0)
	{
		if(nLen >= m_nMax)
		{
			strWinText=strWinText.Left(m_nMax);	// trim text
		}
		else									// padding is needed ... 
		{
			if(m_bFixedLen)						// ... for fixed length strings
			{
				strPadding.Format(_T("%*s"),(m_nMax-nLen),_T(" "));
				strWinText+=strPadding;			// string plus the padding
			}
		}
	}
	strText=strWinText;							// copy to the caller
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.OnUpdate
//
//  This member function is called when the edit control is
//	about to display altered text. This member function validates the
//  string content and if anything is wrong with it is replaced
//  with the last valid string. This validation only happens IF the
//  control is operating in the edit mode. 
//

BOOL CStringEdit::OnUpdate() 
{
	//	Local Variables
	CString strText;
	CString strSave;
	int nSelBeg;
	int nSelEnd;

	if(m_bAlert)				// in alert mode ?
	{
		GetWindowText(strText);		// get current text
		if(strText != m_strAlert)	// force alert back if changed
		{
			SetWindowText(m_strAlert);	// force back to alert always
		}
		return(TRUE);		// exit here and prevent parent from 
	}						// seeing a reflected UPDATE message
	else
	{
		if(m_bMode)					// in status mode?
		{
			GetWindowText(strText);		// get current text
			if(strText != m_strStatus)	// force status back if changed
			{
				SetWindowText(m_strStatus);	// force back to status always
			}
			return(TRUE);		// exit here and prevent parent from 
		}						// seeing a reflected UPDATE message
	}

	//	Copy the contents of the edit control to a string
	GetWindowText(strText);
	strSave = strText;			// keep a copy of the text

	// Get the current selection position
	GetSel(nSelBeg,nSelEnd);

	// validate the window text now
	if(ValidateText(strText) == FALSE)
	{
		//	Alert the operator of the error
		MessageBeep(0xFFFFFFFF);
	}

	if(strSave != strText)			// string was changed by validate
	{
		SetWindowText(strText);

		// reset the selector position
		SetSel(nSelBeg,nSelEnd);
	}
	return(FALSE);					// let parent get an UPDATE message too
}

////////////////////////////////////////////////////////////////////////////////
//
//	CStringEdit.ValidateText
//
//	function will validate the text string passed to it. If the string is
//  valid its value is copied to the m_strLastValid string. If it is invalid
//  the last valid value is copied back to the argument string. Validation consists
//  looking at the string in several ways. Length is checked if the member variable
//  m_nMax is set to a > 0 value. Then all internal characters are checked against
//  being members of the m_strLegal string and to make sure that they are not
//  members if the m_strIllegal string. The two content validation processes
//  are each skipped if the respective control strings are empty.
//  Note that illegal characters also sopecified as legal characters will be
//  considered as illegal

BOOL CStringEdit::ValidateText(CString& strText)
{
	BOOL bOkFlag=1;			// default mark string as OK 

	// first if the string is too long trim it back to m_nMax
	if(m_nMax > 0)	// m_nMax being <= 0 means any length is OK
	{
		if(strText.GetLength() > m_nMax)
		{
			strText=strText.Left(m_nMax);		// trim to keep left part
		}
	}

	// check if characters are legal
	for(int i=0; i<strText.GetLength(); i++)
	{
		// check if character is in legal string 
		if(m_strLegal.IsEmpty() == FALSE)
		{
			if(m_strLegal.Find(strText[i]) < 0)		// char not in legal string
			{
				bOkFlag=0;							// mark as bad string
				break;
			}
		}
	
		// check if character is in illegal string
		if(m_strIllegal.IsEmpty() == FALSE)
		{
			if(m_strIllegal.Find(strText[i]) >= 0)	// char is in illegal string
			{
				bOkFlag=0;
				break;
			}
		}
	}

	// check if validates OK. If so then save this as the good string
	// else substitute the last gdod string
	if(bOkFlag)
	{
		m_strLastValid=strText;
	}
	else
	{
		strText=m_strLastValid;
	}

	return(bOkFlag);								// send back validation status
}

// note that returning an non null brush prevents the parent
// routine from being used instead.
HBRUSH CStringEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_clrText);		// color of the text

	if(m_bAlert)				// Alert mode
	{
		pDC->SetBkColor(m_clrAlBkgnd);	// alert background
		return(m_brAlBkgnd);
	}
	else
	{
		if(m_bMode)					// Status mode
		{
			pDC->SetBkColor(m_clrStBkgnd);	// status background
			return(m_brStBkgnd);
		}
		else						// Edit mode
		{
			pDC->SetBkColor(m_clrEdBkgnd);	// editing background
			return(m_brEdBkgnd);
		}
	}
}

