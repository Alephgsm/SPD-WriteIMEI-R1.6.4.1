#if !defined(AFX_IMEISETDLG_H__8F23C93D_8158_4D5C_8A75_B26E8AEFFFF0__INCLUDED_)
#define AFX_IMEISETDLG_H__8F23C93D_8158_4D5C_8A75_B26E8AEFFFF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IMEISetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIMEISetDlg dialog

class CIMEISetDlg : public CDialog
{
// Construction
public:
	CIMEISetDlg(WriteFlashPara* pSetParam =NULL,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIMEISetDlg)
	enum { IDD = IDD_DIALOG_SETIMEI };
	CEdit	m_edit_wifiaddrfilepath;
	CEdit	m_edit_btaddrfilepath;
	CButton	m_ChkIMEIAfterRead;
	CComboBox	m_ComboGenTypeIMEI4;
	CComboBox	m_ComboGenTypeIMEI3;
	CComboBox	m_ComboGenTypeIMEI2;
	CComboBox	m_ComboGenTypeIMEI1;
	CButton	m_ChkAutoSN2;
	CButton	m_ChkAutoSN1;
	CButton	m_ChkSN2;
	CButton	m_ChkSN1;
	CButton	m_CtrlCheckVer;
	CButton	m_ChkWifi;
	CButton	m_ChkAutoWifi;
	CButton	m_ChkAutoBT;
	CButton	m_ChkBT;
	CButton	m_ChkIMEI1;
	CButton	m_ChkSameIMEI;
	CButton	m_ChkIMEI4;
	CButton	m_ChkIMEI3;
	CButton	m_ChkIMEI2;
	CButton	m_CtrlSaveMdb;
	CButton	m_CtrlSaveTxt;
	CString	m_AutoIMEI;
	BOOL	m_bCheckMdb;
	BOOL	m_bCheckTxt;
	BOOL	m_bCheckSave;
	CString	m_strVersion;
	int		m_nGenAddrTypeBT;
	int		m_nGenAddrTypeWIFI;
	CString	m_strInitAddrforBT;
	CString	m_strInitAddrforWIFI;
	CButton m_chkAPVer;
	CString m_strAPVer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIMEISetDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIMEISetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckImei1();
	afx_msg void OnCheckImei2();
    afx_msg void OnCheckImei3();
	afx_msg void OnCheckBt();
    afx_msg void OnClose();
	virtual void OnCancel();
	afx_msg void OnChangeEditAutoimei();
	afx_msg void OnCheckTxt();
	afx_msg void OnCheckSave();
	afx_msg void OnCheckMdb();
	afx_msg void OnCheckImei4();
	afx_msg void OnCheckWifi();
	afx_msg void OnCheckVer();
	afx_msg void OnCheckSn1();
	afx_msg void OnCheckSn2();
	afx_msg void OnRadioGenaddrType1();
	afx_msg void OnRadioGenaddrType2();
	afx_msg void OnCheckSameimei();
	afx_msg void OnCheckAutobt();
	afx_msg void OnCheckAutowifi();
	afx_msg void OnRadioGenaddrWifiType1();
	afx_msg void OnRadioGenaddrWifiType2();
	afx_msg void OnBtnAssbtaddr();
	afx_msg void OnBtnAsswifiaddr();
	afx_msg void OnCheckApVer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetWifiAddrGenCtrlState(BOOL bState);
	void SetBtAddrGenCtrlState(BOOL bState);
	void SetCtrlStateForImei();
	WriteFlashPara*  m_pSetParam;
	CFont *m_pSetFont;
	CFont *m_pBtnFont;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMEISETDLG_H__8F23C93D_8158_4D5C_8A75_B26E8AEFFFF0__INCLUDED_)
