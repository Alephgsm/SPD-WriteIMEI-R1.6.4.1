// IMEISetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WriteFlash.h"
#include "IMEISetDlg.h"
#include "PhoneCommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIMEISetDlg dialog
//CWnd* pParent /*=NULL*/)

CIMEISetDlg::CIMEISetDlg(WriteFlashPara* pSetParam,CWnd* pParent /*=NULL*/)
: CDialog(CIMEISetDlg::IDD, pParent)
{
    _ASSERTE(NULL != pSetParam);
    //{{AFX_DATA_INIT(CIMEISetDlg)
    m_AutoIMEI = _T("");
	m_bCheckMdb = FALSE;
	m_bCheckTxt = FALSE;
	m_bCheckSave = FALSE;
	m_strVersion = _T("");
	m_nGenAddrTypeBT = -1;
	m_nGenAddrTypeWIFI = -1;
    m_pSetParam = pSetParam;
    m_pSetFont = NULL;
	m_pBtnFont = NULL;
	m_strInitAddrforBT = _T("");
	m_strInitAddrforWIFI = _T("");
	//}}AFX_DATA_INIT
}


void CIMEISetDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CIMEISetDlg)
	DDX_Control(pDX, IDC_EDIT_WIFIADDRPATH, m_edit_wifiaddrfilepath);
	DDX_Control(pDX, IDC_EDIT_BTADDRPATH, m_edit_btaddrfilepath);
	DDX_Control(pDX, IDC_CHECK_IMEI_ONREAD, m_ChkIMEIAfterRead);
	DDX_Control(pDX, IDC_COMBO_GENIMEI4TYPE, m_ComboGenTypeIMEI4);
	DDX_Control(pDX, IDC_COMBO_GENIMEI3TYPE, m_ComboGenTypeIMEI3);
	DDX_Control(pDX, IDC_COMBO_GENIMEI2TYPE, m_ComboGenTypeIMEI2);
	DDX_Control(pDX, IDC_COMBO_GENIMEI1TYPE, m_ComboGenTypeIMEI1);
	DDX_Control(pDX, IDC_CHECK_AUTOSN2, m_ChkAutoSN2);
	DDX_Control(pDX, IDC_CHECK_AUTOSN1, m_ChkAutoSN1);
	DDX_Control(pDX, IDC_CHECK_SN2, m_ChkSN2);
	DDX_Control(pDX, IDC_CHECK_SN1, m_ChkSN1);
	DDX_Control(pDX, IDC_CHECK_VER, m_CtrlCheckVer);
	DDX_Control(pDX, IDC_CHECK_WIFI, m_ChkWifi);
	DDX_Control(pDX, IDC_CHECK_AUTOWIFI, m_ChkAutoWifi);
	DDX_Control(pDX, IDC_CHECK_AUTOBT, m_ChkAutoBT);
	DDX_Control(pDX, IDC_CHECK_BT, m_ChkBT);
	DDX_Control(pDX, IDC_CHECK_IMEI1, m_ChkIMEI1);
	DDX_Control(pDX, IDC_CHECK_SAMEIMEI, m_ChkSameIMEI);
	DDX_Control(pDX, IDC_CHECK_IMEI4, m_ChkIMEI4);
	DDX_Control(pDX, IDC_CHECK_IMEI3, m_ChkIMEI3);
	DDX_Control(pDX, IDC_CHECK_IMEI2, m_ChkIMEI2);
	DDX_Control(pDX, IDC_CHECK_MDB, m_CtrlSaveMdb);
	DDX_Control(pDX, IDC_CHECK_TXT, m_CtrlSaveTxt);
    DDX_Text(pDX, IDC_EDIT_AUTOIMEI, m_AutoIMEI);
    DDV_MaxChars(pDX, m_AutoIMEI, 14);
	DDX_Check(pDX, IDC_CHECK_MDB, m_bCheckMdb);
	DDX_Check(pDX, IDC_CHECK_TXT, m_bCheckTxt);
	DDX_Check(pDX, IDC_CHECK_SAVE, m_bCheckSave);
	DDX_Text(pDX, IDC_EDIT_VER, m_strVersion);
	DDV_MaxChars(pDX, m_strVersion, 256);
	DDX_Radio(pDX, IDC_RADIO_GENADDR_BT_TYPE1, m_nGenAddrTypeBT);
	DDX_Radio(pDX, IDC_RADIO_GENADDR_WIFI_TYPE1, m_nGenAddrTypeWIFI);
	DDX_Text(pDX, IDC_EDIT_INITADDR_BT, m_strInitAddrforBT);
	DDV_MaxChars(pDX, m_strInitAddrforBT, 12);
	DDX_Text(pDX, IDC_EDIT_INITADDR_WIFI, m_strInitAddrforWIFI);
	DDV_MaxChars(pDX, m_strInitAddrforWIFI, 12);
	DDX_Control(pDX, IDC_CHECK_AP_VER, m_chkAPVer);
	DDX_Text(pDX, IDC_EDIT_AP_VER, m_strAPVer);
	DDV_MaxChars(pDX, m_strAPVer, 300);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIMEISetDlg, CDialog)
//{{AFX_MSG_MAP(CIMEISetDlg)
    ON_BN_CLICKED(IDC_CHECK_IMEI1, OnCheckImei1)
    ON_BN_CLICKED(IDC_CHECK_IMEI2, OnCheckImei2)
    ON_BN_CLICKED(IDC_CHECK_IMEI3, OnCheckImei3)
    ON_BN_CLICKED(IDC_CHECK_BT, OnCheckBt)
    ON_WM_CLOSE()
    ON_EN_CHANGE(IDC_EDIT_AUTOIMEI, OnChangeEditAutoimei)
    ON_BN_CLICKED(IDC_CHECK_TXT, OnCheckTxt)
    ON_BN_CLICKED(IDC_CHECK_SAVE, OnCheckSave)
    ON_BN_CLICKED(IDC_CHECK_MDB, OnCheckMdb)
    ON_BN_CLICKED(IDC_CHECK_IMEI4, OnCheckImei4)
    ON_BN_CLICKED(IDC_CHECK_WIFI, OnCheckWifi)
    ON_BN_CLICKED(IDC_CHECK_VER, OnCheckVer)
    ON_BN_CLICKED(IDC_CHECK_SN1, OnCheckSn1)
    ON_BN_CLICKED(IDC_CHECK_SN2, OnCheckSn2)
    ON_BN_CLICKED(IDC_RADIO_GENADDR_BT_TYPE1, OnRadioGenaddrType1)
    ON_BN_CLICKED(IDC_RADIO_GENADDR_BT_TYPE2, OnRadioGenaddrType2)
	ON_BN_CLICKED(IDC_CHECK_SAMEIMEI, OnCheckSameimei)
	ON_BN_CLICKED(IDC_CHECK_AUTOBT, OnCheckAutobt)
	ON_BN_CLICKED(IDC_CHECK_AUTOWIFI, OnCheckAutowifi)
	ON_BN_CLICKED(IDC_RADIO_GENADDR_WIFI_TYPE1, OnRadioGenaddrWifiType1)
	ON_BN_CLICKED(IDC_RADIO_GENADDR_WIFI_TYPE2, OnRadioGenaddrWifiType2)
	ON_BN_CLICKED(IDC_BTN_ASSBTADDR, OnBtnAssbtaddr)
	ON_BN_CLICKED(IDC_BTN_ASSWIFIADDR, OnBtnAsswifiaddr)
	ON_BN_CLICKED(IDC_CHECK_AP_VER, OnCheckApVer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIMEISetDlg message handlers
void CIMEISetDlg::OnClose() 
{
    // TODO: Add your message handler code here and/or call default
    if(NULL!= m_pSetFont)
    {
        m_pSetFont->DeleteObject();
        delete m_pSetFont;
        m_pSetFont= NULL;
    }
	if(NULL!=m_pBtnFont)
	{
        m_pBtnFont->DeleteObject();
        delete m_pBtnFont;
        m_pBtnFont= NULL;
	}
    CDialog::OnClose();
}

BOOL CIMEISetDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    if(NULL == m_pSetParam)
    {
        return FALSE;
    }
    // TODO: Add extra initialization here
    m_pSetFont = new CFont(); 
    m_pSetFont->CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));  
    GetDlgItem(IDC_EDIT_AUTOIMEI)->SetFont(m_pSetFont);  
    GetDlgItem(IDC_EDIT_AUTOIMEI)->SetWindowText(m_pSetParam->sAutoIMEI);

	m_pBtnFont  = new CFont();
	m_pBtnFont->CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Century Gothic"));  
	GetDlgItem(IDOK)->SetFont(m_pBtnFont);  
	GetDlgItem(IDCANCEL)->SetFont(m_pBtnFont);  

    CButton *pButton; 
    m_ChkIMEI1.SetCheck(m_pSetParam->bChkIMEI);
    m_ChkIMEI2.SetCheck(m_pSetParam->bChkIMEI2);
    m_ChkIMEI3.SetCheck(m_pSetParam->bChkIMEI3);
    m_ChkIMEI4.SetCheck(m_pSetParam->bChkIMEI4);

//  ## zijian.zhu @ 20111212 for NEWMS00147831 <<<	
	int i = 0;
	CString strItem(_T(""));
	strItem.LoadString(IDS_GENCODETYPE_ITEM1);
	m_ComboGenTypeIMEI1.InsertString(i, strItem);
	m_ComboGenTypeIMEI2.InsertString(i, strItem);
	m_ComboGenTypeIMEI3.InsertString(i, strItem);
	m_ComboGenTypeIMEI4.InsertString(i++, strItem);
	strItem.LoadString(IDS_GENCODETYPE_ITEM2);
	m_ComboGenTypeIMEI1.InsertString(i, strItem);
	m_ComboGenTypeIMEI2.InsertString(i, strItem);
	m_ComboGenTypeIMEI3.InsertString(i, strItem);
	m_ComboGenTypeIMEI4.InsertString(i++, strItem);
	strItem.LoadString(IDS_GENCODETYPE_ITEM3);
	m_ComboGenTypeIMEI2.InsertString(i, strItem);
	m_ComboGenTypeIMEI3.InsertString(i, strItem);
	m_ComboGenTypeIMEI4.InsertString(i++, strItem);

	if(GENCODE_TYP_AUTADD == m_pSetParam->eGenIIMEI1)
	{
		m_pSetParam->eGenIIMEI1 = GENCODE_TYP_AUTGEN;
	}
	m_ComboGenTypeIMEI1.SetCurSel((int)(m_pSetParam->eGenIIMEI1));
	m_ComboGenTypeIMEI2.SetCurSel((int)(m_pSetParam->eGenIIMEI2));
	m_ComboGenTypeIMEI3.SetCurSel((int)(m_pSetParam->eGenIIMEI3));
	m_ComboGenTypeIMEI4.SetCurSel((int)(m_pSetParam->eGenIIMEI4));
	if(m_pSetParam->bChkUseSameIMEI)
	{
		m_ComboGenTypeIMEI2.EnableWindow(FALSE);
		m_ComboGenTypeIMEI3.EnableWindow(FALSE);
		m_ComboGenTypeIMEI4.EnableWindow(FALSE);
	}
	else
	{
		SetCtrlStateForImei();
	}
//  ## zijian.zhu @ 20111212 for NEWMS00147831 >>>

    m_ChkSameIMEI.SetCheck( m_pSetParam->bChkUseSameIMEI);
    m_ChkBT.SetCheck( m_pSetParam->bChkBTAddr); 
	m_ChkWifi.SetCheck( m_pSetParam->bCheckWifi);
    pButton = (CButton *)GetDlgItem(IDC_CHECK_PHASE);
    pButton->SetCheck( m_pSetParam->bChkPhase);

    if(m_pSetParam->bChkBTAddr)
    {
        m_ChkAutoBT.SetCheck( m_pSetParam->bChkAutoCreateBT);
    }
    else
    {
        m_ChkAutoBT.EnableWindow(FALSE);
    } 
    if(m_pSetParam->bCheckWifi)
    {
        m_ChkAutoWifi.SetCheck( m_pSetParam->bChkAutoCreateWifi);
    }
    else
    {
        m_ChkAutoWifi.EnableWindow(FALSE);
    } 
    if(!m_pSetParam->bChkIMEI)
    {
       m_ChkIMEI2.EnableWindow(FALSE);
       m_ChkIMEI3.EnableWindow(FALSE);
       m_ChkIMEI4.EnableWindow(FALSE);
       m_ChkSameIMEI.EnableWindow(FALSE);
    }
    if((!m_pSetParam->bChkIMEI2)&&(!m_pSetParam->bChkIMEI3)&&(!m_pSetParam->bChkIMEI4))
    {
        m_ChkSameIMEI.EnableWindow(FALSE);
    }
    if(!m_pSetParam->bChkAutoIMEI)
    {
       GetDlgItem(IDC_EDIT_AUTOIMEI)->EnableWindow(FALSE);
    }
    if(0==m_pSetParam->nMocver)
    {
        pButton = (CButton *)GetDlgItem(IDC_CHECK_PHASE);
        pButton->SetCheck(BST_UNCHECKED);
        GetDlgItem(IDC_CHECK_PHASE)->EnableWindow(FALSE);
        m_pSetParam->bChkPhase =0;
    }
    if(m_pSetParam->bSaveIMEI)
    {
       m_bCheckSave = TRUE;
    }
    else
    {
       m_CtrlSaveMdb.EnableWindow(FALSE);
       m_CtrlSaveTxt.EnableWindow(FALSE);
    }
    if(m_bCheckSave && m_pSetParam->bSaveToTxt)
    {
        m_bCheckTxt = TRUE;
        m_CtrlSaveMdb.EnableWindow(FALSE);
    }
    else if(m_pSetParam->bSaveIMEI)
    {
        m_bCheckMdb = TRUE;
        m_CtrlSaveTxt.EnableWindow(FALSE);
    }
    
    pButton = (CButton *)GetDlgItem(IDC_CHECK_VALID);
    pButton->SetCheck( m_pSetParam->bCheckValid);
    
    pButton = (CButton *)GetDlgItem(IDC_CHECK_REPEAT);
    pButton->SetCheck( m_pSetParam->bCheckRepeat);  
    if(m_pSetParam->bCheckVer)
    {
        m_CtrlCheckVer.SetCheck(BST_CHECKED);
        m_strVersion = m_pSetParam->sVersion;
    }
    else
    {
         GetDlgItem(IDC_EDIT_VER)->EnableWindow(FALSE);
    }
	
	m_strInitAddrforBT = m_pSetParam->strInitAddrForBT;
	m_strInitAddrforWIFI = m_pSetParam->strInitAddrForWifi;

	BOOL bState = m_pSetParam->bChkBTAddr && m_pSetParam->bChkAutoCreateBT;
	SetBtAddrGenCtrlState(bState);
	m_nGenAddrTypeBT = (int)(m_pSetParam->bGenBtAddrType);
	if(bState)
	{
		GetDlgItem(IDC_EDIT_INITADDR_BT)->EnableWindow(m_nGenAddrTypeBT);
	}
	
	bState = m_pSetParam->bCheckWifi && m_pSetParam->bChkAutoCreateWifi;	
	SetWifiAddrGenCtrlState(bState);
	m_nGenAddrTypeWIFI = (int)(m_pSetParam->bGenWifiMacType);
	if(bState)
	{
		GetDlgItem(IDC_EDIT_INITADDR_WIFI)->EnableWindow(m_nGenAddrTypeWIFI);
	}

//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
    if(!(m_pSetParam->bEnableSNFunc))
    {
		int nheigredu = 0;
		CRect rectCtrl;
		
		GetDlgItem(IDC_STATIC_SNFRAME)->GetWindowRect(&rectCtrl);
		nheigredu = rectCtrl.top;
		GetDlgItem(IDC_STATIC_VERSIONFRAME)->GetWindowRect(&rectCtrl);
		nheigredu -= rectCtrl.top;
		
		GetDlgItem(IDC_STATIC_SNFRAME)->ShowWindow(FALSE);
		GetDlgItem(IDC_CHECK_SN1)->ShowWindow(FALSE);
		GetDlgItem(IDC_CHECK_AUTOSN1)->ShowWindow(FALSE);
		GetDlgItem(IDC_CHECK_SN2)->ShowWindow(FALSE);
		GetDlgItem(IDC_CHECK_AUTOSN2)->ShowWindow(FALSE);

		int nMoveIDList[2] = { IDOK, IDCANCEL };
		for(int i=0; i<2; i++)
		{
			GetDlgItem(nMoveIDList[i])->GetWindowRect(&rectCtrl);
			rectCtrl.top -= nheigredu;
			rectCtrl.bottom -= nheigredu;
			ScreenToClient(&rectCtrl);
			GetDlgItem(nMoveIDList[i])->MoveWindow(&rectCtrl);
		}
		
		this->GetClientRect(&rectCtrl);
		rectCtrl.bottom -= (nheigredu - 30);
		MoveWindow(&rectCtrl);
    }
	else
	{
		m_ChkSN1.SetCheck(m_pSetParam->bChkSN1);
		if(m_pSetParam->bChkSN1)
		{
			m_ChkAutoSN1.SetCheck(m_pSetParam->bChkAutoSN1);
		}
		else
		{
			m_ChkAutoSN1.SetCheck(BST_UNCHECKED);
			m_ChkAutoSN1.EnableWindow(FALSE);
		}
		
		m_ChkSN2.SetCheck(m_pSetParam->bChkSN2);
		if(m_pSetParam->bChkSN2)
		{
			m_ChkAutoSN2.SetCheck(m_pSetParam->bChkAutoSN2);
		}
		else
		{
			m_ChkAutoSN2.SetCheck(BST_UNCHECKED);
			m_ChkAutoSN2.EnableWindow(FALSE);
		}
	}
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>

	if(m_pSetParam->bChkIMEIAfterRead)
	{
		m_ChkIMEIAfterRead.SetCheck(1);
	}
	else
	{
		m_ChkIMEIAfterRead.SetCheck(0);
	}

//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  {{{
//
	m_edit_btaddrfilepath.SetWindowText(m_pSetParam->strBtAddrPath);
	m_edit_wifiaddrfilepath.SetWindowText(m_pSetParam->strWifiAddrPath);

	BOOL bEnStat = m_pSetParam->bManualAssignBtAddr && m_pSetParam->bChkBTAddr;
	GetDlgItem(IDC_STATIC_BTADAS)->EnableWindow(bEnStat);
	GetDlgItem(IDC_EDIT_BTADDRPATH)->EnableWindow(bEnStat);
	GetDlgItem(IDC_BTN_ASSBTADDR)->EnableWindow(bEnStat);
	if(bEnStat)
	{
		GetDlgItem(IDC_RADIO_GENADDR_BT_TYPE1)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));
		GetDlgItem(IDC_RADIO_GENADDR_BT_TYPE2)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));	
		GetDlgItem(IDC_EDIT_INITADDR_BT)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));
		GetDlgItem(IDC_CHECK_AUTOBT)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));	
	}

	bEnStat = m_pSetParam->bManualAssignWifiAddr && m_pSetParam->bCheckWifi;
	GetDlgItem(IDC_STATIC_WIFIADAS)->EnableWindow(bEnStat);
	GetDlgItem(IDC_EDIT_WIFIADDRPATH)->EnableWindow(bEnStat);
	GetDlgItem(IDC_BTN_ASSWIFIADDR)->EnableWindow(bEnStat);
	if(bEnStat)
	{
		GetDlgItem(IDC_RADIO_GENADDR_WIFI_TYPE1)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));
		GetDlgItem(IDC_RADIO_GENADDR_WIFI_TYPE2)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));	
		GetDlgItem(IDC_EDIT_INITADDR_WIFI)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));		
		GetDlgItem(IDC_CHECK_AUTOWIFI)->EnableWindow(!(m_pSetParam->bManualAssignBtAddr));	
	}

//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  }}}
//

	//teana hu 2012.10.16
	m_chkAPVer.SetCheck(m_pSetParam->bChkAPVer);
	GetDlgItem(IDC_EDIT_AP_VER)->EnableWindow(m_pSetParam->bChkAPVer);
	m_strAPVer = m_pSetParam->strAPVerInput;
	
	//

    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CIMEISetDlg::OnOK() 
{
    // TODO: Add extra validation here
    if(NULL==m_pSetParam)
    {
        return ;
    }
    UpdateData();
    CButton *pButton ;
    m_pSetParam->bChkIMEI =m_ChkIMEI1.GetCheck();
    m_pSetParam->bChkIMEI2 = m_ChkIMEI2.GetCheck();
    m_pSetParam->bChkIMEI3 = m_ChkIMEI3.GetCheck();
    m_pSetParam->bChkIMEI4 = m_ChkIMEI4.GetCheck();
    m_pSetParam->bChkUseSameIMEI = m_ChkSameIMEI.GetCheck();

//  ## zijian.zhu @ 20111212 for NEWMS00147831 <<<
	if(m_pSetParam->bChkIMEI)
	{
		m_pSetParam->eGenIIMEI1 = (GCODIMEI_E)(m_ComboGenTypeIMEI1.GetCurSel());
	}
	if(!(m_pSetParam->bChkUseSameIMEI))
	{
		if(m_pSetParam->bChkIMEI2)
		{
			m_pSetParam->eGenIIMEI2 = (GCODIMEI_E)(m_ComboGenTypeIMEI2.GetCurSel());
		}
		if(m_pSetParam->bChkIMEI3)
		{
			m_pSetParam->eGenIIMEI3 = (GCODIMEI_E)(m_ComboGenTypeIMEI3.GetCurSel());
		}
		if(m_pSetParam->bChkIMEI4)
		{
			m_pSetParam->eGenIIMEI4 = (GCODIMEI_E)(m_ComboGenTypeIMEI4.GetCurSel());
		}
	}
//  ## zijian.zhu @ 20111212 for NEWMS00147831 >>>

    m_pSetParam->bChkBTAddr = m_ChkBT.GetCheck();
    m_pSetParam->bChkAutoCreateBT = m_ChkAutoBT.GetCheck();
	m_pSetParam->bGenBtAddrType = m_nGenAddrTypeBT;
    m_pSetParam->bCheckWifi = m_ChkWifi.GetCheck();
    m_pSetParam->bChkAutoCreateWifi = m_ChkAutoWifi.GetCheck();
	m_pSetParam->bGenWifiMacType = m_nGenAddrTypeWIFI;
    
    pButton = (CButton *)GetDlgItem(IDC_CHECK_PHASE);
    m_pSetParam->bChkPhase = pButton->GetCheck();
    pButton = (CButton *)GetDlgItem(IDC_CHECK_VALID);
    m_pSetParam->bCheckValid =  pButton->GetCheck();
    pButton = (CButton *)GetDlgItem(IDC_CHECK_REPEAT);
    m_pSetParam->bCheckRepeat =  pButton->GetCheck();

    m_pSetParam->sAutoIMEI = m_AutoIMEI;
    if( m_pSetParam->bChkAutoIMEI)
    { 
        if(14 != m_AutoIMEI.GetLength())
        {
            AfxMessageBox(_T("请输入14位起始IMEI"));
            return;
        }
        for(int i = 0; i < 14; i ++)
        {
            if(m_AutoIMEI.GetAt(i) < '0' || m_AutoIMEI.GetAt(i) > '9')
            {
                AfxMessageBox(_T("IMEI必须为0~9数字"));
                return;
            }
        }
    }
    m_pSetParam->bSaveIMEI = m_bCheckSave;
    m_pSetParam->bSaveToTxt = m_bCheckTxt;
    m_pSetParam->bCheckVer = m_CtrlCheckVer.GetCheck();
   
    m_strVersion.TrimLeft();
    m_strVersion.TrimRight();
    if(m_pSetParam->bCheckVer&&m_strVersion.IsEmpty())
    {
       AfxMessageBox(_T("If check version，please input checked version"));
       return;
    }
	if(m_pSetParam->bCheckVer&&(m_pSetParam->nVerLenLowLimit>=m_strVersion.GetLength()))
	{
		CString strVerLenTip(_T(""));
		strVerLenTip.Format(IDS_SET_VERSIONLENGTH_ERR, m_pSetParam->nVerLenLowLimit);
		AfxMessageBox(strVerLenTip);
		return;
	}
    m_pSetParam->sVersion = m_strVersion;

	m_strInitAddrforBT.MakeLower();
	m_pSetParam->strInitAddrForBT = m_strInitAddrforBT;
	m_strInitAddrforWIFI.MakeLower();
	m_pSetParam->strInitAddrForWifi = m_strInitAddrforWIFI;
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	if(m_pSetParam->bEnableSNFunc)
	{
		m_pSetParam->bChkSN1 = m_ChkSN1.GetCheck();
		if(m_pSetParam->bChkSN1)
		{
	    	m_pSetParam->bChkAutoSN1 = m_ChkAutoSN1.GetCheck();
		}
	    else
		{
			m_pSetParam->bChkAutoSN1 = FALSE;
		}

		m_pSetParam->bChkSN2 = m_ChkSN2.GetCheck();
		if(m_pSetParam->bChkSN2)
		{
			m_pSetParam->bChkAutoSN2 = m_ChkAutoSN2.GetCheck();
		}
		else
		{
			m_pSetParam->bChkAutoSN2 = FALSE;
		}
	}
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>


//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  {{{
//
	m_pSetParam->strBtAddrPath.Empty();
	m_edit_btaddrfilepath.GetWindowText(m_pSetParam->strBtAddrPath);
	m_pSetParam->strWifiAddrPath.Empty();
	m_edit_wifiaddrfilepath.GetWindowText(m_pSetParam->strWifiAddrPath);
//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  }}}
//

	//teana hu 2012.10.16
	if(m_chkAPVer.GetCheck() == BST_CHECKED)
	{
		if(m_strAPVer.GetLength() < 5)
		{
			::AfxMessageBox(_T("Check AP Version设置的字符必须大于5个"));
			return;
		}
		m_pSetParam->bChkAPVer = TRUE;
		m_pSetParam->strAPVerInput = m_strAPVer;
	}
	else
	{
		m_pSetParam->bChkAPVer = FALSE;
	}
	//

	if(1 == m_ChkIMEIAfterRead.GetCheck())
	{
		m_pSetParam->bChkIMEIAfterRead = TRUE;
	}
	else
	{
		m_pSetParam->bChkIMEIAfterRead = FALSE;
	}

    if(NULL!= m_pSetFont)
    {
        m_pSetFont->DeleteObject();
        delete m_pSetFont;
        m_pSetFont= NULL;
    }
	if(NULL!=m_pBtnFont)
	{
        m_pBtnFont->DeleteObject();
        delete m_pBtnFont;
        m_pBtnFont= NULL;
	}
    CDialog::OnOK();
}

void CIMEISetDlg::OnCheckImei1() 
{
    // TODO: Add your control notification handler code here
    UpdateData();   
    if(!m_ChkIMEI1.GetCheck())
    {
        m_ChkIMEI2.EnableWindow(FALSE);
        m_ChkIMEI3.EnableWindow(FALSE);
        m_ChkIMEI4.EnableWindow(FALSE);
        m_ChkSameIMEI.EnableWindow(FALSE);
		
        m_ChkIMEI2.SetCheck(BST_UNCHECKED);
        m_ChkIMEI3.SetCheck(BST_UNCHECKED);
        m_ChkIMEI4.SetCheck(BST_UNCHECKED);
        m_ChkSameIMEI.SetCheck(BST_UNCHECKED);
    }
    else
    {
        m_ChkIMEI2.EnableWindow(TRUE);
        m_ChkIMEI3.EnableWindow(TRUE);
        m_ChkIMEI4.EnableWindow(TRUE);
    }
	SetCtrlStateForImei();
}

void CIMEISetDlg::OnCheckImei2() 
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if(!m_ChkIMEI2.GetCheck())
    {    
        if((0==m_ChkIMEI3.GetCheck())&& (0==m_ChkIMEI4.GetCheck()))
        {
            m_ChkSameIMEI.EnableWindow(FALSE);
            m_ChkSameIMEI.SetCheck(BST_UNCHECKED);
        }
    }
    else
    {
		m_ChkSameIMEI.EnableWindow(TRUE);
    }
	SetCtrlStateForImei();
}

void CIMEISetDlg::OnCheckImei3() 
{
    // TODO: Add your control notification handler code here
    UpdateData();
    if(!m_ChkIMEI3.GetCheck())
    {    
        if((0==m_ChkIMEI2.GetCheck())&& (0==m_ChkIMEI4.GetCheck()))
        {
            m_ChkSameIMEI.EnableWindow(FALSE);
            m_ChkSameIMEI.SetCheck(BST_UNCHECKED);
        }
    }
    else
    {
        m_ChkSameIMEI.EnableWindow(TRUE);
    }
	SetCtrlStateForImei();
}
 
void CIMEISetDlg::OnCheckImei4() 
{
	// TODO: Add your control notification handler code here
    UpdateData();
    if(!m_ChkIMEI4.GetCheck())
    {
        if((0==m_ChkIMEI2.GetCheck())&& (0==m_ChkIMEI3.GetCheck()))
        {
            m_ChkSameIMEI.EnableWindow(FALSE);
            m_ChkSameIMEI.SetCheck(BST_UNCHECKED);
        }
    }
    else
    {
        m_ChkSameIMEI.EnableWindow(TRUE);
    }
	SetCtrlStateForImei();
}

void CIMEISetDlg::OnCheckWifi() 
{
    // TODO: Add your control notification handler code here
    UpdateData();

	if(!(m_pSetParam->bManualAssignWifiAddr))
	{
		if(!m_ChkWifi.GetCheck())
		{
			m_ChkAutoWifi.EnableWindow(FALSE);   
			m_ChkAutoWifi.SetCheck(BST_UNCHECKED);
		}
		else
		{
			m_ChkAutoWifi.EnableWindow(TRUE);   
		}
		OnCheckAutowifi();
	}
	else
	{
		BOOL bEnable = (1==m_ChkWifi.GetCheck())?TRUE:FALSE;
		GetDlgItem(IDC_STATIC_WIFIADAS)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_WIFIADDRPATH)->EnableWindow(bEnable);
		GetDlgItem(IDC_BTN_ASSWIFIADDR)->EnableWindow(bEnable);
	}
}

void CIMEISetDlg::OnCheckBt() 
{
    // TODO: Add your control notification handler code here
    UpdateData();

	if(!(m_pSetParam->bManualAssignBtAddr))
	{
		if(!m_ChkBT.GetCheck())
		{
			m_ChkAutoBT.EnableWindow(FALSE);   
			m_ChkAutoBT.SetCheck(BST_UNCHECKED);      
		}
		else
		{
			m_ChkAutoBT.EnableWindow(TRUE);   
		}
		OnCheckAutobt();
	}
	else
	{
		BOOL bEnable = (1==m_ChkBT.GetCheck())?TRUE:FALSE;
		GetDlgItem(IDC_STATIC_BTADAS)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_BTADDRPATH)->EnableWindow(bEnable);
		GetDlgItem(IDC_BTN_ASSBTADDR)->EnableWindow(bEnable);
	}
}

void CIMEISetDlg::OnCancel() 
{
    // TODO: Add extra cleanup here
    if(NULL!= m_pSetFont)
    {
        m_pSetFont->DeleteObject();
        delete m_pSetFont;
        m_pSetFont= NULL;
    }
	if(NULL!=m_pBtnFont)
	{
        m_pBtnFont->DeleteObject();
        delete m_pBtnFont;
        m_pBtnFont= NULL;
	}
    CDialog::OnCancel();
}

void CIMEISetDlg::OnChangeEditAutoimei() 
{
    CString strTmp ;
    GetDlgItem(IDC_EDIT_AUTOIMEI)->GetWindowText(strTmp);
    for(int i = 0; i < strTmp.GetLength(); i ++)
    {
        if(strTmp.GetAt(i) < '0' || strTmp.GetAt(i) > '9')
        {
            GetDlgItem(IDC_EDIT_AUTOIMEI)->SetWindowText(_T(""));
            return;
        }
    }
    UpdateData(); 	
}

void CIMEISetDlg::OnCheckTxt() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if(!m_bCheckTxt)
    {
        m_CtrlSaveMdb.EnableWindow(TRUE);
        m_bCheckMdb = TRUE;
        m_CtrlSaveTxt.EnableWindow(FALSE);
    }
    else
    {
        m_CtrlSaveMdb.EnableWindow(FALSE);
    }
    UpdateData(FALSE);
}

void CIMEISetDlg::OnCheckSave() 
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if(!m_bCheckSave)
    {
        m_bCheckTxt=FALSE;
        m_CtrlSaveTxt.EnableWindow(FALSE);
        m_bCheckMdb=FALSE;
        m_CtrlSaveMdb.EnableWindow(FALSE);
    }
    else
    {   
        m_CtrlSaveTxt.EnableWindow(TRUE);
        m_CtrlSaveMdb.EnableWindow(FALSE);
        m_bCheckTxt = TRUE;
        m_bCheckMdb = FALSE;
        
    }
    UpdateData(FALSE);
}

void CIMEISetDlg::OnCheckMdb() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    if(!m_bCheckMdb)
    {
         m_CtrlSaveTxt.EnableWindow(TRUE);
         m_bCheckTxt = TRUE;
         m_CtrlSaveMdb.EnableWindow(FALSE);
         
    }
    else
    {
        m_CtrlSaveTxt.EnableWindow(FALSE);
       
    }
    UpdateData(FALSE);
}

void CIMEISetDlg::OnCheckVer() 
{
	// TODO: Add your control notification handler code here
	if(m_CtrlCheckVer.GetCheck())
    {
        
       GetDlgItem(IDC_EDIT_VER)->EnableWindow(TRUE);
       if(NULL!=m_pSetParam)
       m_strVersion = m_pSetParam->sVersion;
       
    }
    else
    {
        GetDlgItem(IDC_EDIT_VER)->EnableWindow(FALSE);
       m_strVersion = "";
    }
    UpdateData(FALSE);
}

BOOL CIMEISetDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
    if( (pMsg -> hwnd == GetDlgItem(IDC_EDIT_INITADDR_BT) -> GetSafeHwnd()) ||
		(pMsg -> hwnd == GetDlgItem(IDC_EDIT_INITADDR_WIFI) -> GetSafeHwnd()) )
	{
		if(WM_CHAR == pMsg -> message)
		{
			if(!((pMsg -> wParam >= '0' && pMsg ->wParam <= '9') || 
				 (pMsg -> wParam >= 'A' && pMsg ->wParam <= 'F') || 
				 (pMsg -> wParam >= 'a' && pMsg ->wParam <= 'f') ||
				 (pMsg -> wParam == 8)))
			{
				return 1;
			}
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CIMEISetDlg::OnCheckSn1() 
{
	// TODO: Add your control notification handler code here
    UpdateData();
    if(!m_ChkSN1.GetCheck())
    {
        m_ChkAutoSN1.EnableWindow(FALSE);   
        m_ChkAutoSN1.SetCheck(BST_UNCHECKED);      
    }
    else
    {
		m_ChkAutoSN1.EnableWindow(TRUE);   
    }
}

void CIMEISetDlg::OnCheckSn2() 
{
	// TODO: Add your control notification handler code here
    UpdateData();
    if(!m_ChkSN2.GetCheck())
    {
        m_ChkAutoSN2.EnableWindow(FALSE);   
        m_ChkAutoSN2.SetCheck(BST_UNCHECKED);      
    }
    else
    {
		m_ChkAutoSN2.EnableWindow(TRUE);   
    }
}

void CIMEISetDlg::OnRadioGenaddrType1() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_INITADDR_BT)->EnableWindow(FALSE);
    CheckRadioButton(IDC_RADIO_GENADDR_BT_TYPE1, IDC_RADIO_GENADDR_BT_TYPE2, IDC_RADIO_GENADDR_BT_TYPE1);
}

void CIMEISetDlg::OnRadioGenaddrType2() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_INITADDR_BT)->EnableWindow(TRUE);
    CheckRadioButton(IDC_RADIO_GENADDR_BT_TYPE1, IDC_RADIO_GENADDR_BT_TYPE2, IDC_RADIO_GENADDR_BT_TYPE2);
}

void CIMEISetDlg::OnCheckSameimei() 
{
	// TODO: Add your control notification handler code here
	this->UpdateData();
	if(1 == m_ChkSameIMEI.GetCheck())
	{
		m_ComboGenTypeIMEI2.EnableWindow(FALSE);
		m_ComboGenTypeIMEI3.EnableWindow(FALSE);
		m_ComboGenTypeIMEI4.EnableWindow(FALSE);
	}
	else
	{
		SetCtrlStateForImei();
	}
}

void CIMEISetDlg::SetCtrlStateForImei()
{
	this->UpdateData();
	if(!m_ChkIMEI1.GetCheck())
	{
		m_ComboGenTypeIMEI1.EnableWindow(FALSE);
	}
	else
	{
		m_ComboGenTypeIMEI1.EnableWindow(TRUE);
	}

	if(!m_ChkIMEI2.GetCheck())
	{
		m_ComboGenTypeIMEI2.EnableWindow(FALSE);
	}
	else
	{
		m_ComboGenTypeIMEI2.EnableWindow(TRUE);
	}

	if(!m_ChkIMEI3.GetCheck())
	{
		m_ComboGenTypeIMEI3.EnableWindow(FALSE);
	}
	else
	{
		m_ComboGenTypeIMEI3.EnableWindow(TRUE);
	}

	if(!m_ChkIMEI4.GetCheck())
	{
		m_ComboGenTypeIMEI4.EnableWindow(FALSE);
	}
	else
	{
		m_ComboGenTypeIMEI4.EnableWindow(TRUE);
	}

	this->UpdateData(FALSE);
}

void CIMEISetDlg::SetBtAddrGenCtrlState(BOOL bState)
{
	GetDlgItem(IDC_STATIC_GENADDRTYPE_BT)->EnableWindow(bState);
	GetDlgItem(IDC_RADIO_GENADDR_BT_TYPE1)->EnableWindow(bState);
	GetDlgItem(IDC_RADIO_GENADDR_BT_TYPE2)->EnableWindow(bState);
	BOOL bEn = (bState && 1==m_nGenAddrTypeBT) ? TRUE : FALSE;
	GetDlgItem(IDC_EDIT_INITADDR_BT)->EnableWindow(bEn);
}

void CIMEISetDlg::SetWifiAddrGenCtrlState(BOOL bState)
{
	GetDlgItem(IDC_STATIC_GENADDRTYPE_WIFI)->EnableWindow(bState);
	GetDlgItem(IDC_RADIO_GENADDR_WIFI_TYPE1)->EnableWindow(bState);
	GetDlgItem(IDC_RADIO_GENADDR_WIFI_TYPE2)->EnableWindow(bState);
	BOOL bEn = (bState && 1==m_nGenAddrTypeWIFI) ? TRUE : FALSE;
	GetDlgItem(IDC_EDIT_INITADDR_WIFI)->EnableWindow(bEn);
}

void CIMEISetDlg::OnCheckAutobt() 
{
	// TODO: Add your control notification handler code here
	this->UpdateData();
	BOOL bState = (BOOL)(m_ChkAutoBT.GetCheck());
	SetBtAddrGenCtrlState(bState);
}

void CIMEISetDlg::OnCheckAutowifi() 
{
	// TODO: Add your control notification handler code here
	this->UpdateData();
	BOOL bState = (BOOL)(m_ChkAutoWifi.GetCheck());
	SetWifiAddrGenCtrlState(bState);
}

void CIMEISetDlg::OnRadioGenaddrWifiType1() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_INITADDR_WIFI)->EnableWindow(FALSE);
}

void CIMEISetDlg::OnRadioGenaddrWifiType2() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_INITADDR_WIFI)->EnableWindow(TRUE);
}

void CIMEISetDlg::OnBtnAssbtaddr() 
{
	// TODO: Add your control notification handler code here

	CString strPath(_T(""));
	CFileDialog dlgFile(TRUE,NULL,NULL,OFN_HIDEREADONLY,_T("Text Files (*.txt;*.ini)|*.txt; *.ini|"),NULL);
	if(dlgFile.DoModal()==IDOK)
	{
		strPath = dlgFile.GetPathName();
		m_edit_btaddrfilepath.SetWindowText(strPath);
	}
	
}

void CIMEISetDlg::OnBtnAsswifiaddr() 
{
	// TODO: Add your control notification handler code here
	
	CString strPath(_T(""));
	CFileDialog dlgFile(TRUE,NULL,NULL,OFN_HIDEREADONLY,_T("Text Files (*.txt;*.ini)|*.txt; *.ini|"),NULL);
	if(dlgFile.DoModal()==IDOK)
	{
		strPath = dlgFile.GetPathName();
		m_edit_wifiaddrfilepath.SetWindowText(strPath);
	}
}

void CIMEISetDlg::OnCheckApVer() 
{
	// TODO: Add your control notification handler code here
	if(m_chkAPVer.GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_AP_VER)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_AP_VER)->EnableWindow(FALSE);
	}
	
}
