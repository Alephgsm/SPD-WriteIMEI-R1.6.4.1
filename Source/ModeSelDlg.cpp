// ModeSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "writeflash.h"
#include "ModeSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModeSelDlg dialog


CModeSelDlg::CModeSelDlg(BOOL *bOperMode, BOOL *bDisAutoUSB, BOOL *bRebootDUT, CWnd* pParent /*=NULL*/)
	: CDialog(CModeSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModeSelDlg)
	m_strStaticNormalModeTips = _T("");
	m_strStaticWorkingModeTips = _T("");
	m_bOperMode = bOperMode;
	m_bDisAutoUSB = bDisAutoUSB;
	m_bRebootDUT = bRebootDUT;
	//}}AFX_DATA_INIT
}


void CModeSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModeSelDlg)
	DDX_Control(pDX, IDC_CHECK_DIS_AUTOUSB, m_ChkDisableAutoUSB);
	DDX_Radio(pDX, IDC_RADIO_MODE1, m_nMode);
	DDX_Text(pDX, IDC_STATIC_MODEDESRP_NORMAL, m_strStaticNormalModeTips);
	DDX_Text(pDX, IDC_STATIC_MODEDESRP_WROKING, m_strStaticWorkingModeTips);
	DDX_Control(pDX, IDC_CHECK_NOT_REBOOT_DUT, m_chkNotRebootDUT);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModeSelDlg, CDialog)
	//{{AFX_MSG_MAP(CModeSelDlg)
	ON_BN_CLICKED(IDC_RADIO_MODE2, OnRadioMode2)
	ON_BN_CLICKED(IDC_RADIO_MODE1, OnRadioMode1)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModeSelDlg message handlers

void CModeSelDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strErrTips(_T(""));
	if((NULL == m_bOperMode) || (NULL == m_bDisAutoUSB))
	{
		strErrTips.Format(IDS_ERRORSTR_UNKNOWN);
		AfxMessageBox(strErrTips);
		return;
	}

	UpdateData(TRUE);
	if(0 == m_nMode)
	{
		*m_bOperMode = TRUE;
		*m_bDisAutoUSB = (BOOL)(m_ChkDisableAutoUSB.GetCheck());
		strErrTips.Format(IDS_TIPSSTR_DISAUTOUSB);
		/*
		if(IDYES == AfxMessageBox(strErrTips, MB_YESNO))
		{
			*m_bDisAutoUSB = TRUE;
		}
		else
		{
			*m_bDisAutoUSB = FALSE;
		}
		*/
	}
	else
	{
		*m_bOperMode = FALSE;
		//teana hu 2012.12.06
		if(m_chkNotRebootDUT.GetCheck() == BST_CHECKED)
		{
			*m_bRebootDUT = FALSE;
		}
		else
		{
			*m_bRebootDUT = TRUE;
		}
		//
	}

	CDialog::OnOK();
}


BOOL CModeSelDlg::OnInitDialog() 
{
	if((NULL == m_bOperMode) || (NULL == m_bDisAutoUSB))
	{
		CString strErrTips(_T(""));
		strErrTips.Format(IDS_ERRORSTR_UNKNOWN);
		AfxMessageBox(strErrTips);
		return FALSE;
	}
	
    CDialog::OnInitDialog();
	
	CString strTips(_T(""));
	strTips.Format(IDS_MODEDISP_NEWMODE_TIPS);
	m_strStaticNormalModeTips  = strTips;
	strTips.Format(IDS_MODEDISP_OLDMODE_TIPS);
    m_strStaticWorkingModeTips = strTips;
	strTips.Format(IDS_AUTOCLOSEUSB);
	GetDlgItem(IDC_STATIC_AUTOUSBTIPS)->SetWindowText(strTips);

	m_pfont1 = new CFont(); 
    m_pfont1->CreateFont(18, 0, 0, 0, FW_NORMAL, TRUE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));  
    GetDlgItem(IDC_STATIC_AUTOUSBTIPS)->SetFont(m_pfont1);

	BOOL bConvDisAu = *m_bDisAutoUSB;
	if(bConvDisAu)
	{
		m_ChkDisableAutoUSB.SetCheck(1);
	}
	else
	{
		m_ChkDisableAutoUSB.SetCheck(0);
	}
	GetDlgItem(IDC_CHECK_DIS_AUTOUSB)->EnableWindow(FALSE);

	BOOL bConvMode = *m_bOperMode;
	if(bConvMode)
	{
		m_nMode = 0;
		//GetDlgItem(IDC_CHECK_DIS_AUTOUSB)->EnableWindow(TRUE);
	}
	else
	{
		m_nMode = 1;
		//GetDlgItem(IDC_CHECK_DIS_AUTOUSB)->EnableWindow(FALSE);
	}

	//teana hu 2012.12.06
	BOOL bRebootDUT = *m_bRebootDUT;
	if(m_nMode == 1)
	{
		m_chkNotRebootDUT.EnableWindow(TRUE);
		if(bRebootDUT)
		{
			m_chkNotRebootDUT.SetCheck(0);
		}
		else
		{
			m_chkNotRebootDUT.SetCheck(1);
		}
	}
	else
	{
		m_chkNotRebootDUT.EnableWindow(FALSE);
	}
	//
	
	UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CModeSelDlg::OnRadioMode2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(0 != m_nMode)
	{
		m_chkNotRebootDUT.EnableWindow(TRUE);
		//GetDlgItem(IDC_CHECK_DIS_AUTOUSB)->EnableWindow(FALSE);
	}
	else
	{
		m_chkNotRebootDUT.EnableWindow(FALSE);
		//GetDlgItem(IDC_CHECK_DIS_AUTOUSB)->EnableWindow(TRUE);
	}
}

void CModeSelDlg::OnRadioMode1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(0 != m_nMode)
	{
		m_chkNotRebootDUT.EnableWindow(TRUE);
	//	GetDlgItem(IDC_CHECK_DIS_AUTOUSB)->EnableWindow(FALSE);
	}
	else
	{
		m_chkNotRebootDUT.EnableWindow(FALSE);
	//	GetDlgItem(IDC_CHECK_DIS_AUTOUSB)->EnableWindow(TRUE);
	}
}

void CModeSelDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
    if(NULL!= m_pfont1)
    {
        m_pfont1->DeleteObject();
        delete m_pfont1;
        m_pfont1= NULL;
    }
	CDialog::OnClose();
}
