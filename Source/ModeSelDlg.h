#if !defined(AFX_MODESELDLG_H__2ED8B712_4C60_4C83_AF3A_8421F0C646EB__INCLUDED_)
#define AFX_MODESELDLG_H__2ED8B712_4C60_4C83_AF3A_8421F0C646EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModeSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModeSelDlg dialog

class CModeSelDlg : public CDialog
{
// Construction
public:
	CFont * m_pfont1;
	BOOL *m_bOperMode;
	BOOL *m_bDisAutoUSB;
	BOOL *m_bRebootDUT;
	CModeSelDlg(BOOL *bOperMode = NULL, BOOL *bDisAutoUSB = NULL, BOOL *bRebootDUT = NULL, CWnd* pParent = NULL);   // standard constructor
	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CModeSelDlg)
	enum { IDD = IDD_DIALOG_MODESEL };
	CButton	m_ChkDisableAutoUSB;
	int		m_nMode;
	CString	m_strStaticNormalModeTips;
	CString	m_strStaticWorkingModeTips;
	CButton m_chkNotRebootDUT;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModeSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModeSelDlg)
	virtual void OnOK();
	afx_msg void OnRadioMode2();
	afx_msg void OnRadioMode1();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODESELDLG_H__2ED8B712_4C60_4C83_AF3A_8421F0C646EB__INCLUDED_)
