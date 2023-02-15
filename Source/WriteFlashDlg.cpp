// WriteFlashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WriteFlash.h"
#include "WriteFlashDlg.h"
#include "IMEISetDlg.h"
#include "PhoneCommand.h"
#include "ModeSelDlg.h"
#include <afxconv.h>
#include <devguid.h>
#include <math.h>
//#include <assert.h>
#include "Splog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWriteFlashApp theApp;
extern SP_HANDLE      g_hDiagPhone;

#define  COLOR_IDLE      RGB(100, 150, 200)  
#define  COLOR_WORKING   RGB(255, 255, 000) 
#define  COLOR_PASS      RGB(000, 128, 000)
#define  COLOR_FAIL      RGB(255, 000, 000)
#define  COLOR_PGREEN    RGB(003, 253, 027)
#define  COLOR_ORANGE    RGB(254, 168, 001)

#define WM_REFRESH_UI  (WM_USER + 0x111)
#define RFT_SUCCESS     0
#define PC_ERR_WRITE_IMEI_REJECT 39

#define STATIC_REF_RATE  300
#define WIMEI_SN_LENGTH_MAXIMIZE  (0x18)
#define WIMEI_SN_LENGTH_MINIMIZE  (0x0E)

/////////////////////////////////////////////////////////////////////////////
// 
BOOL WINAPI GetPrivateProfileBool(LPCTSTR lpAppName, LPCTSTR lpKeyName, BOOL bDefault, LPCTSTR lpFileName)
{
    ASSERT( NULL != lpAppName );
    ASSERT( NULL != lpKeyName );
    ASSERT( NULL != lpFileName );
    
    int iVal = ::GetPrivateProfileInt(lpAppName, lpKeyName, (bDefault ? 1 : 0), lpFileName);
    return (1 == iVal) ? TRUE : FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();
    
    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA
    
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
    
    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteFlashDlg dialog

CWriteFlashDlg::CWriteFlashDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWriteFlashDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWriteFlashDlg)
    m_strEditIMEI1 = _T("");
    m_strEditIMEI2 = _T("");
    m_strEditIMEI3 = _T("");
    m_strMsg = _T("");
    m_strEditBT = _T("");
	m_strPass = _T("");
	m_strFail = _T("");
	m_strYield = _T("");
	m_strEditIMEI4 = _T("");
	m_strWifi = _T("");
	m_strSN1 = _T("");
	m_strSN2 = _T("");
	//}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_nComPort = -1;
	m_nResvComPort = -1;
    m_bUsbMode = FALSE;
    m_int64DefaultIMEI=0;
    ZeroMemory(m_szIniPath,sizeof(m_szIniPath));
    ZeroMemory(m_szIMEITxtPath,sizeof(m_szIMEITxtPath));
    ZeroMemory(m_szIMEIMdbPath,sizeof(m_szIMEIMdbPath));
    ZeroMemory(m_szStationName,sizeof(m_szStationName));
    ZeroMemory(m_szCheckStation,sizeof(m_szCheckStation));
    ZeroMemory(m_szCurrentStation,sizeof(m_szCurrentStation));
    //ZeroMemory(&m_WriteParam,sizeof(m_WriteParam));
    m_nTimeout =3000;
    m_dwEnterModeWait= 10000;
    m_nWriteTime = 10000;
    m_bAdminMode = 0;
    m_nStationIndex= 3;
    m_bUsbMode=0;
    m_hEnterModeThread = NULL;
    m_hEnterModeSuccess = NULL;
    m_hExitThread = NULL;
//    m_bCheck =0;
    m_bReadFun = 0;
    //m_bNewVer =0;
    m_pFont = NULL;
    m_pFont1= NULL;
	m_pFontPortInd = NULL;
    m_hPhaseCheckObj= NULL;
    m_nTestCounts = 0;
    m_nActualCounts =0;
    m_bUseDcs =0;
    m_pDcSource = NULL;
    m_pDcsFactory = NULL;
    m_nTestMode = 0;
    m_dwStartTime =0;
    m_bWriteTime =0;
    CreateDCSourceFactory(&m_pDcsFactory);
    m_bEnter = FALSE;
    m_nPassCounts = 0;
    m_nFailCounts = 0;
    m_pRecordset = NULL;
    m_pConnection = NULL;
	m_bOperationMethod = FALSE;
	m_bPhoneConnState = FALSE;
	m_bDisableAutoUSB = FALSE;
	m_bConnState = FALSE;
	m_hVerfConn2Test = NULL;
	m_dwVerfConn2Test = NULL;
	m_bPortCmbAddIndi = FALSE;
	m_bCrtTimer = FALSE;
	m_nSNLength = 14;
	m_int64WifiAddrForMinimize = 0;
	m_int64WifiAddrForMaximize = 0;
	m_int64BtAddrForMinimize = 0;
	m_int64BtAddrForMaximize = 0;
	m_bCompUpgIndiStatic = FALSE;
	m_bPreReadImeiCount = FALSE;
	m_nDisplayLabelFontSize1 = 0;
	m_nDisplayLabelFontSize2 = 0;
	m_bIsTesting = FALSE;
    
    m_bCheckBTInput = FALSE;
    m_bCheckWFInput = FALSE;
    ZeroMemory((void *)&m_szFmtBT, sizeof(m_szFmtBT));
    ZeroMemory((void *)&m_szFmtWF, sizeof(m_szFmtWF));

	m_nScannerKey_OnWrite = VK_RETURN;
	m_nScannerKey_OnRead = VK_SPACE;

    ZeroMemory((void *)&m_CustomBtAddrComp, sizeof(m_CustomBtAddrComp));
    ZeroMemory((void *)&m_CustomWifiAddrComp, sizeof(m_CustomWifiAddrComp));

	m_bCustAddrInvalid = TRUE;
	m_bRebootDUT = TRUE;
	m_nDUTPort = -1;
	m_bOperateCURef = FALSE;
	ZeroMemory(&m_bBuf, CU_REFERENCE_LEN);
}

void CWriteFlashDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWriteFlashDlg)
	DDX_Control(pDX, IDC_EDIT_SNSENIOR, m_CEditSN2);
	DDX_Control(pDX, IDC_EDIT_SNMAIN, m_CEditSN1);
	DDX_Control(pDX, IDC_STATIC_MODEDISP, m_StaticModeDisp);
	DDX_Control(pDX, IDC_COMBO_FORPORT, m_cmbFormalPort);
	DDX_Control(pDX, IDC_STATIC_PORTINDICATOR, m_StaticPortIndicator);
	DDX_Control(pDX, IDC_EDIT_WIFI, m_CEditWifi);
	DDX_Control(pDX, IDC_EDIT_IMEIFOUR, m_CEditFourIMEI);
	DDX_Control(pDX, IDC_STATIC_PASSYIELD, m_StaticYield);
	DDX_Control(pDX, IDC_STATIC_FAIL, m_StaticFail);
	DDX_Control(pDX, IDC_STATIC_PASS, m_StaticPass);
    DDX_Control(pDX, IDC_EDIT_IMEISENIOR, m_CEditSecIMEI);
    DDX_Control(pDX, IDC_EDIT_IMEIMAIN, m_CEditMainIMEI);
    DDX_Control(pDX, IDC_EDIT_IMEITHIRD,m_CEditThirdIMEI);
    DDX_Control(pDX, IDC_EDIT_BT, m_CEditBT);
    DDX_Control(pDX, IDC_STATIC_MESSAGE, m_StaticMsg);
    DDX_Text(pDX, IDC_EDIT_IMEIMAIN, m_strEditIMEI1);
    DDV_MaxChars(pDX, m_strEditIMEI1, 15);
    DDX_Text(pDX, IDC_EDIT_IMEISENIOR, m_strEditIMEI2);
    DDV_MaxChars(pDX, m_strEditIMEI2, 15);
    DDX_Text(pDX, IDC_EDIT_IMEITHIRD, m_strEditIMEI3);
    DDV_MaxChars(pDX, m_strEditIMEI3, 15);
    DDX_Text(pDX, IDC_STATIC_MESSAGE, m_strMsg);
    DDX_Text(pDX, IDC_EDIT_BT, m_strEditBT);
    DDV_MaxChars(pDX, m_strEditBT, 12);
	DDX_Text(pDX, IDC_STATIC_PASS, m_strPass);
	DDX_Text(pDX, IDC_STATIC_FAIL, m_strFail);
	DDX_Text(pDX, IDC_STATIC_PASSYIELD, m_strYield);
	DDX_Text(pDX, IDC_EDIT_IMEIFOUR, m_strEditIMEI4);
	DDV_MaxChars(pDX, m_strEditIMEI4, 15);
	DDX_Text(pDX, IDC_EDIT_WIFI, m_strWifi);
	DDV_MaxChars(pDX, m_strWifi, 12);
	DDX_Text(pDX, IDC_EDIT_SNMAIN, m_strSN1);
	DDX_Text(pDX, IDC_EDIT_SNSENIOR, m_strSN2);
	DDX_Text(pDX, IDC_EDIT_SW_VERSION, m_strVersion);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWriteFlashDlg, CDialog)
//{{AFX_MSG_MAP(CWriteFlashDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_MESSAGE(WM_REFRESH_UI, OnRefreshUI)
	ON_EN_CHANGE(IDC_EDIT_IMEIMAIN, OnChangeEditImeimain)
	ON_EN_CHANGE(IDC_EDIT_IMEISENIOR, OnChangeEditImeisenior)
	ON_BN_CLICKED(IDC_BUTTON_READ, OnButtonRead)
	ON_EN_CHANGE(IDC_EDIT_BT, OnChangeEditBt)
	ON_EN_CHANGE(IDC_EDIT_IMEITHIRD, OnChangeEditImeithird)
	ON_EN_CHANGE(IDC_EDIT_IMEIFOUR,OnChangeEditImeiFour)
	ON_EN_CHANGE(IDC_EDIT_WIFI, OnChangeEditWifi)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, OnBtnModeSel)
	ON_CBN_SELCHANGE(IDC_COMBO_FORPORT, OnSelchangeComboForport)
	ON_EN_CHANGE(IDC_EDIT_SNMAIN, OnChangeEditSnmain)
	ON_EN_CHANGE(IDC_EDIT_SNSENIOR, OnChangeEditSnsenior)
	ON_EN_CHANGE(IDC_EDIT_CUREF1, OnChangeEditCuref1)
	ON_EN_CHANGE(IDC_EDIT_CUREF2, OnChangeEditCuref2)
	ON_EN_CHANGE(IDC_EDIT_CUREF3, OnChangeEditCuref3)
	ON_EN_CHANGE(IDC_EDIT_CUREF4, OnChangeEditCuref4)
	ON_EN_CHANGE(IDC_EDIT_CUREF5, OnChangeEditCuref5)
	ON_EN_CHANGE(IDC_EDIT_CUREF6, OnChangeEditCuref6)
	ON_EN_CHANGE(IDC_EDIT_CUREF7, OnChangeEditCuref7)
	ON_EN_CHANGE(IDC_EDIT_CUREF8, OnChangeEditCuref8)
	ON_EN_CHANGE(IDC_EDIT_CUREF9, OnChangeEditCuref9)
	ON_EN_CHANGE(IDC_EDIT_CUREF10, OnChangeEditCuref10)
	ON_EN_CHANGE(IDC_EDIT_CUREF11, OnChangeEditCuref11)
	ON_EN_CHANGE(IDC_EDIT_CUREF12, OnChangeEditCuref12)
	ON_EN_CHANGE(IDC_EDIT_CUREF13, OnChangeEditCuref13)
	ON_EN_CHANGE(IDC_EDIT_CUREF14, OnChangeEditCuref14)
	ON_EN_CHANGE(IDC_EDIT_CUREF15, OnChangeEditCuref15)
	ON_EN_CHANGE(IDC_EDIT_CUREF16, OnChangeEditCuref16)
	ON_EN_CHANGE(IDC_EDIT_CUREF17, OnChangeEditCuref17)
	ON_EN_CHANGE(IDC_EDIT_CUREF18, OnChangeEditCuref18)
	ON_EN_CHANGE(IDC_EDIT_CUREF19, OnChangeEditCuref19)
	ON_EN_CHANGE(IDC_EDIT_CUREF20, OnChangeEditCuref20)
	ON_EN_CHANGE(IDC_EDIT_CUREF21, OnChangeEditCuref21)
	ON_EN_CHANGE(IDC_EDIT_CUREF22, OnChangeEditCuref22)
	ON_EN_CHANGE(IDC_EDIT_CUREF23, OnChangeEditCuref23)
	ON_EN_CHANGE(IDC_EDIT_CUREF24, OnChangeEditCuref24)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECV_DEVICE_CHANGE, OnRecvDeviceChange)
END_MESSAGE_MAP()
#define SZ_SECT_GENERAL         _T("General")
#define SZ_SECT_CONTROL         _T("Control")
#define SZ_SECT_DCSOURCE        _T("DCSOURCE")
#define SZ_SECT_PHONE           _T("PHONE")

#define SZ_KEY_OPERMETHOD       _T("Operation Mode")
#define SZ_KEY_DISAUTOUSB       _T("Disable AutoUSB")
#define SZ_KEY_IMEI             _T("IMEI1")
#define SZ_KEY_IMEI2            _T("IMEI2")
#define SZ_KEY_IMEI3            _T("IMEI3")
#define SZ_KEY_IMEI4            _T("IMEI4")
#define SZ_KEY_GENTYPEIMEI1     _T("IMEI1GenMode")
#define SZ_KEY_GENTYPEIMEI2     _T("IMEI2GenMode")
#define SZ_KEY_GENTYPEIMEI3     _T("IMEI3GenMode")
#define SZ_KEY_GENTYPEIMEI4     _T("IMEI4GenMode")
#define SZ_KEY_WIFI_ADDR        _T("WIFI_ADDR")
#define SZ_KEY_AUTO_GEN_WIFI    _T("AutoWIFI_ADDR")
#define SZ_KEY_BT_ADDR          _T("BT_ADDR")
#define SZ_KEY_PORT             _T("Port")
#define SZ_KEY_ENTER_MODE_WAIT  _T("EntrerModeWait")
#define SZ_KEY_AUTO_GEN_BT      _T("AutoGenerateBT_ADDR")
#define SZ_KEY_AUTO_GEN_IMEI    _T("AutoCreateIMEI")
#define SZ_KEY_DEFAULT_IMEI     _T("DefaultIMEI")
#define SZ_KEY_USE_SAME_IMEI    _T("UseSameIMEI")
#define SZ_KEY_PHASE_CHECK      _T("PhaseCheck")
#define SZ_KEY_PHASE_STATION    _T("PhaseIndex")
#define SZ_KEY_READBACK         _T("ReadBack")
#define SZ_KEY_USER_PRIVILEGE   _T("User Privilege")
#define SZ_KEY_TESTMODE         _T("TESTMODE")
#define SZ_KEY_CURRENTSTATION   _T("CurrentStationName")
#define SZ_KEY_PRESTATION       _T("CheckStationName")
#define SZ_KEY_REBOOT_DUT		_T("RebootDUT")
#define SZ_KEY_DUT_PORT		    _T("DUTPort")
#define SZ_KEY_OPERATE_CUREF	 _T("Operate CURef")
#define SZ_KEY_SEND_ATCMD		 _T("SendATCmd")
#define SZ_KEY_W_ATCMD      	 _T("WATCmd")

#define SZ_KEY_GENBTADDR_TYPE   _T("AutoGenerateBTAddrType")
#define SZ_KEY_BT_INIT_ADDR     _T("InitialAddrForBT")
#define SZ_KEY_GENWIFIADDR_TYPE _T("AutoGenerateWifiMacType")
#define SZ_KEY_WIFI_INIT_ADDR   _T("InitialAddrForWIFI")

#define SZ_KEY_PREREADIMEICOUNT _T("PreReadImeiCount")
#define SZ_KEY_ENABLE_WRITESN   _T("WriteSN")
#define SZ_KEY_SN1              _T("SN1")
#define SZ_KEY_SN2              _T("SN2")
#define SZ_KEY_AUTO_GEN_SN1     _T("AutoCreateSN1")
#define SZ_KEY_AUTO_GEN_SN2     _T("AutoCreateSN2")
#define SZ_KEY_SNLEN            _T("SN Length")

#define SZ_KEY_USEDC            _T("UseDCS")
#define SZ_KEY_DCTYPE           _T("DCType")
#define SZ_KEY_GPIBNO           _T("DCGpibNo")
#define SZ_KEY_GPIBADD          _T("DCGpibAddr")
#define SZ_KEY_TESTCOUNT        _T("TestCounts")

#define SZ_KEY_OTHER            _T("OTHER")
#define SZ_KEY_LAB_FONTSIZE1    _T("DisplayLab font size1")
#define SZ_KEY_LAB_FONTSIZE2    _T("DisplayLab font size2")

#define SZ_KEY_PHONE_CHKVER     _T("CheckVer")
#define SZ_KEY_PHONE_VERLMT     _T("Ver Length LowerLimit")
#define SZ_KEY_PHONE_VERSTR     _T("Version")

#define SZ_KEY_CHKIMEIAFTERREAD _T("CheckIMEIAfterRead")


#define SZ_SECT_SCANNER         _T("SCANNER")
#define SZ_KEY_ON_WRITE         _T("OnWrite")
#define SZ_KEY_ON_READ          _T("OnRead")


#define SZ_SECT_CUSTOMIZATION   _T("CUSTOMIZATION")
#define SZ_KEY_MA_BTADDR        _T("Manual Assign BT Addr")
#define SZ_KEY_MA_BTADDR_PATH   _T("BT Addr File Path")
#define SZ_KEY_MA_WIFIADDR      _T("Manual Assign WIFI Addr")
#define SZ_KEY_MA_WIFIADDR_PATH _T("WIFI Addr File Path")
#define SZ_KEY_AP_VER_FLAG		_T("CheckAPVersion")
#define SZ_KEY_AP_VER			_T("APVersion")


/////////////////////////////////////////////////////////////////////////////
// CWriteFlashDlg message handlers
void CWriteFlashDlg::LoadSetting()
{
	int nScannerKey = GetPrivateProfileInt(SZ_SECT_SCANNER, SZ_KEY_ON_WRITE, 0, m_szIniPath);
	if(!nScannerKey)
	{
		m_nScannerKey_OnWrite = VK_RETURN;
	}
	else
	{
		m_nScannerKey_OnWrite = VK_SPACE;
	}

	nScannerKey = GetPrivateProfileInt(SZ_SECT_SCANNER, SZ_KEY_ON_READ, 0, m_szIniPath);
	if(!nScannerKey)
	{
		m_nScannerKey_OnRead = VK_RETURN;
		if(m_nScannerKey_OnWrite == m_nScannerKey_OnRead)
		{
			m_nScannerKey_OnRead = VK_SPACE;
		}
	}
	else
	{
		m_nScannerKey_OnRead = VK_SPACE;
		if(m_nScannerKey_OnWrite == m_nScannerKey_OnRead)
		{
			m_nScannerKey_OnRead = VK_RETURN;
		}
	}

    TCHAR szTempBuf[50] = {0};
	m_bOperationMethod = GetPrivateProfileBool(SZ_SECT_GENERAL, SZ_KEY_OPERMETHOD, FALSE, m_szIniPath);
	m_bDisableAutoUSB = GetPrivateProfileBool(SZ_SECT_GENERAL, SZ_KEY_DISAUTOUSB, FALSE, m_szIniPath);
	m_nComPort = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_PORT, 1, m_szIniPath);
	m_nResvComPort = m_nComPort;
	m_bUsbMode = FALSE;
	if((!m_bOperationMethod) && (-1 == m_nComPort))
	{
		m_bUsbMode = TRUE;
	}

	m_bPreReadImeiCount = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_PREREADIMEICOUNT, 0, m_szIniPath);
    m_WriteParam.bChkIMEI = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_IMEI, 0, m_szIniPath);
    m_WriteParam.bChkIMEI2 = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_IMEI2, 0, m_szIniPath);
    m_WriteParam.bChkIMEI3= GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_IMEI3,0,m_szIniPath);
    m_WriteParam.bChkIMEI4= GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_IMEI4,0,m_szIniPath);
//  ## zijian.zhu @ 20111212 for NEWMS00147831 <<<	
	m_WriteParam.eGenIIMEI1 = (GCODIMEI_E)(GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI1, 0, m_szIniPath));
	m_WriteParam.eGenIIMEI2 = (GCODIMEI_E)(GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI2, 0, m_szIniPath));
	m_WriteParam.eGenIIMEI3 = (GCODIMEI_E)(GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI3, 0, m_szIniPath));
	m_WriteParam.eGenIIMEI4 = (GCODIMEI_E)(GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI4, 0, m_szIniPath));
//  ## zijian.zhu @ 20111212 for NEWMS00147831 >>>	
    m_WriteParam. bChkAutoIMEI = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_IMEI, 0, m_szIniPath);
    m_WriteParam. bChkUseSameIMEI = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_USE_SAME_IMEI, 0, m_szIniPath);
    m_WriteParam.bChkPhase = GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_PHASE_CHECK,0,m_szIniPath);
    m_WriteParam.bChkBTAddr = GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_BT_ADDR,0,m_szIniPath);
    m_WriteParam.bChkAutoCreateBT = GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_AUTO_GEN_BT,0,m_szIniPath);
    m_WriteParam.bCheckWifi = GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_WIFI_ADDR,0,m_szIniPath);
    m_WriteParam.bChkAutoCreateWifi = GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_AUTO_GEN_WIFI,0,m_szIniPath);
    m_dwEnterModeWait = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_ENTER_MODE_WAIT, 100, m_szIniPath);
    m_nTimeout  = GetPrivateProfileInt(SZ_SECT_GENERAL, _T("Timeout"), 40000, m_szIniPath);
    m_nWriteTime = GetPrivateProfileInt(SZ_SECT_GENERAL, _T("WriteTime"), 10000, m_szIniPath);
    m_bWriteTime = GetPrivateProfileInt(SZ_SECT_GENERAL, _T("WriteTimeCheck"),0,m_szIniPath);
    m_nStationIndex = GetPrivateProfileInt(SZ_SECT_GENERAL,SZ_KEY_PHASE_STATION,1,m_szIniPath);
    m_bAdminMode = GetPrivateProfileInt(SZ_SECT_CONTROL, SZ_KEY_USER_PRIVILEGE, 0, m_szIniPath);
    m_nTestMode  = GetPrivateProfileInt(SZ_SECT_CONTROL,SZ_KEY_TESTMODE,0,m_szIniPath);
    m_WriteParam.bCheckValid  = GetPrivateProfileInt(SZ_SECT_GENERAL, _T("CheckValid"), 0, m_szIniPath);
    m_WriteParam.bSaveIMEI  =  GetPrivateProfileInt(SZ_SECT_GENERAL, _T("SaveIMEI"),0,m_szIniPath); 
    m_WriteParam.bSaveToTxt  = GetPrivateProfileInt(SZ_SECT_GENERAL, _T("SaveToTxt"),0,m_szIniPath); 
    m_WriteParam.bCheckRepeat= GetPrivateProfileInt(SZ_SECT_GENERAL, _T("CheckRepeat"),0,m_szIniPath);
    m_WriteParam.nMocver =   GetPrivateProfileInt(SZ_SECT_CONTROL, _T("MocorNew"), 0, m_szIniPath);
    m_bUseDcs = GetPrivateProfileInt(SZ_SECT_DCSOURCE, SZ_KEY_USEDC, 0, m_szIniPath);
    int nDcsType = GetPrivateProfileInt(SZ_SECT_DCSOURCE, SZ_KEY_DCTYPE,1,m_szIniPath);
    int nGpibNo  = GetPrivateProfileInt(SZ_SECT_DCSOURCE, SZ_KEY_GPIBNO,0,m_szIniPath);
    int nGpibAddr = GetPrivateProfileInt(SZ_SECT_DCSOURCE, SZ_KEY_GPIBADD,5,m_szIniPath);
    m_nTestCounts = GetPrivateProfileInt(SZ_SECT_DCSOURCE, SZ_KEY_TESTCOUNT,1,m_szIniPath);
    USES_CONVERSION;
    GetPrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_CURRENTSTATION,_T("IMEI"),m_szStationName,sizeof(m_szStationName),m_szIniPath); 
    strcpy(m_szCurrentStation,W2A(m_szStationName));
    GetPrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_PRESTATION,_T("NULL"),m_szStationName,sizeof(m_szStationName),m_szIniPath); 
    strcpy(m_szCheckStation,W2A(m_szStationName));

    m_WriteParam.bCheckVer = GetPrivateProfileInt(SZ_SECT_PHONE, SZ_KEY_PHONE_CHKVER, 0, m_szIniPath);
	m_WriteParam.nVerLenLowLimit = GetPrivateProfileInt(SZ_SECT_PHONE, SZ_KEY_PHONE_VERLMT, 1, m_szIniPath);
    GetPrivateProfileString(SZ_SECT_PHONE, SZ_KEY_PHONE_VERSTR, _T("1.0"), szTempBuf, sizeof(szTempBuf), m_szIniPath);
    m_WriteParam.sVersion.Format(_T("%s"),szTempBuf);

	if((m_WriteParam.nVerLenLowLimit >= m_WriteParam.sVersion.GetLength()) && m_WriteParam.bCheckVer)
	{
		CString strVerLenTip(_T(""));
		strVerLenTip.Format(IDS_SET_VERSIONLENGTH_ERR, m_WriteParam.nVerLenLowLimit);
		AfxMessageBox(strVerLenTip);
		m_WriteParam.bCheckVer = FALSE;
	}

	if(!m_WriteParam.bChkIMEI)   //强制
    {
        m_WriteParam.bChkIMEI2 =0;
        m_WriteParam.bChkIMEI3 =0;
        m_WriteParam.bChkIMEI4 =0;
        m_WriteParam.bChkAutoIMEI=0;
        m_WriteParam.bChkUseSameIMEI =0;
    }
    if(!m_WriteParam.bChkBTAddr)
    {
        m_WriteParam.bChkAutoCreateBT =0;
    }
    if(MOCOR_VER_PRE09A37==m_WriteParam.nMocver)
    {
        m_WriteParam.bChkPhase =0;
    }

    GetPrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_DEFAULT_IMEI, _T("12345678901235"), szTempBuf,  sizeof(szTempBuf), m_szIniPath);
    szTempBuf[14]='\0';  
    m_WriteParam.sAutoIMEI.Format(_T("%s"),szTempBuf);
    
    GetPrivateProfileString(SZ_SECT_GENERAL, _T("ForeWIFI"), _T("00037F"), szTempBuf,  sizeof(szTempBuf), m_szIniPath);
    szTempBuf[6]='\0';  
    m_WriteParam.sForeWifi.Format(_T("%s"),szTempBuf);
	CString strWifiAddress(_T(""));
	strWifiAddress.Format(_T("%s000000"),szTempBuf);
	m_int64WifiAddrForMinimize = StringHextoDec(strWifiAddress);
	strWifiAddress.Format(_T("%sFFFFFF"),szTempBuf);
	m_int64WifiAddrForMaximize = StringHextoDec(strWifiAddress);

	m_int64BtAddrForMinimize = 0x000000000000;
	m_int64BtAddrForMaximize = 0xFFFFFFFFFFFF;

	// about AutoGenerate Bluetooth Address
	m_WriteParam.bGenBtAddrType = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_GENBTADDR_TYPE, 0, m_szIniPath);
	ZeroMemory(szTempBuf, sizeof(szTempBuf));
    GetPrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_BT_INIT_ADDR, _T("010203040506"), szTempBuf, sizeof(szTempBuf), m_szIniPath);
    m_WriteParam.strInitAddrForBT.Format(_T("%s"),szTempBuf);
	m_WriteParam.strInitAddrForBT.MakeLower();
	if(12 < m_WriteParam.strInitAddrForBT.GetLength())
	{
		m_WriteParam.strInitAddrForBT = m_WriteParam.strInitAddrForBT.Left(12);
	}

	// about AutoGenerate WIFI Mac Address
	m_WriteParam.bGenWifiMacType = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_GENWIFIADDR_TYPE, 0, m_szIniPath);
	ZeroMemory(szTempBuf, sizeof(szTempBuf));
    GetPrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_WIFI_INIT_ADDR, _T("010203040506"), szTempBuf, sizeof(szTempBuf), m_szIniPath);
    m_WriteParam.strInitAddrForWifi.Format(_T("%s"),szTempBuf);
	m_WriteParam.strInitAddrForWifi.MakeLower();
	if(12 < m_WriteParam.strInitAddrForWifi.GetLength())
	{
		m_WriteParam.strInitAddrForWifi = m_WriteParam.strInitAddrForWifi.Left(12);
	}

	// about Read/Write SN Function
	m_WriteParam.bEnableSNFunc = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_ENABLE_WRITESN, 0, m_szIniPath);
	if(m_WriteParam.bEnableSNFunc)
	{
		m_WriteParam.bChkSN1 = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_SN1, 0, m_szIniPath);
		m_WriteParam.bChkSN2 = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_SN2, 0, m_szIniPath);
		m_WriteParam.bChkAutoSN1 = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_SN1, 0, m_szIniPath);
		m_WriteParam.bChkAutoSN2 = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_SN2, 0, m_szIniPath);
		m_nSNLength = GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_SNLEN, WIMEI_SN_LENGTH_MINIMIZE, m_szIniPath);
		if(WIMEI_SN_LENGTH_MINIMIZE > m_nSNLength)
		{
			m_nSNLength = WIMEI_SN_LENGTH_MINIMIZE;
		}
		else if(WIMEI_SN_LENGTH_MAXIMIZE <= m_nSNLength)
		{
			m_nSNLength = WIMEI_SN_LENGTH_MAXIMIZE;
		}
	}

	// about Other
	m_nDisplayLabelFontSize1 = GetPrivateProfileInt(SZ_KEY_OTHER, SZ_KEY_LAB_FONTSIZE1, 20, m_szIniPath);
	m_nDisplayLabelFontSize2 = GetPrivateProfileInt(SZ_KEY_OTHER, SZ_KEY_LAB_FONTSIZE2, 20, m_szIniPath);
	
	// for NEWMS00185078
	m_WriteParam.bChkIMEIAfterRead = (BOOL)GetPrivateProfileInt(SZ_SECT_GENERAL, SZ_KEY_CHKIMEIAFTERREAD, 0, m_szIniPath);


	//Debug时控制电流源循环测试用
    if(m_bUseDcs && (!m_bOperationMethod))   
    {
       	DCS_TYPE eDcsType = static_cast<DCS_TYPE>(nDcsType);
        m_pDcsFactory->CreateDCSource(&m_pDcSource, eDcsType);
        if(NULL != m_pDcSource)
        {
            if(RFT_SUCCESS != m_pDcSource->InitDCSource(nGpibNo, nGpibAddr))
            {
                AfxMessageBox(_T("找不到电源，关闭电源控制!"));
                m_bUseDcs=0;
            }
        }    
    }

//  [5/17/2012 Xiaoping.Jing] NEWMS00205017 [[[ 
    int nInput = 0;
    nInput = GetPrivateProfileInt(SZ_SECT_GENERAL, _T("Check BT Valid"), 0, m_szIniPath);
    m_bCheckBTInput = (1 == nInput) ? TRUE : FALSE;
    GetPrivateProfileString(SZ_SECT_GENERAL, _T("BT   Addr. Format"), _T(""), m_szFmtBT, sizeof(m_szFmtBT), m_szIniPath);
    m_szFmtBT[MAX_BT_ADDR_STR_LENGTH-1]   = _T('\0');

    nInput = GetPrivateProfileInt(SZ_SECT_GENERAL, _T("Check WIFI Valid"), 0, m_szIniPath);
    m_bCheckWFInput = (1 == nInput) ? TRUE : FALSE;
    GetPrivateProfileString(SZ_SECT_GENERAL, _T("WIFI Addr. Format"), _T(""), m_szFmtWF, sizeof(m_szFmtWF), m_szIniPath);
    m_szFmtWF[MAX_WIFI_ADDR_STR_LENGTH-1] = _T('\0');
//  [5/17/2012 Xiaoping.Jing] NEWMS00205017 ]]]

//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  {{{
//
	TCHAR szPath[_MAX_PATH] = {0};

	m_WriteParam.bManualAssignBtAddr = GetPrivateProfileBool(SZ_SECT_CUSTOMIZATION, SZ_KEY_MA_BTADDR, FALSE, m_szIniPath);
	GetPrivateProfileString(SZ_SECT_CUSTOMIZATION, SZ_KEY_MA_BTADDR_PATH, _T(""), szPath, sizeof(szPath), m_szIniPath);
	m_WriteParam.strBtAddrPath.Format(_T("%s"), szPath);
	ZeroMemory(szPath, sizeof(szPath));
	m_WriteParam.bManualAssignWifiAddr = GetPrivateProfileBool(SZ_SECT_CUSTOMIZATION, SZ_KEY_MA_WIFIADDR, FALSE, m_szIniPath);
	GetPrivateProfileString(SZ_SECT_CUSTOMIZATION, SZ_KEY_MA_WIFIADDR_PATH, _T(""), szPath, sizeof(szPath), m_szIniPath);
	m_WriteParam.strWifiAddrPath.Format(_T("%s"), szPath);

//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  }}}
//
	if((m_WriteParam.bChkBTAddr && m_WriteParam.bManualAssignBtAddr) || (m_WriteParam.bCheckWifi && m_WriteParam.bManualAssignWifiAddr))
	{
		LoadCustomAddrConfig();
	}

	//teana hu 2012.10.16
	m_WriteParam.bChkAPVer = GetPrivateProfileBool(SZ_SECT_GENERAL, SZ_KEY_AP_VER_FLAG, FALSE, m_szIniPath);
	if(m_WriteParam.bChkAPVer)
	{
		TCHAR szTemp[512] = {0};
		GetPrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AP_VER,_T(""),szTemp, 512, m_szIniPath);
		m_WriteParam.strAPVerInput = szTemp;
	}
	//

	//teana hu 2012.12.06
	m_bRebootDUT = GetPrivateProfileBool(SZ_SECT_CONTROL, SZ_KEY_REBOOT_DUT, TRUE, m_szIniPath);
	m_nDUTPort = GetPrivateProfileInt(SZ_SECT_CONTROL, SZ_KEY_DUT_PORT, 0, m_szIniPath);

	if(m_nDUTPort <= 0)
	{
		m_nDUTPort = -1;
	}

	//

	//teana hu 2012.12.17
	m_bOperateCURef = GetPrivateProfileBool(SZ_SECT_CONTROL, SZ_KEY_OPERATE_CUREF, FALSE, m_szIniPath);
	//

	//teana hu 2012.12.19
	m_strArrWATCmd.RemoveAll();
	TCHAR szTemp[4096] = {0};
	GetPrivateProfileString(SZ_SECT_CONTROL, SZ_KEY_W_ATCMD,_T(""),szTemp, 4096, m_szIniPath);
	CString strCmd = szTemp;
	CString strTemp;
	int nIndex = strCmd.Find(_T("|"));
	while (nIndex != -1)
	{
		strTemp = strCmd.Left(nIndex);
		m_strArrWATCmd.Add(strTemp);
		strCmd.Delete(0, nIndex + 1);
		nIndex = strCmd.Find(_T("|"));
	}
	
	m_bSendATCmd = GetPrivateProfileBool(SZ_SECT_CONTROL, SZ_KEY_SEND_ATCMD, FALSE, m_szIniPath);
		
	//

    return;
}

#define SZ_CUST_SECT_BTADDR  _T("BT Address")
#define SZ_CUST_SECT_WFADDR  _T("WIFI Address")
#define SZ_CUST_KEY_ADDRBGN  _T("Start Address")
#define SZ_CUST_KEY_ADDRCRR  _T("Current Address")
#define SZ_CUST_KEY_ADDREND  _T("End Address")
void CWriteFlashDlg::LoadCustomAddrConfig()
{
	ZeroMemory(&m_CustomBtAddrComp, sizeof(m_CustomBtAddrComp));
	ZeroMemory(&m_CustomWifiAddrComp, sizeof(m_CustomWifiAddrComp));

	TCHAR szTemp[50]={0};
	unsigned __int64 adcurr=0, adstar=0, adstop=0;
	BOOL bCheckManAss=TRUE;

	if(m_WriteParam.bChkBTAddr && m_WriteParam.bManualAssignBtAddr)
	{
		GetPrivateProfileString(SZ_CUST_SECT_BTADDR, SZ_CUST_KEY_ADDRBGN, _T(""), m_CustomBtAddrComp.szStarAddr, sizeof(m_CustomBtAddrComp.szStarAddr), m_WriteParam.strBtAddrPath);
		GetPrivateProfileString(SZ_CUST_SECT_BTADDR, SZ_CUST_KEY_ADDRCRR, _T(""), m_CustomBtAddrComp.szCurrAddr, sizeof(m_CustomBtAddrComp.szCurrAddr), m_WriteParam.strBtAddrPath);
		GetPrivateProfileString(SZ_CUST_SECT_BTADDR, SZ_CUST_KEY_ADDREND, _T(""), m_CustomBtAddrComp.szStopAddr, sizeof(m_CustomBtAddrComp.szStopAddr), m_WriteParam.strBtAddrPath);
		adcurr = StringHextoDec(m_CustomBtAddrComp.szCurrAddr);
		adstar = StringHextoDec(m_CustomBtAddrComp.szStarAddr);
		adstop = StringHextoDec(m_CustomBtAddrComp.szStopAddr);
		if(adstar>=adstop || adstar>adcurr || adstop<adcurr)
		{
			AfxMessageBox(_T("当前蓝牙地址配置不正确，请重新选择正确的蓝牙地址文件!"));
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
			m_bCustAddrInvalid = FALSE;
			return;
		}	
	}
	
	if(m_WriteParam.bCheckWifi && m_WriteParam.bManualAssignWifiAddr)
	{
		GetPrivateProfileString(SZ_CUST_SECT_WFADDR, SZ_CUST_KEY_ADDRBGN, _T(""), m_CustomWifiAddrComp.szStarAddr, sizeof(m_CustomWifiAddrComp.szStarAddr), m_WriteParam.strWifiAddrPath);
		GetPrivateProfileString(SZ_CUST_SECT_WFADDR, SZ_CUST_KEY_ADDRCRR, _T(""), m_CustomWifiAddrComp.szCurrAddr, sizeof(m_CustomWifiAddrComp.szCurrAddr), m_WriteParam.strWifiAddrPath);
		GetPrivateProfileString(SZ_CUST_SECT_WFADDR, SZ_CUST_KEY_ADDREND, _T(""), m_CustomWifiAddrComp.szStopAddr, sizeof(m_CustomWifiAddrComp.szStopAddr), m_WriteParam.strWifiAddrPath);
		
		adcurr = StringHextoDec(m_CustomWifiAddrComp.szCurrAddr);
		adstar = StringHextoDec(m_CustomWifiAddrComp.szStarAddr);
		adstop = StringHextoDec(m_CustomWifiAddrComp.szStopAddr);
		if(adstar>=adstop || adstar>adcurr || adstop<adcurr)
		{
			AfxMessageBox(_T("当前WIFI地址配置不正确，请重新选择正确的蓝牙地址文件!"));
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
			m_bCustAddrInvalid = FALSE;
			return;
		}
	}

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	m_bCustAddrInvalid = TRUE;
}

void CWriteFlashDlg::SaveCustomAddrConfig()
{
    WritePrivateProfileString(SZ_CUST_SECT_BTADDR, SZ_CUST_KEY_ADDRCRR, m_CustomBtAddrComp.szCurrAddr, m_WriteParam.strBtAddrPath);
	WritePrivateProfileString(SZ_CUST_SECT_WFADDR, SZ_CUST_KEY_ADDRCRR, m_CustomWifiAddrComp.szCurrAddr, m_WriteParam.strWifiAddrPath);
}

BOOL CWriteFlashDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
	DoRegisterDeviceInterface();
    // Add "About..." menu item to system menu.
    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);  //lint !e506
    ASSERT(IDM_ABOUTBOX < 0xF000);  //lint !e506
    
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    
    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon
    
    m_hExitThread = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hEnterModeSuccess = CreateEvent(NULL, TRUE, FALSE, NULL);
    // TODO: Add extra initialization here
    //Setting title 
    CString str;
    GetWindowText(str);
#ifdef _DEBUG
    str += _T(" D");
#else
    str += _T(" R");
#endif
    str += theApp.m_strVersion;
    SetWindowText(str);  
    DWORD dwRet = ::GetModuleFileName(AfxGetApp()->m_hInstance, m_szIniPath, _MAX_PATH);    
    if (!dwRet)    
        return FALSE; 
    TCHAR* pResult = _tcsrchr(m_szIniPath,_T('\\'));    
    if (pResult == NULL)
    {        
        return FALSE;
    }    
    *pResult = 0;
    _tcscpy(m_szIMEITxtPath,m_szIniPath);
    _tcscpy(m_szIMEIMdbPath,m_szIniPath);
    _tcscat(m_szIMEITxtPath, _T("\\IMEI.txt"));
    _tcscat(m_szIMEIMdbPath, _T("\\IMEI.mdb"));
    _tcscat(m_szIniPath, _T("\\WriteImei.ini"));
    LoadSetting();


	//teana hu 2012.12.17
	if(!m_bOperateCURef)
	{
		for(int i = 0; i < CU_REFERENCE_LEN; i++)
		{
			GetDlgItem(IDC_EDIT_CUREF1 + i)->EnableWindow(FALSE);
		}
	}
	for(int i = 0; i < CU_REFERENCE_LEN; i++)
	{
		GetDlgItem(IDC_EDIT_CUREF1 + i)->SetWindowText(_T("\0"));
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF1 + i))->SetLimitText(1);
	}
	//

//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<	
	if (!m_WriteParam.bEnableSNFunc)
	{
		int nheigredu = 0;
		CRect rectCtrl;

		GetDlgItem(IDC_EDIT_SNSENIOR)->GetWindowRect(&rectCtrl);
		nheigredu = rectCtrl.bottom;
		GetDlgItem(IDC_EDIT_SNMAIN)->GetWindowRect(&rectCtrl);
		nheigredu -= rectCtrl.top;

		GetDlgItem(IDC_EDIT_SNMAIN)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SNSENIOR)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_SN1)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_SN2)->ShowWindow(FALSE);

		GetDlgItem(IDC_STATIC_MAINFRAME)->GetWindowRect(&rectCtrl);
		rectCtrl.bottom -= nheigredu;
		ScreenToClient(&rectCtrl);
		GetDlgItem(IDC_STATIC_MAINFRAME)->MoveWindow(&rectCtrl);

		int nMoveIDList[2] = { IDC_STATIC_MODEDISP, IDC_STATIC_PORTINDICATOR };
		for(int i=0; i<2; i++)
		{
			GetDlgItem(nMoveIDList[i])->GetWindowRect(&rectCtrl);
			rectCtrl.top -= nheigredu;
			rectCtrl.bottom -= nheigredu;
			ScreenToClient(&rectCtrl);
			GetDlgItem(nMoveIDList[i])->MoveWindow(&rectCtrl);
		}

		GetClientRect(&rectCtrl);
		rectCtrl.bottom -= (nheigredu - 30);
		MoveWindow(&rectCtrl);
	}
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>
    
    //设置界面字体背景颜色
    m_pFont= new CFont(); 
    m_pFont->CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));  
    m_CEditMainIMEI.SetFont(m_pFont);
    m_CEditSecIMEI.SetFont(m_pFont);
    m_CEditThirdIMEI.SetFont(m_pFont);
    m_CEditFourIMEI.SetFont(m_pFont);
    m_CEditBT.SetFont(m_pFont);
    m_CEditWifi.SetFont(m_pFont);
	m_CEditSN1.SetFont(m_pFont);
	m_CEditSN2.SetFont(m_pFont);
    m_pFont1 = new CFont();

	if (!m_WriteParam.bEnableSNFunc)
	{
		m_pFont1->CreateFont(m_nDisplayLabelFontSize1, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("SIMHEI"));  
	}
	else
	{
		m_pFont1->CreateFont(m_nDisplayLabelFontSize2, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("SIMHEI"));  
	}
    GetDlgItem(IDC_STATIC_MESSAGE)->SetFont(m_pFont1);

	m_pFontPortInd = new CFont();
	m_pFontPortInd->CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Dotum"));
    GetDlgItem(IDC_STATIC_PORTINDICATOR)->SetFont(m_pFontPortInd);
    //串口
	CString strInitTmp(_T(""));
	m_bPortCmbAddIndi = FALSE;
	m_bConnState = EnumCommPort();
	DisplayPortCableInd(m_bConnState);
	ModeStaticIndi(m_bOperationMethod, m_bConnState);
	if(m_bOperationMethod || (-1 != m_nComPort))
	{
		TimerAdminMg(TRUE);
	}
	
    //界面显示
    m_strMsg.LoadString(IDS_INIT);
    DisPlayMessage(STATE_IDLE);     
    EnableWindow(TRUE);
    ReflashUI();  
    if(m_WriteParam.bSaveIMEI&&(!m_WriteParam.bSaveToTxt))
    {
        ConnectDatabase();
    }
    else if(m_WriteParam.bSaveToTxt)
    {
        OpenFile();
    }

	((CEdit *)GetDlgItem(IDC_EDIT_SW_VERSION))->SetReadOnly(TRUE);

	m_devHound.SetReceiver((DWORD)this->GetSafeHwnd(), WM_RECV_DEVICE_CHANGE, FALSE);
	m_devHound.Start();

    return FALSE;   //Thera are setfocus in Reflash function
}

void CWriteFlashDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CXAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWriteFlashDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting
        
        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
        
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWriteFlashDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CWriteFlashDlg::AutoCreateIMEI(LPTSTR lpszIMEI)
{
    __int64 int64CD = ComputeCD(m_int64DefaultIMEI);
    __int64 int64IMEI = m_int64DefaultIMEI * 10 + int64CD;
    _i64tot(int64IMEI, lpszIMEI, 10); //10进制IMEI转化成字符串
}

__int64 CWriteFlashDlg::ComputeCD(__int64 nImei)
{
    int nTime = 0;
    __int64 nCD = 0;  
    while(nImei != 0)
    {
        __int64 d = nImei % 10;
        if(nTime % 2 == 0)
        {
            d *= 2;
            if(d >= 10)
            {
                d = d % 10 + d / 10;
            }
        }   
        nCD += d;   
        nTime++;
        nImei /= 10;
    }  
    nCD %= 10;
    if(nCD != 0)
    {
        nCD = 10 - nCD;
    }  
    return nCD;
}

void CWriteFlashDlg::AutoCreateBTAddr(LPTSTR lpszTempBT)
{
    ASSERT(NULL != lpszTempBT);
    srand((unsigned)time( NULL));
    int nRand = rand();
    CTime CurrTime = CTime::GetCurrentTime();
    _stprintf(lpszTempBT, _T("%04d%02d%02d%02d%02d"), nRand % 10000, 
        CurrTime.GetDay(), CurrTime.GetHour(), CurrTime.GetMinute(), CurrTime.GetSecond());
}

void CWriteFlashDlg::AutoCreateWifiAddr(LPTSTR lpszTempWIFI)
{
    ASSERT(NULL != lpszTempWIFI);
    srand((unsigned)time( NULL));
    DWORD dwTime = GetTickCount();
    _stprintf(lpszTempWIFI, _T("%s%02d%02d%x%x"),m_WriteParam.sForeWifi, 
		rand()%99,dwTime%99, rand()%16,rand()%16);
}

void CWriteFlashDlg::ReflashUI()
{ 
    TCHAR szTemp[50]={0}, szAssi[50]={0};
    _tcscpy(szTemp,m_WriteParam.sAutoIMEI);
    m_int64DefaultIMEI=  _ttoi64(szTemp);
	CString strInitCode(_T("")); 
	
	// ## Refresh IMEI Code
	AutoGenerateImeiCodeAction(FALSE);
	
	// ## Bluetooth Address
	if(m_WriteParam.bChkBTAddr)
	{
		if(m_WriteParam.bManualAssignBtAddr)
		{
			m_strEditBT.Format(_T("%s"),m_CustomBtAddrComp.szCurrAddr);
		}
		else
		{
			if(m_WriteParam.bChkAutoCreateBT)
			{
				if(m_WriteParam.bGenBtAddrType)
				{
					m_strEditBT = m_WriteParam.strInitAddrForBT;
				}
				else
				{
					// AutoCreateBTAddr(m_szBT);
					AutoCreateBTAddr(szTemp);
					m_strEditBT.Format(_T("%s"),szTemp);
				}
			}
		}
	}


	// ## WIFI Mac Address
	if(m_WriteParam.bCheckWifi)
	{
		if(m_WriteParam.bManualAssignWifiAddr)
		{
			m_strWifi.Format(_T("%s"),m_CustomWifiAddrComp.szCurrAddr);			
		}
		else
		{
			if(m_WriteParam.bChkAutoCreateWifi)
			{
				if(m_WriteParam.bGenWifiMacType)
				{
					m_strWifi =  m_WriteParam.strInitAddrForWifi;
				}
				else
				{
					// AutoCreateBTAddr(m_szBT);
					AutoCreateWifiAddr(szTemp);
					m_strWifi.Format(_T("%s"),szTemp);
				}
			}
		}
	}


//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	if(m_WriteParam.bEnableSNFunc)
	{
		if(m_WriteParam.bChkSN1 && m_WriteParam.bChkAutoSN1)
		{
			ZeroMemory(szTemp, sizeof(szTemp));
			AutoCreateSN(szTemp, m_nSNLength);
			m_strSN1.Format(_T("%s"),szTemp);
		}
		if(m_WriteParam.bChkSN2 && m_WriteParam.bChkAutoSN2)
		{
			ZeroMemory(szTemp, sizeof(szTemp));
			if(m_WriteParam.bChkSN1 && m_WriteParam.bChkAutoSN1)
			{
				if(m_nSNLength > 12)
				{
					// ## zijian.zhu @ 20111213 for NEWMS00147831 <<<
					CString strLowgB(_T("")), strHighB(_T(""));
					strLowgB = m_strSN1.Right(12);
					strHighB = m_strSN1.Left((int)(m_nSNLength-12));
					unsigned __int64 c1 = _ttoi64(strLowgB);
					unsigned __int64 c2 = _ttoi64(strHighB);
					unsigned __int64 c3 = (unsigned __int64)pow((double)0xF, (int)(m_nSNLength-12)) - 1;
					if(0xFFFFFFFFFFFF == c1)
					{
						c1 = 0;
						c2 = (c2 == c3) ? 0 : (c2 + 1);
					}
					else
					{
						c1 += 1;
					}
					_ui64tot(c1, szTemp, 10);
					_ui64tot(c2, szAssi, 10);
					m_strSN2.Format(_T("%s%012s"), szAssi, szTemp);
					// ## zijian.zhu @ 20111213 for NEWMS00147831 >>>
				}
				else
				{
					m_strSN2 = m_strSN1;
					unsigned __int64 c = _ttoi64(m_strSN2)+1;
					_ui64tot(c,szTemp,10);
					m_strSN2.Format(_T("%s"), szTemp);   //m_strEditSN2 =m_strEditSN1+1   
				}
			}
			else
			{
				AutoCreateSN(szTemp, m_nSNLength);
				m_strSN2.Format(_T("%s"),szTemp);
			}
		}
	}
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>

    if(m_WriteParam.bChkIMEI&&(!m_WriteParam.bChkAutoIMEI))
    {
        m_CEditMainIMEI.SetFocus(); 
    } 
    else if(m_WriteParam.bChkBTAddr&&(!m_WriteParam.bChkAutoCreateBT))
    {
        m_CEditBT.SetFocus();    
    }
	else if(m_WriteParam.bCheckWifi&&(!m_WriteParam.bChkAutoCreateWifi))
	{
        m_CEditWifi.SetFocus();    
	}
	else if(m_WriteParam.bChkSN1&&(!m_WriteParam.bChkAutoSN1))
	{
		m_CEditSN1.SetFocus();
	}
	else if(m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2))
	{
		m_CEditSN2.SetFocus();
	}
    else
    {
        GetDlgItem(IDC_BUTTON_START)->SetFocus();
        m_bEnter = TRUE;  
    }

     //显示界面Yield 
    m_strPass.Format(IDS_STATIC_PASS,m_nPassCounts);
    m_strFail.Format(IDS_STATIC_FAIL,m_nFailCounts);
    m_strYield.Format(IDS_STATIC_YIELD,m_nPassCounts*100);
    if((m_nFailCounts+m_nPassCounts)!=0)
    {
       m_strYield.Format(IDS_STATIC_YIELD,m_nPassCounts*100/(m_nFailCounts+m_nPassCounts)); //lint !e414
    }
    //m_strYield.Format(IDS_STATIC_YIELD,m_nPassCounts*100/((m_nFailCounts+m_nPassCounts)==0?1:(m_nFailCounts+m_nPassCounts)));
    m_StaticPass.SetTextColor(COLOR_PASS);
    m_StaticFail.SetTextColor(COLOR_FAIL);
    m_StaticYield.SetTextColor(COLOR_WORKING);
    UpdateData(FALSE);
}

void CWriteFlashDlg::OnButtonSet() 
{
    // TODO: Add your control notification handler code here
    CloseFile();
    CIMEISetDlg dlg(&m_WriteParam);
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
		if((m_WriteParam.bChkBTAddr && m_WriteParam.bManualAssignBtAddr) || (m_WriteParam.bCheckWifi && m_WriteParam.bManualAssignWifiAddr))
		{
			LoadCustomAddrConfig();
		}

        EnableWindow(TRUE);
		OnCleanCodeEdit();
        m_strMsg.LoadString(IDS_INIT);
        m_StaticMsg.SetBkColor(COLOR_IDLE);
        ReflashUI();
        UpdateData(FALSE);          
    }
    if(m_WriteParam.bSaveIMEI&&(!m_WriteParam.bSaveToTxt))
    {
        ConnectDatabase();
    }
    else if(m_WriteParam.bSaveToTxt)
    {
        OpenFile();
    }
    m_nActualCounts =0;
    return;
}

BOOL CWriteFlashDlg::PreTranslateMessage(MSG* pMsg) 
{
    // TODO: Add your specialized code here and/or call the base class
    if(m_bEnter&&pMsg->message==WM_KEYDOWN && pMsg->wParam ==m_nScannerKey_OnWrite)
        //if(pMsg->message==WM_KEYDOWN && pMsg->wParam ==VK_SPACE)
    {
        if(GetDlgItem(IDC_BUTTON_START)->IsWindowEnabled())
            OnButtonStart();
    }
    if(pMsg->message==WM_KEYDOWN && pMsg->wParam ==m_nScannerKey_OnRead)
    {        
        if(GetDlgItem(IDC_BUTTON_READ)->IsWindowEnabled())
            OnButtonRead();
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CWriteFlashDlg::EnumCommPort()
{
	if(!m_bPortCmbAddIndi || m_bIsTesting)
	{
		GetDlgItem(IDC_COMBO_FORPORT)->EnableWindow(FALSE);
	}
    // Check valible serial port
    HKEY  hOpenKey;
    const int cLen = 255;
    DWORD dwValueLen;
    DWORD dwDataLen;
    TCHAR szValueName[cLen];
    TCHAR szData[cLen];
    DWORD dwType;
    DWORD dwRet;
	CString strpn(_T(""));
	BOOL  bFindRes = FALSE;
	CString strText(_T(""));
    int i = 0, j = 0;
    
    if( ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), NULL, KEY_QUERY_VALUE | KEY_READ,
        &hOpenKey))
    {
		if(!m_bPortCmbAddIndi)
		{
			m_cmbFormalPort.ResetContent();
			if(!m_bOperationMethod)
			{
				m_cmbFormalPort.InsertString(0, _T("USB"));
				j++;
			}
		}
        while(TRUE)   //lint !e716
        {
            dwValueLen = cLen;
            dwDataLen = cLen;
            szValueName[0] = 0;
            szData[0] = 0;      
            dwRet = ::RegEnumValue(hOpenKey,  i++, szValueName,&dwValueLen, NULL,&dwType,(BYTE*)szData, &dwDataLen); 
            if(dwRet == ERROR_NO_MORE_ITEMS)
			{
                break;
			}
			if(0 == wcsnicmp(szData, _T("com"), 3)) 
			{
				if(!m_bPortCmbAddIndi)
				{
					m_cmbFormalPort.InsertString(j, szData);
				}
				if(m_bOperationMethod || (-1 != m_nComPort))
				{
					strText.Format(_T("%s"), szData);
					strText.TrimLeft(_T("COM"));
					if((m_nComPort == _ttoi(strText)) || (m_nResvComPort == _ttoi(strText)))
					{
						bFindRes = TRUE;
					}
				}
			}
        }        
    }
    RegCloseKey(hOpenKey);

	if(!m_bPortCmbAddIndi)
	{

		int k = 0, l = 0;
		BOOL bFPort = FALSE, bFRsvPort = FALSE;
		j = 0;
		if((!m_bOperationMethod) && (-1 == m_nComPort))
		{
			m_bUsbMode = TRUE;
		}
		else
		{
			if(!bFindRes)
			{
				if(!m_bOperationMethod)
				{
					m_nComPort = -1;
					m_bUsbMode = TRUE;
				}
				else
				{
					strText.Empty();
					m_cmbFormalPort.GetLBText(0, strText);
					strText.TrimLeft(_T("COM"));
					m_nComPort = _ttoi(strText);
					m_bUsbMode = FALSE;
				}
				j = 0;
			}
			else
			{
				i = 0;
				if(!m_bOperationMethod)
				{
					i++;
				}
				for (; i<m_cmbFormalPort.GetCount(); i++)
				{
					strText.Empty();
					m_cmbFormalPort.GetLBText(i, strText);
					strText.TrimLeft(_T("COM"));
					int uPort = _ttoi(strText);
					if (uPort == m_nComPort) 
					{
						l = i;
						bFPort = TRUE;
					}
					if(uPort == m_nResvComPort)
					{
						k = i;
						bFRsvPort = TRUE;
					}
				}
				if(bFRsvPort)
				{
					j = k;
					m_nComPort = m_nResvComPort;
				}
				else if(bFPort)
				{
					j = l;
				}
				else
				{
					if(!m_bOperationMethod)
					{
						m_nComPort = -1;
						m_bUsbMode = TRUE;
					}
					else
					{
						strText.Empty();
						m_cmbFormalPort.GetLBText(0, strText);
						strText.TrimLeft(_T("COM"));
						m_nComPort = _ttoi(strText);
						m_bUsbMode = FALSE;
					}
					j = 0;
				}
			}
		}
		m_cmbFormalPort.SetCurSel(j);
	}
	

	if(!m_bPortCmbAddIndi && (!m_bIsTesting))
	{
		GetDlgItem(IDC_COMBO_FORPORT)->EnableWindow(TRUE);
	}
	m_bPortCmbAddIndi = TRUE;
	return bFindRes;
}

BOOL CWriteFlashDlg::CheckIMEI()
{
    UpdateData();
    if(!m_WriteParam.bChkIMEI&&!m_WriteParam.bChkIMEI2&&!m_WriteParam.bChkBTAddr&&!m_WriteParam.bCheckWifi&&!m_WriteParam.bChkSN1&&!m_WriteParam.bChkSN2&&!m_bOperateCURef)
    {
        m_strMsg.LoadString(IDS_CHECK);
        return FALSE;
    }
    if(m_bReadFun)
    {
        return TRUE;
    }
	
	//检查各IMEI是否相同 -Zijian.Zhu @ 20120302 ##
	
	CString strIMEI(_T(""));
	BOOL bChkInd[4] = {m_WriteParam.bChkIMEI, m_WriteParam.bChkIMEI2, m_WriteParam.bChkIMEI3, m_WriteParam.bChkIMEI4};
	CStringArray arrIMEI;
	arrIMEI.RemoveAll();
	arrIMEI.Add(m_strEditIMEI1);
	arrIMEI.Add(m_strEditIMEI2);
	arrIMEI.Add(m_strEditIMEI3);
	arrIMEI.Add(m_strEditIMEI4);
	BOOL bRep = FALSE;
	for(int n=0; n<4; n++)
	{
		strIMEI = arrIMEI.GetAt(n);
		for(int k=n+1; k<4; k++)
		{
			if(bChkInd[k] && (strIMEI == arrIMEI.GetAt(k))) 
			{
				bRep = TRUE;
				goto _IMEI_REPEAT;
			}
		}
	}
	arrIMEI.RemoveAll();

_IMEI_REPEAT:
	if(bRep && (!(m_WriteParam.bChkUseSameIMEI)))
	{
		m_strMsg.Format(IDS_IMEI_REPEAT);
		return FALSE;
	}

    //检查长度
    if(   (m_WriteParam.bChkIMEI && (m_strEditIMEI1.GetLength() != MAX_IMEI_STR_LENGTH -1)) 
        ||(m_WriteParam.bChkIMEI2&& (m_strEditIMEI2.GetLength() != MAX_IMEI_STR_LENGTH -1))
        ||(m_WriteParam.bChkIMEI3&& (m_strEditIMEI3.GetLength() != MAX_IMEI_STR_LENGTH -1))
        ||(m_WriteParam.bChkIMEI4&& (m_strEditIMEI4.GetLength() != MAX_IMEI_STR_LENGTH -1)))
    {
        m_strMsg.LoadString(IDS_IMEI_LENTH);  
        return FALSE;   
    }  
    if(m_WriteParam.bChkBTAddr&& (m_strEditBT.GetLength()!= BT_ADDR_STR_LENGTH - 1))
    {
        m_strMsg.LoadString(IDS_BT_LENTH);
        return FALSE;   
    }
	if(m_WriteParam.bEnableSNFunc)
	{
		if(  (m_WriteParam.bChkSN1 && (m_strSN1.GetLength() != m_nSNLength)) ||
			(m_WriteParam.bChkSN2 && (m_strSN2.GetLength() != m_nSNLength)))
		{
			m_strMsg.LoadString(IDS_SN_LENTH);  
			return FALSE; 
		}
	}
    //检查wifi
    if(m_WriteParam.bCheckWifi && !(m_WriteParam.bManualAssignWifiAddr))
    {
        if ( !(!m_WriteParam.bChkAutoCreateWifi && m_bCheckWFInput && _tcsclen(m_szFmtWF) > 0) )   // NEWMS00205017
        {
            CString strTemp;
            strTemp= m_strWifi.Left(2);
            if(0!=_tcstol(strTemp, NULL,16)%2)
            {
                 m_strMsg.LoadString(IDS_WIFI_FORMAT);
                 return FALSE;   
            }
            strTemp= m_strWifi.Left(6);
		    if(0 != wcsnicmp(m_WriteParam.sForeWifi.GetBuffer(MAX_PATH), strTemp.GetBuffer(6), 6))
		    {
			    m_strMsg.LoadString(IDS_WIFI_SIX);
			    return FALSE;  
		    }
		    m_WriteParam.sForeWifi.ReleaseBuffer();
		    strTemp.ReleaseBuffer();
        }
    }
    //检查合法性
    if(m_WriteParam.bCheckValid)
    {
        if(m_WriteParam.bChkIMEI && (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI1))
        {
            __int64 int64IMEI =  _ttoi64(m_strEditIMEI1);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
                m_strMsg.Format(IDS_IMEI_ERR, 1);
                return FALSE;
            }
        }    
        if(m_WriteParam.bChkIMEI2 && (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI2))
        {
            __int64 int64IMEI =  _ttoi64(m_strEditIMEI2);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
               // m_strMsg.LoadString(IDS_IMEI_ERR);
                m_strMsg.Format(IDS_IMEI_ERR, 2);
                return FALSE;
            }
        } 
        if(m_WriteParam.bChkIMEI3 && (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI3))
        {
            __int64 int64IMEI =  _ttoi64(m_strEditIMEI3);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
               // m_strMsg.LoadString(IDS_IMEI_ERR);
                m_strMsg.Format(IDS_IMEI_ERR, 3);
                return FALSE;
            }
        } 
          if(m_WriteParam.bChkIMEI4 && (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI4))
        {
            __int64 int64IMEI =  _ttoi64(m_strEditIMEI4);
            __int64 int64CD = ComputeCD(int64IMEI / 10);
            __int64 int64IMEICD = int64IMEI % 10;
            if(int64IMEICD != int64CD)
            {
               // m_strMsg.LoadString(IDS_IMEI_ERR);
                m_strMsg.Format(IDS_IMEI_ERR, 4);
                return FALSE;
            }
        } 
    } 
    if(m_WriteParam.bCheckRepeat)
    {
        //检查重复性
        BOOL bRet=TRUE ;
        if(m_WriteParam.bChkIMEI)
        {
           bRet = SearchImei(1,m_strEditIMEI1);
        }
        if(bRet&& m_WriteParam.bChkIMEI2)
        {
            bRet = SearchImei(2,m_strEditIMEI2);
        }
        if(bRet&& m_WriteParam.bChkIMEI3)
        {
            bRet = SearchImei(3,m_strEditIMEI3);
        }
         if(bRet&& m_WriteParam.bChkIMEI4)
        {
            bRet = SearchImei(4,m_strEditIMEI4);
        }
        if(!bRet)
        {
            return FALSE;
        }
    }

	if(m_WriteParam.bEnableSNFunc)
	{
		if(m_WriteParam.bChkSN1)
		{
			for (int i=0; i<m_strSN1.GetLength(); i++)
			{
				if (!( _istdigit( m_strSN1.GetAt(i)) ||
					((m_strSN1.GetAt(i) >= 0X61 && m_strSN1.GetAt(i) <= 0X7A) //  >'a'  <'z'
					||(m_strSN1.GetAt(i) >= 0x41 && m_strSN1.GetAt(i) <= 0x5A ) )
					))
				{
					m_strMsg.Format(IDS_SN_ERR);
					return FALSE;  
				}
			}
		}
		if(m_WriteParam.bChkSN2)
		{
			for (int i=0; i<m_strSN2.GetLength(); i++)
			{
				if (!( _istdigit( m_strSN2.GetAt(i)) ||
					((m_strSN2.GetAt(i) >= 0X61 && m_strSN2.GetAt(i) <= 0X7A) //  >'a'  <'z'
					||(m_strSN2.GetAt(i) >= 0x41 && m_strSN2.GetAt(i) <= 0x5A ) )
					))
				{
					m_strMsg.Format(IDS_SN_ERR);
					return FALSE;  
				}
			}
		}
	}

    //  [5/17/2012 Xiaoping.Jing] NEWMS00205017 [[[
    if (!m_WriteParam.bChkAutoCreateBT && m_bCheckBTInput && _tcsclen(m_szFmtBT) > 0)
    {
        if (NULL == _tcsstr(m_strEditBT.operator LPCTSTR(), m_szFmtBT))
        {
            CString strFmtMsg = m_szFmtBT;
            for (int i=_tcslen(m_szFmtBT); i<MAX_BT_ADDR_STR_LENGTH-1; i++)
            {
                strFmtMsg += _T("X");
            }
            m_strMsg.Format(IDS_INVALID_INPUT_BTADDR, strFmtMsg);
            return FALSE;
        }
    }

    if (!m_WriteParam.bChkAutoCreateWifi &&m_bCheckWFInput && _tcsclen(m_szFmtWF) > 0)
    {
        if (NULL == _tcsstr(m_strWifi.operator LPCTSTR(), m_szFmtWF))
        {
            CString strFmtMsg = m_szFmtWF;
            for (int i=_tcslen(m_szFmtWF); i<MAX_WIFI_ADDR_STR_LENGTH-1; i++)
            {
                strFmtMsg += _T("X");
            }
            m_strMsg.Format(IDS_INVALID_INPUT_WFADDR, strFmtMsg);
            return FALSE;
        }
    }
    //  [5/17/2012 Xiaoping.Jing] NEWMS00205017 ]]]



	TCHAR szTemp[50]={0};
	unsigned __int64 adcurr=0, adstar=0, adstop=0;
	BOOL bCheckManAss=TRUE;
	if(m_WriteParam.bChkBTAddr && m_WriteParam.bManualAssignBtAddr)
	{
		adcurr = StringHextoDec(m_CustomBtAddrComp.szCurrAddr);
		adstar = StringHextoDec(m_CustomBtAddrComp.szStarAddr);
		adstop = StringHextoDec(m_CustomBtAddrComp.szStopAddr);
		if(adcurr < adstar)
		{
			adcurr = adstar;
			AfxMessageBox(_T("当前蓝牙地址已小于设定的起始地址，点击确定将当前地址置为开始地址。"));
			bCheckManAss = FALSE;
		}
		else if(adcurr > adstop)
		{
			adcurr = adstar;
			AfxMessageBox(_T("当前蓝牙地址已大于设定的结束地址，点击确定将当前地址置为开始地址。"));
			bCheckManAss = FALSE;
		}
		
		if(!bCheckManAss)
		{
			_ui64tot(adcurr, szTemp, 16);
			_stprintf(m_CustomBtAddrComp.szCurrAddr, _T("%012s"), _tcsupr(szTemp));
			WritePrivateProfileString(SZ_CUST_SECT_BTADDR, SZ_CUST_KEY_ADDRCRR, m_CustomBtAddrComp.szCurrAddr, m_WriteParam.strBtAddrPath);
			return FALSE;
		}
	}
	
	if(m_WriteParam.bCheckWifi && m_WriteParam.bManualAssignWifiAddr)
	{
		adcurr = StringHextoDec(m_CustomWifiAddrComp.szCurrAddr);
		adstar = StringHextoDec(m_CustomWifiAddrComp.szStarAddr);
		adstop = StringHextoDec(m_CustomWifiAddrComp.szStopAddr);
		if(adcurr < adstar)
		{
			adcurr = adstar;
			AfxMessageBox(_T("当前WIFI地址已小于设定的起始地址，点击确定将当前地址置为开始地址。"));
			bCheckManAss = FALSE;
		}
		else if(adcurr > adstop)
		{
			adcurr = adstar;
			AfxMessageBox(_T("当前WIFI地址已大于设定的结束地址，点击确定将当前地址置为开始地址。"));
			bCheckManAss = FALSE;
		}
		
		if(!bCheckManAss)
		{
			_ui64tot(adcurr, szTemp, 16);
			_stprintf(m_CustomWifiAddrComp.szCurrAddr, _T("%012s"), _tcsupr(szTemp));
			WritePrivateProfileString(SZ_CUST_SECT_WFADDR, SZ_CUST_KEY_ADDRCRR, m_CustomWifiAddrComp.szCurrAddr, m_WriteParam.strWifiAddrPath);
			return FALSE;
		}
	}

    return TRUE;
}

BOOL CWriteFlashDlg::SearchImei(int nIndex, LPCTSTR strImei)
{
    if(m_WriteParam.bSaveIMEI&&(!m_WriteParam.bSaveToTxt)) 
    {
         if(m_pRecordset)  //lint !e1037
        {
            if(adStateClosed != m_pRecordset->GetState())
            {
                m_pRecordset->Close();
            }
        }
        try
        {
            CString str;
            str.Format(_T("SELECT * FROM IMEI where IMEI=\'%s\'"),strImei);
            m_pRecordset->Open((LPCTSTR)str,_variant_t((IDispatch*)m_pConnection, TRUE),   //lint !e747
                adOpenStatic, adLockOptimistic, adCmdText);
            long lRowCount = m_pRecordset->GetRecordCount();
            m_pRecordset->Close();
            if(0==lRowCount)
            {
                return TRUE;
            }
            else
            {  
                m_strMsg.Format(IDS_REUSE_IMEI,nIndex);
                return FALSE;
            }
            
        }
        
        catch (_com_error)   //lint !e1752
        {
            CString errMessage;
            m_strMsg.LoadString(IDS_OPEN_SAVEFILE_ERR);
            return FALSE;
        }  
    }
    else
    {
        FILE *File = NULL;
        TCHAR tszContext[500];
        TCHAR *str;
        TCHAR tszImei[30];	
        _tcscpy(tszImei, strImei); 
        File = _wfopen (m_szIMEITxtPath, _T("r"));   
        if(NULL != File)
        {
            while(fgetws(tszContext,254,File)!=NULL)
                
            {
                str = _tcsstr(tszContext,tszImei);					   
                if(str != NULL)
                {  
                    m_strMsg.Format(IDS_REUSE_IMEI,nIndex);
                    fclose(File);
                    return FALSE;
                }
            }
            
            fclose(File);
        }
        else
        {
             m_strMsg.LoadString(IDS_OPEN_SAVEFILE_ERR);
             return FALSE; 
        }
        return TRUE;

    }
 
}

BOOL CWriteFlashDlg::SaveImei()
{
    if((!m_WriteParam.bChkIMEI)&&(!m_WriteParam.bChkIMEI2)&&((!m_WriteParam.bChkIMEI3)))
    {
        return TRUE;
    }
    TCHAR szDate[100];
    FILE *File = NULL ;  
    CTime m_TestTime = CTime::GetCurrentTime();
    _stprintf(szDate, _T("%d-%02d-%02d-%02d-%02d-%02d"), m_TestTime.GetYear(), 
        m_TestTime.GetMonth(), m_TestTime.GetDay(), m_TestTime.GetHour(), 
        m_TestTime.GetMinute(), m_TestTime.GetSecond());
    
    if(m_WriteParam.bSaveIMEI&&m_WriteParam.bSaveToTxt)
    { 
        File = _tfopen (m_szIMEITxtPath, _T("a+"));
        if(NULL != File)
        {
            fseek(File, 0, SEEK_END); 
			
            TCHAR szIMEI[256];
            _stprintf(szIMEI,_T("%s, IMEI:%s"),szDate, m_strEditIMEI1.operator LPCTSTR ( ) );
            if(m_WriteParam.bChkIMEI2)
            {
                _tcscat(szIMEI,_T(", IMEI2:"));
                _tcscat(szIMEI,m_strEditIMEI2);
            }
            if(m_WriteParam.bChkIMEI3)
            {
                _tcscat(szIMEI,_T(", IMEI3:"));
                _tcscat(szIMEI,m_strEditIMEI3);
            }
             if(m_WriteParam.bChkIMEI4)
            {
                _tcscat(szIMEI,_T(", IMEI4:"));
                _tcscat(szIMEI,m_strEditIMEI4);
            }
			 if(m_WriteParam.bCheckWifi)
			 {
				 _tcscat(szIMEI,_T(", WIFI:"));
				 _tcscat(szIMEI,m_strWifi);
			 }
			 if(m_WriteParam.bChkBTAddr)
			 {
				 _tcscat(szIMEI,_T(", BT:"));
				 _tcscat(szIMEI,m_strEditBT);
			 }
			 if(m_WriteParam.bEnableSNFunc)
			 {
				 if(m_WriteParam.bChkSN1)
				 {
					 _tcscat(szIMEI,_T(", SN1:"));
					 _tcscat(szIMEI,m_strSN1);
				 }
				 if(m_WriteParam.bChkSN2)
				 {
					 _tcscat(szIMEI,_T(", SN2:"));
					 _tcscat(szIMEI,m_strSN2);
				 }
			 }
            _ftprintf(File,_T("%s\n"),szIMEI);
            fclose(File);
            return TRUE;
        }
        else
        {
            m_strMsg.LoadString(IDS_OPEN_SAVEFILE_ERR);
            DisPlayMessage(STATE_FAIL);
            return FALSE;
        }
    }
    else if(m_WriteParam.bSaveIMEI)
    {
        if(m_pRecordset)  //lint !e1037
        {
            if(adStateClosed != m_pRecordset->GetState())
            {
                m_pRecordset->Close();
            }
        }
        try
        {
            CString str;
            //str.Format(_T("SELECT * FROM IMEI "),strImei);
            m_pRecordset->Open(_T("SELECT * FROM IMEI "),_variant_t((IDispatch*)m_pConnection, TRUE),   //lint !e747
                adOpenStatic, adLockOptimistic, adCmdText);
            m_pRecordset->AddNew();
            m_pRecordset->PutCollect(_T("IMEI"),_variant_t(m_strEditIMEI1)); 
            if(m_WriteParam.bChkIMEI2)
            {
                m_pRecordset->PutCollect(_T("IMEI2"),_variant_t(m_strEditIMEI2));
            }
            if(m_WriteParam.bChkIMEI3)
            {
                m_pRecordset->PutCollect(_T("IMEI3"),_variant_t(m_strEditIMEI3));
            }
			if(m_WriteParam.bChkIMEI4)
            {
                m_pRecordset->PutCollect(_T("IMEI4"),_variant_t(m_strEditIMEI4));
            }
			if(m_WriteParam.bChkBTAddr)
			{
                m_pRecordset->PutCollect(_T("BLUETOOTH ADDR"),_variant_t(m_strEditBT));
			}
			if(m_WriteParam.bCheckWifi)
			{
                m_pRecordset->PutCollect(_T("WIFI MAC ADDR"),_variant_t(m_strWifi));
			}
			if(m_WriteParam.bChkSN1)
			{
                m_pRecordset->PutCollect(_T("SN1"),_variant_t(m_strSN1));
			}
			if(m_WriteParam.bChkSN2)
			{
                m_pRecordset->PutCollect(_T("SN2"),_variant_t(m_strSN2));
			}
            m_pRecordset->PutCollect(_T("Time"),_variant_t(szDate)); 
            m_pRecordset->Update();
			/*
            m_pRecordset->AddNew();
            m_pRecordset->PutCollect(_T("IMEI"),_variant_t(m_strEditIMEI1)); 
            m_pRecordset->PutCollect(_T("Time"),_variant_t(szDate)); 
            m_pRecordset->Update();
            if(m_WriteParam.bChkIMEI2)
            {
				m_pRecordset->AddNew();
                m_pRecordset->PutCollect(_T("IMEI"),_variant_t(m_strEditIMEI2));
                m_pRecordset->PutCollect(_T("Time"),_variant_t(szDate)); 
                m_pRecordset->Update();
            }
            if(m_WriteParam.bChkIMEI3)
            {
				m_pRecordset->AddNew();
                m_pRecordset->PutCollect(_T("IMEI"),_variant_t(m_strEditIMEI3));
                m_pRecordset->PutCollect(_T("Time"),_variant_t(szDate)); 
                m_pRecordset->Update();
            }
			if(m_WriteParam.bChkIMEI4)
            {
				m_pRecordset->AddNew();
                m_pRecordset->PutCollect(_T("IMEI"),_variant_t(m_strEditIMEI4));
                m_pRecordset->PutCollect(_T("Time"),_variant_t(szDate)); 
                m_pRecordset->Update();
            }
			*/
            m_pRecordset->Close();
            return TRUE;
        }
        
        catch (_com_error)  //lint !e1752
        {
             m_strMsg.LoadString(IDS_OPEN_SAVEFILE_ERR);
            DisPlayMessage(STATE_FAIL);
            return FALSE;
        }  
        
    }
    return TRUE;
}


void CWriteFlashDlg::EnableWindow(BOOL bEnabled)
{
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(bEnabled&&m_bCustAddrInvalid);
    GetDlgItem(IDC_COMBO_FORPORT)->EnableWindow(bEnabled);
    if(!m_bAdminMode)
    {
        GetDlgItem(IDC_BUTTON_SET)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_BUTTON_SET)->EnableWindow(bEnabled);
    }
    if(MOCOR_VER_PRE09A37==m_WriteParam.nMocver)       
    {
        GetDlgItem(IDC_BUTTON_READ)->EnableWindow(FALSE);
    }
    else
    { 
        GetDlgItem(IDC_BUTTON_READ)->EnableWindow(bEnabled);
    }

//  ## zijian.zhu @ 20111212 for NEWMS00147831 <<<	
    if((!m_WriteParam.bChkIMEI) || (GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI1))
    {
        m_CEditMainIMEI.EnableWindow(FALSE);      
    }
    else 
    {
        m_CEditMainIMEI.EnableWindow(bEnabled);    
    }

    if((!m_WriteParam.bChkIMEI2) || (GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI2) || m_WriteParam.bChkUseSameIMEI)
    {
        m_CEditSecIMEI.EnableWindow(FALSE);         
    }
    else 
    {
        m_CEditSecIMEI.EnableWindow(bEnabled);   
    }

    if((!m_WriteParam.bChkIMEI3) || (GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI3) || m_WriteParam.bChkUseSameIMEI)
    {
        m_CEditThirdIMEI.EnableWindow(FALSE);         
    }
    else 
    {
        m_CEditThirdIMEI.EnableWindow(bEnabled);   
    }

    if((!m_WriteParam.bChkIMEI4) || (GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI4) || m_WriteParam.bChkUseSameIMEI)
    {
        m_CEditFourIMEI.EnableWindow(FALSE);         
    }
    else 
    {
        m_CEditFourIMEI.EnableWindow(bEnabled);   
    }
//  ## zijian.zhu @ 20111212 for NEWMS00147831 >>>

    if(!m_WriteParam.bChkBTAddr||m_WriteParam.bChkAutoCreateBT)
    {
        m_CEditBT.EnableWindow(FALSE);  
    }
	else if(m_WriteParam.bChkBTAddr&&m_WriteParam.bManualAssignBtAddr)
	{
        m_CEditBT.EnableWindow(FALSE);  
	}
    else 
    {
        m_CEditBT.EnableWindow(bEnabled);   
    }
	
	if(!m_WriteParam.bCheckWifi||m_WriteParam.bChkAutoCreateWifi)
    {
        m_CEditWifi.EnableWindow(FALSE);  
    }
	else if(m_WriteParam.bCheckWifi&&m_WriteParam.bManualAssignWifiAddr)
	{
		m_CEditWifi.EnableWindow(FALSE);  	
	}
    else 
    {
        m_CEditWifi.EnableWindow(bEnabled);   
    }
    
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	if(m_WriteParam.bEnableSNFunc)
	{
		if(!m_WriteParam.bChkSN1 || m_WriteParam.bChkAutoSN1)
		{
			m_CEditSN1.EnableWindow(FALSE);      
		}
		else 
		{
			m_CEditSN1.EnableWindow(bEnabled);    
		}

		if(!m_WriteParam.bChkSN2 || m_WriteParam.bChkAutoSN2)
		{
			m_CEditSN2.EnableWindow(FALSE);      
		}
		else 
		{
			m_CEditSN2.EnableWindow(bEnabled);    
		}
	}
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>
}

void CWriteFlashDlg::GetImeiCodePlus(CString *strRtn, CString strOrg)
{
	TCHAR szAssi1[50]={0}, szAssi2[50]={0};
	CString strlb(_T("")), strhb(_T(""));
	int nLen = strOrg.GetLength(), nCri = 12;
	if(nCri >= nLen)
	{
		unsigned __int64 c = _ttoi64(strOrg)+1;
		_ui64tot(c, szAssi1, 10);
		strRtn->Format(_T("%014s"), szAssi1);
	}
	else
	{
		strlb = strOrg.Right(nCri);
		strhb = strOrg.Left((int)(nLen-nCri));
		unsigned __int64 c1 = _ttoi64(strlb);
		unsigned __int64 c2 = _ttoi64(strhb);
		unsigned __int64 c3 = (unsigned __int64)pow((double)10.0, (int)(nLen-nCri))-1;
		if(999999999999 == c1)
		{
			c1 = 0;
			c2 = (c2 == c3) ? 0 : (c2 + 1);
		}
		else
		{
			c1 += 1;
		}
		_ui64tot(c1, szAssi1, 10);
		_ui64tot(c2, szAssi2, 10);
		strRtn->Format(_T("%02s%012s"), szAssi2, szAssi1);
	}

	TCHAR szTemp[50] = {0};
	__int64 intImeiBase = _ttoi64(*strRtn);
    __int64 int64CD = ComputeCD(intImeiBase);
    __int64 int64IMEI = intImeiBase * 10 + int64CD;
    _i64tot(int64IMEI, szTemp, 10);

	(*strRtn).Format(_T("%015s"), szTemp);
}

void CWriteFlashDlg::OnButtonStart() 
{
    // TODO: Add your control notification handler code here
	m_bReadFun = FALSE;
	OnStartAction();
}

DWORD CWriteFlashDlg::EnterModeProc(LPVOID lpParam)
{
    CWriteFlashDlg *pThis = (CWriteFlashDlg *)lpParam;
    return pThis->EnterModeFunc(0, (long)pThis);
}

DWORD CWriteFlashDlg::EnterModeFunc(WPARAM, LPARAM)
{
    HANDLE hHandle[]={m_hExitThread,m_hEnterModeSuccess};
    DWORD dwRet =0;
    dwRet = WaitForMultipleObjects(2,hHandle,FALSE,m_nTimeout);
    if(WAIT_TIMEOUT == dwRet)
    {
        SP_StopModeProcess(g_hDiagPhone);
        m_strMsg.LoadString(IDS_TIMEOUT);
        DisPlayMessage(STATE_FAIL);
        LogErr(FALSE);
        PostMessage(WM_REFRESH_UI, 0, 0);
    }
    else if(0 == dwRet- WAIT_OBJECT_0)
    {
        ResetEvent(m_hExitThread);
        SP_StopModeProcess(g_hDiagPhone);  
    }         
    else if (1 == dwRet- WAIT_OBJECT_0)
    { 
		if(!m_bCrtTimer)
		{
			m_StaticPortIndicator.SetBkColor(COLOR_PGREEN);
			ModeStaticIndi(m_bOperationMethod, TRUE);
		}

        m_dwStartTime = ::GetTickCount();
        ResetEvent(m_hEnterModeSuccess);
        GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE); 
        if(m_bUsbMode)
        {
            m_nComPort = SP_GetUsbPort(g_hDiagPhone);
        }  
        else
        {
            Sleep(m_dwEnterModeWait);    //串口需要waiting 一定时间才进模式
        }
        m_strMsg.LoadString(IDS_CONNECT);       //connect success
        DisPlayMessage(STATE_WORKING);
        if(m_bReadFun)  //Read Fun
        {
            ReadBack();
            GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
            PostMessage(WM_REFRESH_UI, 0, 0);
        }
        else        //WriteteFun
        {  
            WriteFun();
            GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
            PostMessage(WM_REFRESH_UI, 0, 0);   
        }  
    } 
    SP_EndPhoneTest(g_hDiagPhone);
	if(!m_bCrtTimer)
	{
		m_StaticPortIndicator.SetBkColor(COLOR_ORANGE);
		ModeStaticIndi(m_bOperationMethod, FALSE);
	}

	UpgradeCursorPosition();
	m_bIsTesting = FALSE;
	GetDlgItem(IDC_COMBO_FORPORT)->EnableWindow(TRUE);
    return 0;  
}

void CWriteFlashDlg::UpgradeCursorPosition()
{
	if(m_WriteParam.bChkIMEI && GENCODE_TYP_MANUAL==m_WriteParam.eGenIIMEI1)
	{
		m_CEditMainIMEI.SetFocus();
		m_CEditMainIMEI.SetSel(0, -1);
	}
	else if(m_WriteParam.bChkIMEI && m_WriteParam.bChkIMEI2 && GENCODE_TYP_MANUAL==m_WriteParam.eGenIIMEI2)
	{
		m_CEditSecIMEI.SetFocus();
		m_CEditSecIMEI.SetSel(0, -1);
	}
	else if(m_WriteParam.bChkIMEI && m_WriteParam.bChkIMEI3 && GENCODE_TYP_MANUAL==m_WriteParam.eGenIIMEI3)
	{
		m_CEditThirdIMEI.SetFocus();
		m_CEditThirdIMEI.SetSel(0, -1);
	}
	else if(m_WriteParam.bChkIMEI && m_WriteParam.bChkIMEI4 && GENCODE_TYP_MANUAL==m_WriteParam.eGenIIMEI4)
	{
		m_CEditFourIMEI.SetFocus();
		m_CEditFourIMEI.SetSel(0, -1);
	}
	else if(m_WriteParam.bChkBTAddr && !(m_WriteParam.bChkAutoCreateBT))
	{
		m_CEditBT.SetFocus();
		m_CEditBT.SetSel(0, -1);
	}
	else if(m_WriteParam.bCheckWifi && !(m_WriteParam.bChkAutoCreateWifi))
	{
		m_CEditWifi.SetFocus();
		m_CEditWifi.SetSel(0, -1);
	}
	else if(m_WriteParam.bEnableSNFunc)
	{
		if(m_WriteParam.bChkSN1 && !(m_WriteParam.bChkAutoSN1))
		{
			m_CEditSN1.SetFocus();
			m_CEditSN1.SetSel(0, -1);
		}
		else if(m_WriteParam.bChkSN2 && !(m_WriteParam.bChkAutoSN2))
		{
			m_CEditSN2.SetFocus();
			m_CEditSN2.SetSel(0, -1);
		}
	}
}

void CWriteFlashDlg::WriteFun()
{
    //DWORD dwTicks = ::GetTickCount();
    int nErrorCode = RFT_SUCCESS;
    int nPhaseErr = RFT_SUCCESS;
    
    int nCount=0;
	/*
    if(m_bPreReadImeiCount && m_WriteParam.bChkIMEI4)
    {
        nErrorCode = SP_ReadImeiCount(g_hDiagPhone, &nCount);
        if(nCount!=4)
        {
            m_strMsg.LoadString(IDS_WARNING);
            DisPlayMessage(STATE_FAIL); 
            LogErr(FALSE);
            return;  
        }  
    }
	*/

	//teana hu 2012.10.16
	m_strVersion.Empty();
	if(m_WriteParam.bChkAPVer)
	{
		DWORD dwRet = 0xffffffff;
		if(m_bOperationMethod)
		{
			dwRet = CheckApVer();
		}
		else
		{
			Sleep(7000);
			int nCount = 0;
			do 
			{
				dwRet = CheckApVer();
				if(dwRet != READ_FAIL)
				{
					break;
				}
				else
				{
					Sleep(1000);
					nCount ++;
				}
			} while(nCount < 5);
		}
		
		if(dwRet != EQUAL)
		{
			m_strMsg.Format(IDS_AP_VER_ERR);
			DisPlayMessage(STATE_FAIL);
			return;
		}
	}
	//

    if(m_WriteParam.bCheckVer)   //check version
    {
       if(!CheckVersion())
       {
           return;
       }
    }


	//teana hu 2012.12.17
	if(m_bOperateCURef)
	{
		if(!WriteCUReference())
		{
			return;
		}
	}
	//

    if(m_WriteParam.bChkPhase)
    {
        m_hPhaseCheckObj = CreatePhaseCheckObject(g_hDiagPhone);
        if(m_hPhaseCheckObj!=NULL)
        {    
            // PutProperty(m_hPhaseCheckObj, SPPH_VERSION, SPPH_SP09);
            nErrorCode =CheckPhase(m_hPhaseCheckObj,m_szCurrentStation,m_szCheckStation);
        }
        else
        {
            nErrorCode =-1;
            m_strMsg.LoadString(IDS_CREATPHASE_ERR);
            DisPlayMessage(STATE_FAIL); 
            LogErr(FALSE);
            return;
        } 
        if(RFT_SUCCESS!=nErrorCode)
        {
            PhaseError((SPPH_ERROR)nErrorCode);
            DisPlayMessage(STATE_FAIL); 
            LogErr(FALSE);
            return;
        }
    } 

//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	if(m_WriteParam.bEnableSNFunc)
	{
		char *pszN1 = NULL;
		char *pszN2 = NULL;
		USES_CONVERSION;
		if(m_WriteParam.bChkSN1)
		{
			pszN1 = new char[WIMEI_SN_LENGTH_MAXIMIZE];
			ZeroMemory(pszN1, sizeof(char)*WIMEI_SN_LENGTH_MAXIMIZE);
			strcpy(pszN1,W2A(m_strSN1));  //lint !e668
		}
		if(m_WriteParam.bChkSN2)
		{
			pszN2 =new char[WIMEI_SN_LENGTH_MAXIMIZE];
			ZeroMemory(pszN2, sizeof(char)*WIMEI_SN_LENGTH_MAXIMIZE);
			strcpy(pszN2,W2A(m_strSN2));  //lint !e668
		}

		if(m_WriteParam.bChkSN1)
		{
			nErrorCode = SP_WriteSN(g_hDiagPhone, true,  (const void *)pszN1, m_nSNLength);
		}
		if(RFT_SUCCESS == nErrorCode && m_WriteParam.bChkSN2)
		{
			nErrorCode = SP_WriteSN(g_hDiagPhone, false, (const void *)pszN2, m_nSNLength);
		}

		delete []pszN1;
		delete []pszN2;
		pszN1 = NULL;
		pszN2 = NULL;

		if(RFT_SUCCESS != nErrorCode)
		{
            m_strMsg.Format(IDS_WRITESN_FAIL, nErrorCode);
            DisPlayMessage(STATE_FAIL); 
            LogErr(FALSE);
            return;
		}
	}
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>

	if(m_WriteParam.bChkIMEI || m_WriteParam.bChkIMEI2 || m_WriteParam.bChkIMEI3 || m_WriteParam.bChkIMEI4 || 
	   m_WriteParam.bChkBTAddr || m_WriteParam.bCheckWifi)
	{
		if(MOCOR_VER_PRE09A37==m_WriteParam.nMocver)      
		{ 
			nErrorCode = WriteMoc1();
		}
		else // && MOCOR_VER_AFTER09A37==m_WriteParam.nMocver)
		{
			nErrorCode = WriteMoc2();     
		}
	}

	//teana hu 2012.12.14
// 	if(!OpenCDROM())
// 	{
// 		m_strMsg.LoadString(IDS_OPEN_CDROM_FAIL);
// 		DisPlayMessage(STATE_FAIL); 
// 		LogErr(FALSE);
//         return;
// 	}
	//

	//teana hu 2012.12.19
	if(!CheckATCmd())
	{
		return;
	}
	//

    if(m_WriteParam.bChkPhase)  
    {
        if(RFT_SUCCESS==nErrorCode)
        {
            nPhaseErr =UpdatePhase(m_hPhaseCheckObj,m_szCurrentStation,1,NULL,NULL,NULL);
        }
        else
        {
            nPhaseErr =UpdatePhase(m_hPhaseCheckObj,m_szCurrentStation,0,"Write Fail",NULL,NULL);
        }
        if(RFT_SUCCESS != nErrorCode)
        {
            m_strMsg.Format(IDS_FAIL, nErrorCode);
            DisPlayMessage(STATE_FAIL);   
        }
        if (RFT_SUCCESS != nPhaseErr && RFT_SUCCESS == nErrorCode)
        {
            
            PhaseError((SPPH_ERROR)nPhaseErr);
            DisPlayMessage(STATE_FAIL);
        } 
    }
    if(NULL!=m_hPhaseCheckObj)
    {
        ReleasePhaseCheckObject(m_hPhaseCheckObj);
        m_hPhaseCheckObj = NULL;
    }
    if(RFT_SUCCESS == nErrorCode&&RFT_SUCCESS == nPhaseErr && m_bWriteTime )
    {
        if(::GetTickCount() - m_dwStartTime > (DWORD)m_nWriteTime)  
        {
            m_strMsg.Format(IDS_WRITETIME,m_nWriteTime);
            DisPlayMessage(STATE_FAIL);
            LogErr(FALSE);
            goto _END_ACTO;
        }
    }
    if (RFT_SUCCESS==nErrorCode && RFT_SUCCESS == nPhaseErr  )
    {
        if(!SaveImei())
        {
           LogErr(FALSE);
           goto _END_ACTO;
        }
        else
        {
            m_strMsg.LoadString(IDS_SUCCESS);
            DisPlayMessage(STATE_PASS);
            if(m_WriteParam.bChkAutoIMEI)
            {
                // m_int64DefaultIMEI = m_int64DefaultIMEI+1;
            }
            TCHAR szTempIMEI[50] = {0};
            _i64tot(m_int64DefaultIMEI, szTempIMEI, 10);
            m_WriteParam.sAutoIMEI.Format(_T("%s"),szTempIMEI);
            WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_DEFAULT_IMEI, szTempIMEI, m_szIniPath);//防止强制关闭 
            LogErr(TRUE);
        }
    }
    else// else err 已经在各函数内显示
    {
        LogErr(FALSE);
    }

_END_ACTO:

	if(m_bOperationMethod && m_bDisableAutoUSB)
	{
		SP_SetUsbAutoMode(g_hDiagPhone, false);
	}

//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	TCHAR szTemp[50]={0};
	unsigned __int64 adcurr=0, adstar=0, adstop=0;
	if(m_WriteParam.bChkBTAddr && m_WriteParam.bManualAssignBtAddr)
	{
		adcurr = StringHextoDec(m_CustomBtAddrComp.szCurrAddr);
		adstar = StringHextoDec(m_CustomBtAddrComp.szStarAddr);
		adstop = StringHextoDec(m_CustomBtAddrComp.szStopAddr);
		adcurr += 1;
		if(adcurr < adstar)
		{
			adcurr = adstar;
		}
		else if(adcurr > adstop)
		{
			adcurr = adstar;
			AfxMessageBox(_T("当前蓝牙地址已大于设定的结束地址，点击确定将当前地址置为开始地址。"));
		}
		_ui64tot(adcurr, szTemp, 16);
		_stprintf(m_CustomBtAddrComp.szCurrAddr, _T("%012s"), _tcsupr(szTemp));
		
		WritePrivateProfileString(SZ_CUST_SECT_BTADDR, SZ_CUST_KEY_ADDRCRR, m_CustomBtAddrComp.szCurrAddr, m_WriteParam.strBtAddrPath);
	}
	else if(m_WriteParam.bChkBTAddr && m_WriteParam.bChkAutoCreateBT && m_WriteParam.bGenBtAddrType)
	{
		unsigned __int64 ad = StringHextoDec(m_WriteParam.strInitAddrForBT);
		if(m_int64BtAddrForMaximize != ad)
		{
			ad += 1;
		}
		else
		{
			ad = m_int64BtAddrForMinimize;
		}
		_ui64tot(ad, szTemp, 16);
		m_WriteParam.strInitAddrForBT.Format(_T("%012s"), szTemp);
	}

	if(m_WriteParam.bCheckWifi && m_WriteParam.bManualAssignWifiAddr)
	{
		adcurr = StringHextoDec(m_CustomWifiAddrComp.szCurrAddr);
		adstar = StringHextoDec(m_CustomWifiAddrComp.szStarAddr);
		adstop = StringHextoDec(m_CustomWifiAddrComp.szStopAddr);
		adcurr += 1;
		if(adcurr < adstar)
		{
			adcurr = adstar;
		}
		else if(adcurr > adstop)
		{
			adcurr = adstar;
			AfxMessageBox(_T("当前WIFI地址已大于设定的结束地址，点击确定将当前地址置为开始地址。"));
		}
		_ui64tot(adcurr, szTemp, 16);
		_stprintf(m_CustomWifiAddrComp.szCurrAddr, _T("%012s"), _tcsupr(szTemp));
		
		WritePrivateProfileString(SZ_CUST_SECT_WFADDR, SZ_CUST_KEY_ADDRCRR, m_CustomWifiAddrComp.szCurrAddr, m_WriteParam.strWifiAddrPath);
	}
	else if(m_WriteParam.bCheckWifi && m_WriteParam.bChkAutoCreateWifi && m_WriteParam.strInitAddrForWifi)
	{
		unsigned __int64 ad = StringHextoDec(m_WriteParam.strInitAddrForWifi);
		if(m_int64WifiAddrForMaximize != ad)
		{
			ad += 1;
		}
		else
		{
			ad = m_int64WifiAddrForMinimize;
		}
		_ui64tot(ad, szTemp, 16);
		m_WriteParam.strInitAddrForWifi.Format(_T("%012s"), szTemp);
	}
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>

    return ;
}

int CWriteFlashDlg::WriteMoc1()
{
    USES_CONVERSION;
    int nErrorCode  = RFT_SUCCESS;  
    CString strTemp;
    char szTemp[50]={0};

    FASTNVACCESS_T      nvparam, nvparamass;
    ZeroMemory((void *)&nvparam, sizeof(nvparam));
    ZeroMemory((void *)&nvparamass, sizeof(nvparamass));

	BOOL  bSendDivision = GetDivisionSend();
	if(m_bOperationMethod && bSendDivision)
	{
		if (m_WriteParam.bChkIMEI)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
			strcpy(szTemp,W2A(m_strEditIMEI1));   //lint !e668
			strncpy((char *)nvparam.szImei1, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if (m_WriteParam.bChkIMEI2)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI2;
			strcpy(szTemp,W2A(m_strEditIMEI2));  //lint !e668
			strncpy((char *)nvparam.szImei2, szTemp, MAX_IMEI_STR_LENGTH);
		}
		nErrorCode = SP_NvWriteDirect(g_hDiagPhone, &nvparam);
		if(RFT_SUCCESS!=nErrorCode)
		{  
			if(nErrorCode==PC_ERR_WRITE_IMEI_REJECT)
				m_strMsg.Format(IDS_IMEI_REJECT);
			else
				m_strMsg.Format(IDS_FAIL, nErrorCode);
			DisPlayMessage(STATE_FAIL);  
			goto _WT_COMP;
		} 
		
		if (m_WriteParam.bChkBTAddr)
		{
			nvparamass.dwOperMask |= FNAMASK_WRITE_BTADDR;
			strcpy(szTemp, W2A(m_strEditBT));   //lint !e668
			strncpy((char *)nvparamass.szBTAddr, szTemp, MAX_BT_ADDR_STR_LENGTH);
		}
		nErrorCode = SP_NvWriteDirect(g_hDiagPhone, &nvparamass);
		if(RFT_SUCCESS!=nErrorCode)
		{  
			if(nErrorCode==PC_ERR_WRITE_IMEI_REJECT)
				m_strMsg.Format(IDS_IMEI_REJECT);
			else
				m_strMsg.Format(IDS_FAIL, nErrorCode);
			DisPlayMessage(STATE_FAIL);  
			goto _WT_COMP;
		} 
	}
	else
	{
		if (m_WriteParam.bChkBTAddr)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_BTADDR;
			strcpy(szTemp,W2A(m_strEditBT));   //lint !e668
			strncpy((char *)nvparam.szBTAddr, szTemp, MAX_BT_ADDR_STR_LENGTH);
		}
		if (m_WriteParam.bChkIMEI)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
			strcpy(szTemp,W2A(m_strEditIMEI1));   //lint !e668
			strncpy((char *)nvparam.szImei1, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if (m_WriteParam.bChkIMEI2)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI2;
			strcpy(szTemp,W2A(m_strEditIMEI2));  //lint !e668
			strncpy((char *)nvparam.szImei2, szTemp, MAX_IMEI_STR_LENGTH);
		} 
		nErrorCode = SP_NvWriteDirect(g_hDiagPhone, &nvparam);
		if(RFT_SUCCESS!=nErrorCode)
		{  
			if(nErrorCode==PC_ERR_WRITE_IMEI_REJECT)
				m_strMsg.Format(IDS_IMEI_REJECT);
			else
				m_strMsg.Format(IDS_FAIL, nErrorCode);
			DisPlayMessage(STATE_FAIL);  
			goto _WT_COMP;
		}   
	}

_WT_COMP:
    return nErrorCode;
}


int CWriteFlashDlg::WriteMoc2()
{
    USES_CONVERSION;

    CString strTemp;
    char szTemp[50]={0};
    int nErrorCode  = RFT_SUCCESS;  

    FASTNVACCESS_T      nvparam, nvparamass;
    ZeroMemory((void *)&nvparam, sizeof(nvparam));
    ZeroMemory((void *)&nvparamass, sizeof(nvparamass));
	
	BOOL  bSendDivision = GetDivisionSend();

	if(m_bOperationMethod && bSendDivision)
	{
		if (m_WriteParam.bChkIMEI)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
			strcpy(szTemp,W2A(m_strEditIMEI1));   //lint !e668
			strncpy((char *)nvparam.szImei1, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if (m_WriteParam.bChkIMEI2)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI2;
			strcpy(szTemp,W2A(m_strEditIMEI2));   //lint !e668
			strncpy((char *)nvparam.szImei2, szTemp, MAX_IMEI_STR_LENGTH);
		}	 
		if(m_WriteParam.bChkIMEI3)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI3;
			strcpy(szTemp,W2A(m_strEditIMEI3));  //lint !e668
			strncpy((char *)nvparam.szImei3, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if(m_WriteParam.bChkIMEI4)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI4;
			strcpy(szTemp,W2A(m_strEditIMEI4));  //lint !e668
			strncpy((char *)nvparam.szImei4, szTemp, MAX_IMEI_STR_LENGTH);
		}
		nErrorCode=SP_FastWriteNvInfo(g_hDiagPhone, &nvparam);
		if(RFT_SUCCESS != nErrorCode && !m_WriteParam.bChkPhase)
		{
			if(nErrorCode==PC_ERR_WRITE_IMEI_REJECT)
				m_strMsg.Format(IDS_IMEI_REJECT);
			else
				m_strMsg.Format(IDS_FAIL, nErrorCode);
			DisPlayMessage(STATE_FAIL); 
			goto _WTMOC2_COMP;
		}

		if (m_WriteParam.bChkBTAddr)
		{
			nvparamass.dwOperMask |= FNAMASK_WRITE_BTADDR;
			strcpy(szTemp,W2A(m_strEditBT));     //lint !e668
			strncpy((char *)nvparamass.szBTAddr, szTemp, MAX_BT_ADDR_STR_LENGTH);
		}
		if(m_WriteParam.bCheckWifi)
		{
			nvparamass.dwOperMask |= FNAMASK_WRITE_WIFIADDR;
			strcpy(szTemp,W2A(m_strWifi));  //lint !e668
			strncpy((char *)nvparamass.szWIFIAddr, szTemp, BT_ADDR_STR_LENGTH);
		}
		nErrorCode=SP_FastWriteNvInfo(g_hDiagPhone, &nvparamass);
		if(RFT_SUCCESS != nErrorCode && !m_WriteParam.bChkPhase)
		{
			if(nErrorCode==PC_ERR_WRITE_IMEI_REJECT)
				m_strMsg.Format(IDS_IMEI_REJECT);
			else
				m_strMsg.Format(IDS_FAIL, nErrorCode);
			DisPlayMessage(STATE_FAIL); 
			goto _WTMOC2_COMP;
		}	
	}
	else
	{
		if (m_WriteParam.bChkIMEI)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
			strcpy(szTemp,W2A(m_strEditIMEI1));   //lint !e668
			strncpy((char *)nvparam.szImei1, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if (m_WriteParam.bChkIMEI2)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI2;
			strcpy(szTemp,W2A(m_strEditIMEI2));   //lint !e668
			strncpy((char *)nvparam.szImei2, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if (m_WriteParam.bChkBTAddr)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_BTADDR;
			strcpy(szTemp,W2A(m_strEditBT));     //lint !e668
			strncpy((char *)nvparam.szBTAddr, szTemp, MAX_BT_ADDR_STR_LENGTH);
		}	 
		if(m_WriteParam.bChkIMEI3)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI3;
			strcpy(szTemp,W2A(m_strEditIMEI3));  //lint !e668
			strncpy((char *)nvparam.szImei3, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if(m_WriteParam.bChkIMEI4)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_IMEI4;
			strcpy(szTemp,W2A(m_strEditIMEI4));  //lint !e668
			strncpy((char *)nvparam.szImei4, szTemp, MAX_IMEI_STR_LENGTH);
		}
		if(m_WriteParam.bCheckWifi)
		{
			nvparam.dwOperMask |= FNAMASK_WRITE_WIFIADDR;
			strcpy(szTemp,W2A(m_strWifi));  //lint !e668
			strncpy((char *)nvparam.szWIFIAddr, szTemp, BT_ADDR_STR_LENGTH);
		}
		nErrorCode=SP_FastWriteNvInfo(g_hDiagPhone, &nvparam);
		if(RFT_SUCCESS != nErrorCode && !m_WriteParam.bChkPhase)
		{
			if(nErrorCode==PC_ERR_WRITE_IMEI_REJECT)
				m_strMsg.Format(IDS_IMEI_REJECT);
			else
				m_strMsg.Format(IDS_FAIL, nErrorCode);
			DisPlayMessage(STATE_FAIL); 
			goto _WTMOC2_COMP;
		}
	}

_WTMOC2_COMP:
    return nErrorCode;
}

void CWriteFlashDlg::OnClose() 
{
    // TODO: Add your message handler code here and/or call default
	m_devHound.Stop();
    SaveSetting();
    if(NULL != m_hEnterModeSuccess)
    {
        CloseHandle(m_hEnterModeSuccess);  
    }
    
    if(NULL != m_hExitThread)
    {
        CloseHandle(m_hExitThread);
    }
    if(NULL != m_hEnterModeThread)
    {    
        TerminateThread(m_hEnterModeThread, 0);    
        CloseHandle(m_hEnterModeThread);
        m_hEnterModeThread = NULL;
    }
    if(NULL!= m_pFont)
    {
        m_pFont->DeleteObject();
        delete m_pFont;
        m_pFont= NULL;
    }
    if(NULL!= m_pFont1)
    {
        m_pFont1->DeleteObject();
        delete m_pFont1;
        m_pFont1= NULL;
    }
    if(NULL!= m_pFontPortInd)
    {
        m_pFontPortInd->DeleteObject();
        delete m_pFontPortInd;
        m_pFontPortInd= NULL;
    }
    if(NULL!=m_hPhaseCheckObj)
    {
        ReleasePhaseCheckObject(m_hPhaseCheckObj);
        m_hPhaseCheckObj = NULL;
    }
    if (m_pDcSource)
    {
        m_pDcSource->Release();
        m_pDcSource = NULL;
    }
    
    if( m_pDcsFactory )
    {
        m_pDcsFactory->Release();
        m_pDcsFactory = NULL;  
    }
    
    CDialog::OnClose();
}

void CWriteFlashDlg::SaveSetting()
{  
    CString strTemp;
	strTemp.Format(_T("%d"), m_bOperationMethod);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_OPERMETHOD, strTemp, m_szIniPath);
	strTemp.Format(_T("%d"), m_bDisableAutoUSB);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_DISAUTOUSB, strTemp, m_szIniPath);
	if(!m_bOperationMethod)
	{
		strTemp.Format(_T("%d"), (m_bUsbMode? -1: m_nComPort ));
	}
	else
	{
		strTemp.Format(_T("%d"), m_nComPort);
	}
	WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_PORT, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkIMEI);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_IMEI, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkIMEI2);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_IMEI2, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkIMEI3);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_IMEI3, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkIMEI4);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_IMEI4, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkBTAddr);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_BT_ADDR, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkAutoCreateBT);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_BT, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bCheckWifi);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_WIFI_ADDR, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkAutoCreateWifi);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_WIFI, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkAutoIMEI);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_IMEI, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkUseSameIMEI);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_USE_SAME_IMEI, strTemp, m_szIniPath);   
    strTemp.Format(_T("%d"), m_WriteParam.bChkPhase);
    WritePrivateProfileString(SZ_SECT_GENERAL,SZ_KEY_PHASE_CHECK,strTemp,m_szIniPath); 
    TCHAR szTempBuf[50] = {0};
    if(m_WriteParam.bChkIMEI2 && m_WriteParam.bChkIMEI3 &&m_WriteParam.bChkIMEI4 &&m_WriteParam.bChkAutoIMEI &&(!m_WriteParam.bChkUseSameIMEI))
    {
        m_int64DefaultIMEI= m_int64DefaultIMEI-3;
    }
    else if(m_WriteParam.bChkIMEI2 && m_WriteParam.bChkIMEI3&&m_WriteParam.bChkAutoIMEI &&(!m_WriteParam.bChkUseSameIMEI))
    {
       m_int64DefaultIMEI= m_int64DefaultIMEI-2;
    }
    else if((m_WriteParam.bChkIMEI2||m_WriteParam.bChkIMEI3||m_WriteParam.bChkIMEI4) && m_WriteParam.bChkAutoIMEI &&(!m_WriteParam.bChkUseSameIMEI))
    {
        m_int64DefaultIMEI--;
    }
    _i64tot(m_int64DefaultIMEI, (TCHAR *)szTempBuf, 10); 
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_DEFAULT_IMEI, szTempBuf, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bSaveIMEI);
    WritePrivateProfileString(SZ_SECT_GENERAL, _T("SaveIMEI"), strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bSaveToTxt);
    WritePrivateProfileString(SZ_SECT_GENERAL, _T("SaveToTxt"), strTemp, m_szIniPath);

    strTemp.Format(_T("%d"), m_WriteParam.bCheckValid);
    WritePrivateProfileString(SZ_SECT_GENERAL, _T("CheckValid"), strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bCheckRepeat);
    WritePrivateProfileString(SZ_SECT_GENERAL, _T("CheckRepeat"), strTemp, m_szIniPath);

    WritePrivateProfileString(SZ_SECT_PHONE, SZ_KEY_PHONE_VERSTR, m_WriteParam.sVersion, m_szIniPath); 
    strTemp.Format(_T("%d"), m_WriteParam.bCheckVer);
    WritePrivateProfileString(SZ_SECT_PHONE, SZ_KEY_PHONE_CHKVER, strTemp, m_szIniPath);
	strTemp.Format(_T("%d"), m_WriteParam.nVerLenLowLimit);
	WritePrivateProfileString(SZ_SECT_PHONE, SZ_KEY_PHONE_VERLMT, strTemp, m_szIniPath);

//  ## zijian.zhu @ 20111212 for NEWMS00147831 <<<	
	strTemp.Format(_T("%d"), (int)(m_WriteParam.eGenIIMEI1));
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI1, strTemp, m_szIniPath);
	strTemp.Format(_T("%d"), (int)(m_WriteParam.eGenIIMEI2));
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI2, strTemp, m_szIniPath);
	strTemp.Format(_T("%d"), (int)(m_WriteParam.eGenIIMEI3));
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI3, strTemp, m_szIniPath);
	strTemp.Format(_T("%d"), (int)(m_WriteParam.eGenIIMEI4));
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_GENTYPEIMEI4, strTemp, m_szIniPath);
//  ## zijian.zhu @ 20111212 for NEWMS00147831 >>>	

//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	strTemp.Format(_T("%d"), m_WriteParam.bGenBtAddrType);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_GENBTADDR_TYPE, strTemp, m_szIniPath);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_BT_INIT_ADDR, m_WriteParam.strInitAddrForBT, m_szIniPath);

	strTemp.Format(_T("%d"), m_WriteParam.bGenWifiMacType);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_GENWIFIADDR_TYPE, strTemp, m_szIniPath);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_WIFI_INIT_ADDR, m_WriteParam.strInitAddrForWifi, m_szIniPath);

    strTemp.Format(_T("%d"), m_WriteParam.bEnableSNFunc);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_ENABLE_WRITESN, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkSN1);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_SN1, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkAutoSN1);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_SN1, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkSN2);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_SN2, strTemp, m_szIniPath);
    strTemp.Format(_T("%d"), m_WriteParam.bChkAutoSN2);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AUTO_GEN_SN2, strTemp, m_szIniPath);
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>

//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  {{{
//
    WritePrivateProfileString(SZ_SECT_CUSTOMIZATION, SZ_KEY_MA_BTADDR_PATH, m_WriteParam.strBtAddrPath, m_szIniPath);
	WritePrivateProfileString(SZ_SECT_CUSTOMIZATION, SZ_KEY_MA_WIFIADDR_PATH, m_WriteParam.strWifiAddrPath, m_szIniPath);

//
//  Zijian.Zhu @ 20120725 for NEWMS00226697  }}}
//

	//teana hu 2012.10.16
	strTemp.Format(_T("%d"), m_WriteParam.bChkAPVer);
    WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AP_VER_FLAG, strTemp, m_szIniPath);
	WritePrivateProfileString(SZ_SECT_GENERAL, SZ_KEY_AP_VER, m_WriteParam.strAPVerInput, m_szIniPath);
	//


	//teana hu 2012.12.06
	strTemp.Format(_T("%d"), m_bRebootDUT);
	WritePrivateProfileString(SZ_SECT_CONTROL, SZ_KEY_REBOOT_DUT, strTemp, m_szIniPath);
	//


}

void CWriteFlashDlg::OnButtonStop() 
{
    // TODO: Add your control notification handler code here
	SP_EndPhoneTest(g_hDiagPhone);
	if(m_bOperationMethod)
	{
		if(NULL != m_hVerfConn2Test)
		{
			TerminateThread(m_hVerfConn2Test, 0);
			CloseHandle(m_hVerfConn2Test);
			m_hVerfConn2Test = NULL;
		}
	}
	else
	{
		if(NULL != m_hEnterModeThread)
		{
			SetEvent(m_hExitThread);   
			if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEnterModeThread,	3000))
			{
				TerminateThread(m_hEnterModeThread, 0);
			}
			CloseHandle(m_hEnterModeThread);
			m_hEnterModeThread = NULL;
		}
	}

    m_strMsg.LoadString(IDS_STOP);  
    DisPlayMessage(STATE_IDLE);
    EnableWindow(TRUE);
    ReflashUI();
    if(m_bUseDcs && m_nActualCounts<=m_nTestCounts && (!m_bOperationMethod) && m_pDcSource != NULL)
    {
        m_pDcSource->SetVoltageOutput((float)0,1);
        m_nActualCounts =0;
    }
	
	m_bIsTesting = FALSE;
	if(m_bOperationMethod || (-1 != m_nComPort))
	{
		m_bConnState = EnumCommPort();
		DisplayPortCableInd(m_bConnState);
		TimerAdminMg(TRUE);
	}
	
    return;
}

void CWriteFlashDlg::ReadBack()
{
	USES_CONVERSION;
    CString strTemp(_T(""));
    int nErrorCode= RFT_SUCCESS;


	//teana hu 2012.12.17
	if(m_bOperateCURef)
	{
		if(!ReadCUReference())
		{
			return;
		}
	}
	//

	//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	if(m_WriteParam.bEnableSNFunc)
	{
		BYTE byszN1[WIMEI_SN_LENGTH_MAXIMIZE] = {0};
		BYTE byszN2[WIMEI_SN_LENGTH_MAXIMIZE] = {0};
		if(m_WriteParam.bChkSN1)
		{
			nErrorCode = SP_ReadSN(g_hDiagPhone, true,  (void *)byszN1, sizeof(byszN1));
			if(RFT_SUCCESS != nErrorCode)
			{
				m_strMsg.Format(IDS_WRITESN_FAIL, nErrorCode);
				DisPlayMessage(STATE_FAIL); 
				LogErr(FALSE);
				return;
			}
			m_strSN1 = A2W((const char *)byszN1);
			if(WIMEI_SN_LENGTH_MAXIMIZE < m_strSN1.GetLength())
			{
				m_strSN1 = m_strSN1.Left(WIMEI_SN_LENGTH_MAXIMIZE);
			}
		}
		if(m_WriteParam.bChkSN2)
		{
			nErrorCode = SP_ReadSN(g_hDiagPhone, false, (void *)byszN2, sizeof(byszN2));
			if(RFT_SUCCESS != nErrorCode)
			{
				m_strMsg.Format(IDS_WRITESN_FAIL, nErrorCode);
				DisPlayMessage(STATE_FAIL); 
				LogErr(FALSE);
				return;
			}
			m_strSN2 = A2W((const char *)byszN2);
			if(WIMEI_SN_LENGTH_MAXIMIZE < m_strSN2.GetLength())
			{
				m_strSN2 = m_strSN2.Left(WIMEI_SN_LENGTH_MAXIMIZE);
			}
		}
	}
	//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>

	if(m_WriteParam.bChkIMEI || m_WriteParam.bChkIMEI2 || m_WriteParam.bChkIMEI3 || m_WriteParam.bChkIMEI4 || 
		m_WriteParam.bChkBTAddr || m_WriteParam.bCheckWifi)
	{
		FASTNVACCESS_T      nvparam, nvparamass;
		ZeroMemory((void *)&nvparam, sizeof(nvparam));
		ZeroMemory((void *)&nvparamass, sizeof(nvparamass));
		
		DWORD dwRwMask = 0, dwRwMaskAss = 0;
		BOOL  bSendDivision = FALSE;
		if(m_bOperationMethod)
		{
			bSendDivision = GenerateRWMask(&dwRwMask, &dwRwMaskAss);
		}
		else
		{
			dwRwMask = (DWORD)(FNAMASK_WRITE_IMEI1 | FNAMASK_WRITE_IMEI2 | 
				FNAMASK_WRITE_IMEI3 | FNAMASK_WRITE_IMEI4 | 
				FNAMASK_WRITE_BTADDR | FNAMASK_WRITE_WIFIADDR );
		}
		nvparam.dwOperMask = dwRwMask;
		nvparamass.dwOperMask = dwRwMaskAss;
		
		char  szImei1[MAX_IMEI_STR_LENGTH] = {0};
		char  szImei2[MAX_IMEI_STR_LENGTH] = {0};
		char  szImei3[MAX_IMEI_STR_LENGTH] = {0};
		char  szImei4[MAX_IMEI_STR_LENGTH] = {0};
		char  szBt[MAX_BT_ADDR_STR_LENGTH] = {0};
		char  szWifi[MAX_WIFI_ADDR_STR_LENGTH] = {0};
		if(MOCOR_VER_AFTER09A37==m_WriteParam.nMocver)
		{
			if(!bSendDivision)
			{
				nErrorCode =SP_FastReadNvInfo(g_hDiagPhone, &nvparam);
				if(RFT_SUCCESS==nErrorCode)
				{
					strcpy(szImei1, (const char *)nvparam.szImei1);
					strcpy(szImei2, (const char *)nvparam.szImei2);
					strcpy(szBt,    (const char *)nvparam.szBTAddr);
					strcpy(szImei3, (const char *)nvparam.szImei3);
					strcpy(szImei4, (const char *)nvparam.szImei4);
					strcpy(szWifi,  (const char *)nvparam.szWIFIAddr);
				}
			}
			else
			{
				nErrorCode =SP_FastReadNvInfo(g_hDiagPhone, &nvparam);
				if(RFT_SUCCESS==nErrorCode)
				{
					strcpy(szImei1, (const char *)nvparam.szImei1);
					strcpy(szImei2, (const char *)nvparam.szImei2);
					strcpy(szImei3, (const char *)nvparam.szImei3);
					strcpy(szImei4, (const char *)nvparam.szImei4);
				}
				nErrorCode =SP_FastReadNvInfo(g_hDiagPhone, &nvparamass);
				if(RFT_SUCCESS==nErrorCode)
				{
					strcpy(szBt,    (const char *)nvparamass.szBTAddr);
					strcpy(szWifi,  (const char *)nvparamass.szWIFIAddr);
				}
			}
		}

		CString  strImei1(_T(""));
		CString  strImei2(_T(""));
		CString  strImei3(_T(""));
		CString  strImei4(_T(""));
		CString  strBt(_T(""));
		CString  strWifi(_T(""));
		if(RFT_SUCCESS ==nErrorCode)
		{
			if(m_WriteParam.bChkIMEI)
			{   
				strImei1 = A2W(szImei1); 
			}
			if(m_WriteParam.bChkIMEI2)
			{
				strImei2 = A2W(szImei2); 
			}
			if(m_WriteParam.bChkIMEI3)
			{
				strImei3= A2W(szImei3); 
				for(int i = 0; i < strImei3.GetLength(); i ++)
				{
					if(strImei3.GetAt(i) < '0' || strImei3.GetAt(i) > '9')
					{
						m_strMsg.Format(_T("IMEI3 not exist,may be not support IMEI3"));
						strImei3=_T("");
						DisPlayMessage(STATE_FAIL);  
						return;
					}
				}
			}
			if(m_WriteParam.bChkIMEI4)
			{
				strImei4= A2W(szImei4);  
				for(int i = 0; i < strImei4.GetLength(); i ++)
				{
					if(strImei4.GetAt(i) < '0' || strImei4.GetAt(i) > '9')
					{
						m_strMsg.Format(_T("IMEI4 not exist"));
						strImei4=_T("");
						DisPlayMessage(STATE_FAIL);  
						return;
					}
				}
			}
			if(m_WriteParam.bChkBTAddr)
			{
				strBt = A2W(szBt);
			}
			if(m_WriteParam.bCheckWifi)
			{
				strWifi = A2W(szWifi);
			}

			if(m_WriteParam.bChkIMEIAfterRead)
			{
				BOOL bCompRes = TRUE;
				if(0!=m_strEditIMEI1.GetLength() && 0!=strImei1.CompareNoCase(m_strEditIMEI1) && m_WriteParam.bChkIMEI)
				{
					m_strMsg.LoadString(IDS_CHK_FAIL);
					bCompRes = FALSE;
				}
				if(0!=m_strEditIMEI2.GetLength() && 0!=strImei2.CompareNoCase(m_strEditIMEI2) && m_WriteParam.bChkIMEI2)
				{
					m_strMsg.LoadString(IDS_CHK_FAIL);
					bCompRes = FALSE;
				}
				if(0!=m_strEditIMEI3.GetLength() && 0!=strImei3.CompareNoCase(m_strEditIMEI3) && m_WriteParam.bChkIMEI3)
				{
					m_strMsg.LoadString(IDS_CHK_FAIL);
					bCompRes = FALSE;
				}
				if(0!=m_strEditIMEI4.GetLength() && 0!=strImei4.CompareNoCase(m_strEditIMEI4) && m_WriteParam.bChkIMEI4)
				{
					m_strMsg.LoadString(IDS_CHK_FAIL);	
					bCompRes = FALSE;
				}
				if(0!=m_strEditBT.GetLength() && 0!=strBt.CompareNoCase(m_strEditBT) && m_WriteParam.bChkBTAddr)
				{
					m_strMsg.LoadString(IDS_CHK_FAIL);
					bCompRes = FALSE;
				}
				if(0!=m_strWifi.GetLength() && 0!=strWifi.CompareNoCase(m_strWifi) && m_WriteParam.bCheckWifi)
				{
					m_strMsg.LoadString(IDS_CHK_FAIL);
					bCompRes = FALSE;
				}
				if(bCompRes)
				{
					if(!ConclNullValonCheck())
					{
						m_strMsg.LoadString(IDS_CHK_PASS);
					}
					else
					{
						m_strMsg.LoadString(IDS_READ_PASS);						
					}
				}
				
				if(m_WriteParam.bChkIMEI)
				{
					m_strMsg += _T("\nIMEI1:");
					m_strMsg += strImei1;
					m_strMsg += _T("  Check:");
					m_strMsg += m_strEditIMEI1;
				}
				if(m_WriteParam.bChkIMEI2)
				{
					m_strMsg += _T("\nIMEI2:");
					m_strMsg += strImei2;
					m_strMsg += _T("  Check:");
					m_strMsg += m_strEditIMEI2;
				}
				if(m_WriteParam.bChkIMEI3)
				{
					m_strMsg += _T("\nIMEI3:");
					m_strMsg += strImei3;
					m_strMsg += _T("  Check:");
					m_strMsg += m_strEditIMEI3;
				}
				if(m_WriteParam.bChkIMEI4)
				{
					m_strMsg += _T("\nIMEI4:");
					m_strMsg += strImei4;
					m_strMsg += _T("  Check:");
					m_strMsg += m_strEditIMEI4;
				}
				if(m_WriteParam.bChkBTAddr)
				{
					m_strMsg += _T("\nBT:");
					m_strMsg += strBt;
					m_strMsg += _T("  Check:");
					m_strMsg += m_strEditBT;
				}
				if(m_WriteParam.bCheckWifi)
				{
					m_strMsg += _T("\nWIFI:");
					m_strMsg += strWifi;
					m_strMsg += _T("  Check:");
					m_strMsg += m_strWifi;
				}

				if(bCompRes)
				{
					m_StaticMsg.SetBkColor(COLOR_PASS);
				}
				else
				{
					m_StaticMsg.SetBkColor(COLOR_FAIL);
				}
			}
			else
			{
				m_strEditIMEI1 = strImei1;
				m_strEditIMEI2 = strImei2;
				m_strEditIMEI3 = strImei3;
				m_strEditIMEI4 = strImei4;
				m_strEditBT = strBt;
				m_strWifi = strWifi;
				m_strMsg.LoadString(IDS_READ_PASS);
				DisPlayMessage(STATE_PASS); 
			}
		}
		else
		{
			m_strMsg .LoadString(IDS_READ_FAIL);
			DisPlayMessage(STATE_FAIL); 
			return;
		}
	}

    return;
}

BOOL CWriteFlashDlg::ConclNullValonCheck()
{
	BOOL bNull = TRUE;
	if(0!=m_strEditIMEI1.GetLength() && m_WriteParam.bChkIMEI)
	{
		bNull = FALSE;
	}
	if(0!=m_strEditIMEI2.GetLength() && m_WriteParam.bChkIMEI2)
	{
		bNull = FALSE;
	}
	if(0!=m_strEditIMEI3.GetLength() && m_WriteParam.bChkIMEI3)
	{
		bNull = FALSE;
	}
	if(0!=m_strEditIMEI4.GetLength() && m_WriteParam.bChkIMEI4)
	{
		bNull = FALSE;
	}
	if(0!=m_strWifi.GetLength() && m_WriteParam.bCheckWifi)
	{
		bNull = FALSE;
	}
	if(0!=m_strEditBT.GetLength() && m_WriteParam.bChkBTAddr)
	{
		bNull = FALSE;
	}
	return bNull;
}

LRESULT CWriteFlashDlg::OnRefreshUI(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    m_bEnter = FALSE;
    EnableWindow(TRUE);
	OnCleanCodeEdit();
    if(!m_bReadFun)
    {
        ReflashUI();
    }
    else
    {
        UpdateData(FALSE);
        m_bReadFun = FALSE; 
        GetDlgItem(IDC_BUTTON_READ)->SetFocus();
    }
    if(m_bUseDcs && m_nActualCounts<=m_nTestCounts && (!m_bOperationMethod) && m_pDcSource != NULL)
    {
        m_pDcSource->SetVoltageOutput((float)(-1.0));
        m_nActualCounts++;
        Sleep(2000);
		if(m_nActualCounts<=m_nTestCounts)
		{
			OnButtonStart();
		}
		else
		{
			m_nActualCounts = 0;
		}
        
    }

	UpgradeCursorPosition();
    
    return 1L;
}

void CWriteFlashDlg::OnChangeEditImeimain() 
{
    m_bEnter = FALSE;
    CString strTmp ;
    m_CEditMainIMEI.GetWindowText(strTmp);
    for(int i = 0; i < strTmp.GetLength(); i ++)
    {
        if(strTmp.GetAt(i) < '0' || strTmp.GetAt(i) > '9')
        {
            m_CEditMainIMEI.SetWindowText(_T(""));
            return;
        }
    }
    UpdateData(); 
    int nLength = m_strEditIMEI1.GetLength();
    if (nLength == MAX_IMEI_STR_LENGTH - 1)
    {
        if(m_WriteParam.bChkIMEI2&&m_WriteParam.bChkUseSameIMEI&&(!m_WriteParam.bChkAutoIMEI))
        {
           m_CEditSecIMEI.SetWindowText(m_strEditIMEI1);
           m_strEditIMEI2= m_strEditIMEI1;
        }
        if(m_WriteParam.bChkIMEI3&&m_WriteParam.bChkUseSameIMEI&&(!m_WriteParam.bChkAutoIMEI))
        {
           m_CEditThirdIMEI.SetWindowText(m_strEditIMEI1);
           m_strEditIMEI3= m_strEditIMEI1;
        }
        if(m_WriteParam.bChkIMEI4&&m_WriteParam.bChkUseSameIMEI&&(!m_WriteParam.bChkAutoIMEI))
        {
           m_CEditFourIMEI.SetWindowText(m_strEditIMEI1);
           m_strEditIMEI4= m_strEditIMEI1;
        }
        if(m_WriteParam.bChkIMEI2&&(!m_WriteParam.bChkUseSameIMEI)&&(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI2)&&(m_strEditIMEI2.GetLength()!=MAX_IMEI_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_IMEISENIOR)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bChkIMEI3&&(!m_WriteParam.bChkUseSameIMEI)&&(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI3)&&(m_strEditIMEI3.GetLength()!=MAX_IMEI_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_IMEITHIRD)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bChkIMEI4&&(!m_WriteParam.bChkUseSameIMEI)&&(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI4)&&(m_strEditIMEI4.GetLength()!=MAX_IMEI_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_IMEIFOUR)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bChkBTAddr&&(!m_WriteParam.bChkAutoCreateBT)&&(m_strEditBT.GetLength()!=MAX_BT_ADDR_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_BT)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bCheckWifi&&(!m_WriteParam.bChkAutoCreateWifi)&&(m_strWifi.GetLength()!=MAX_WIFI_ADDR_STR_LENGTH-1))//wifi lenth = BT_ADDR_STR_LENGTH
        {
            GetDlgItem(IDC_EDIT_WIFI)->SetFocus();
            m_bEnter = FALSE;
        }
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN1&&(!m_WriteParam.bChkAutoSN1)&&(m_strSN1.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNMAIN)->SetFocus();
            m_bEnter = FALSE;
		}
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2)&&(m_strSN2.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNSENIOR)->SetFocus();
            m_bEnter = FALSE;
		}
        else
        {
            m_bEnter = TRUE;
        }
    } 
}

void CWriteFlashDlg::OnChangeEditImeisenior() 
{
    m_bEnter = FALSE;
    CString strTmp ;
    m_CEditSecIMEI.GetWindowText(strTmp);
    for(int i = 0; i < strTmp.GetLength(); i ++)
    {
        if(strTmp.GetAt(i) < '0' || strTmp.GetAt(i) > '9')
        {
            m_CEditSecIMEI.SetWindowText(_T(""));
            return;
        }
    }
    UpdateData();  
    int nLength = m_strEditIMEI2.GetLength();
    if (nLength == MAX_IMEI_STR_LENGTH - 1)
    {
        if(m_WriteParam.bChkIMEI3&&(!m_WriteParam.bChkUseSameIMEI)&&(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI3)&&(m_strEditIMEI3.GetLength()!=MAX_IMEI_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_IMEITHIRD)->SetFocus();
            m_bEnter = FALSE;
        }
         else if(m_WriteParam.bChkIMEI4&&(!m_WriteParam.bChkUseSameIMEI)&&(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI4)&&(m_strEditIMEI4.GetLength()!=MAX_IMEI_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_IMEIFOUR)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bChkBTAddr&&(!m_WriteParam.bChkAutoCreateBT)&&(m_strEditBT.GetLength()!=MAX_BT_ADDR_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_BT)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bCheckWifi&&(!m_WriteParam.bChkAutoCreateWifi)&&(m_strWifi.GetLength()!=MAX_WIFI_ADDR_STR_LENGTH-1)) //wifi lenth = BT_ADDR_STR_LENGTH
        {
           
            GetDlgItem(IDC_EDIT_WIFI)->SetFocus();
            m_bEnter = FALSE;
        }
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN1&&(!m_WriteParam.bChkAutoSN1)&&(m_strSN1.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNMAIN)->SetFocus();
            m_bEnter = FALSE;
		}
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2)&&(m_strSN2.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNSENIOR)->SetFocus();
            m_bEnter = FALSE;
		}
        else
        {
            m_bEnter = TRUE;
        }
    }
}

void CWriteFlashDlg::OnChangeEditImeithird() 
{
	 m_bEnter = FALSE;
    CString strTmp ;
    m_CEditThirdIMEI.GetWindowText(strTmp);
    for(int i = 0; i < strTmp.GetLength(); i ++)
    {
        if(strTmp.GetAt(i) < '0' || strTmp.GetAt(i) > '9')
        {
            m_CEditThirdIMEI.SetWindowText(_T(""));
            return;
        }
    }
    UpdateData();  
    int nLength = m_strEditIMEI3.GetLength();
    if (nLength == MAX_IMEI_STR_LENGTH - 1)
    {
        if(m_WriteParam.bChkIMEI4&&(!m_WriteParam.bChkUseSameIMEI)&&(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI4)&&(m_strEditIMEI4.GetLength()!=MAX_IMEI_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_IMEIFOUR)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bChkBTAddr&&(!m_WriteParam.bChkAutoCreateBT)&&(m_strEditBT.GetLength()!=MAX_BT_ADDR_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_BT)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bCheckWifi&&(!m_WriteParam.bChkAutoCreateWifi)&&(m_strWifi.GetLength()!=MAX_WIFI_ADDR_STR_LENGTH-1)) //wifi lenth = BT_ADDR_STR_LENGTH
        {
           
            GetDlgItem(IDC_EDIT_WIFI)->SetFocus();
            m_bEnter = FALSE;
        }
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN1&&(!m_WriteParam.bChkAutoSN1)&&(m_strSN1.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNMAIN)->SetFocus();
            m_bEnter = FALSE;
		}
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2)&&(m_strSN2.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNSENIOR)->SetFocus();
            m_bEnter = FALSE;
		}
        else
        {
            m_bEnter = TRUE;
        }
    }
}

void CWriteFlashDlg::OnChangeEditImeiFour() 
{
    m_bEnter = FALSE;
    CString strTmp ;
    m_CEditFourIMEI.GetWindowText(strTmp);
    for(int i = 0; i < strTmp.GetLength(); i ++)
    {
        if(strTmp.GetAt(i) < '0' || strTmp.GetAt(i) > '9')
        {
            m_CEditFourIMEI.SetWindowText(_T(""));
            return;
        }
    }
    UpdateData();  
    int nLength = m_strEditIMEI4.GetLength();
    if (nLength == MAX_IMEI_STR_LENGTH - 1)
    {
        if(m_WriteParam.bChkBTAddr&&(!m_WriteParam.bChkAutoCreateBT)&&(m_strEditBT.GetLength()!=BT_ADDR_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_BT)->SetFocus();
            m_bEnter = FALSE;
        }
        else if(m_WriteParam.bCheckWifi&&(!m_WriteParam.bChkAutoCreateWifi)&&(m_strWifi.GetLength()!=BT_ADDR_STR_LENGTH-1)) //wifi lenth = BT_ADDR_STR_LENGTH
        {
           
            GetDlgItem(IDC_EDIT_WIFI)->SetFocus();
            m_bEnter = FALSE;
        }
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN1&&(!m_WriteParam.bChkAutoSN1)&&(m_strSN1.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNMAIN)->SetFocus();
            m_bEnter = FALSE;
		}
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2)&&(m_strSN2.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNSENIOR)->SetFocus();
            m_bEnter = FALSE;
		}
        else
        {
            m_bEnter = TRUE;
        }
    }
}
void CWriteFlashDlg::OnChangeEditBt() 
{
    m_bEnter = FALSE;
    CString strTmp ;
    m_CEditBT.GetWindowText(strTmp);
    for(int i = 0; i < strTmp.GetLength(); i ++)
    {
        if(!((strTmp.GetAt(i) >= 0X61 && strTmp.GetAt(i) <= 0X66)||(strTmp.GetAt(i) >= 0x41 && strTmp.GetAt(i) <= 0x46 ) \
            ||( _istdigit( strTmp.GetAt(i))) )) ////a~f,A~F,0,9
        {
            m_CEditBT.SetWindowText(_T(""));
            return;
        }
    }
    UpdateData();  
    int nLength = m_strEditBT.GetLength();
    if (nLength == BT_ADDR_STR_LENGTH - 1)
    {
        if(m_WriteParam.bCheckWifi&&(!m_WriteParam.bChkAutoCreateWifi)&&(m_strWifi.GetLength()!=BT_ADDR_STR_LENGTH-1))
        {
            GetDlgItem(IDC_EDIT_WIFI)->SetFocus();
             m_bEnter = FALSE;
        }
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN1&&(!m_WriteParam.bChkAutoSN1)&&(m_strSN1.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNMAIN)->SetFocus();
            m_bEnter = FALSE;
		}
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2)&&(m_strSN2.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNSENIOR)->SetFocus();
            m_bEnter = FALSE;
		}
        else
        {
            m_bEnter = TRUE;
        }
    }
}

void CWriteFlashDlg::OnChangeEditWifi() 
{
    m_bEnter = FALSE;
    CString strTmp ;
    m_CEditWifi.GetWindowText(strTmp);
    for(int i = 0; i < strTmp.GetLength(); i ++)
    {
        if(!((strTmp.GetAt(i) >= 0X61 && strTmp.GetAt(i) <= 0X66)||(strTmp.GetAt(i) >= 0x41 && strTmp.GetAt(i) <= 0x46 ) \
            ||( _istdigit( strTmp.GetAt(i))) )) ////a~f,A~F,0,9
        {
			m_CEditWifi.SetWindowText(_T(""));
            return;
        }
    }
    UpdateData();  
    int nLength = m_strWifi.GetLength();
    if (nLength == BT_ADDR_STR_LENGTH - 1)
    {
		if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN1&&(!m_WriteParam.bChkAutoSN1)&&(m_strSN1.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNMAIN)->SetFocus();
            m_bEnter = FALSE;
		}
		else if(m_WriteParam.bEnableSNFunc&&m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2)&&(m_strSN2.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNSENIOR)->SetFocus();
            m_bEnter = FALSE;
		}
		else
		{
			m_bEnter = TRUE;
		}
    }
}

void CWriteFlashDlg::OnChangeEditSnmain() 
{
	this->UpdateData();
    m_bEnter = FALSE;
    int nLength = m_strSN1.GetLength();
	if(m_nSNLength < nLength)
	{
		m_strSN1 = m_strSN1.Left(m_nSNLength);
		m_CEditSN1.SetWindowText(m_strSN1);
		m_CEditSN1.SetSel(m_nSNLength, m_nSNLength);
	}
    if (m_nSNLength == nLength)
	{
		if(m_WriteParam.bChkSN2&&(!m_WriteParam.bChkAutoSN2)&&(m_strSN2.GetLength()!=m_nSNLength))
		{
            GetDlgItem(IDC_EDIT_SNSENIOR)->SetFocus();
            m_bEnter = FALSE;
		}
		else
		{
			m_bEnter = TRUE;
		}
	}
	this->UpdateData(FALSE);
}

void CWriteFlashDlg::OnChangeEditSnsenior() 
{
	this->UpdateData();
    m_bEnter = FALSE;
    int nLength = m_strSN2.GetLength();
	if(m_nSNLength < nLength)
	{
		m_strSN2 = m_strSN2.Left(m_nSNLength);
		m_CEditSN2.SetWindowText(m_strSN2);
		m_CEditSN2.SetSel(m_nSNLength, m_nSNLength);
	}
    if (m_nSNLength == nLength)
	{
		m_bEnter = TRUE;
	}
	this->UpdateData(FALSE);
}


void CWriteFlashDlg::OnButtonRead() 
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_BUTTON_READ)->EnableWindow(FALSE);
	if(!m_WriteParam.bChkIMEIAfterRead)
	{
		m_strEditIMEI1 =_T("");
		m_strEditIMEI2 =_T("");
		m_strEditIMEI3 =_T("");
		m_strEditIMEI4 =_T("");
		m_strEditBT =_T("");
		m_strWifi =_T("");
		m_strSN1 =_T("");
		m_strSN2 =_T("");
	}
	m_bReadFun = TRUE;
	OnStartAction();
}

void CWriteFlashDlg::DisPlayMessage(TESTSTAT_E eState)
{
   if(eState == STATE_FAIL || eState ==STATE_PASS)
    {
	   TCHAR szMsg[256];
	   _stprintf(szMsg,_T("%s\n"),m_strMsg.operator LPCTSTR ( ));
	   if(m_WriteParam.bChkIMEI)
	   {
           _tcscat(szMsg,_T("IMEI1:"));
           _tcscat(szMsg,m_strEditIMEI1);
           _tcscat(szMsg,_T("\n"));
	   }
	   if(m_WriteParam.bChkIMEI2)
	   {
		   _tcscat(szMsg,_T("IMEI2:"));
		   _tcscat(szMsg,m_strEditIMEI2);
		   _tcscat(szMsg,_T("\n"));
	   }
	   if(m_WriteParam.bChkIMEI3)
	   {
		   _tcscat(szMsg,_T("IMEI3:"));
		   _tcscat(szMsg,m_strEditIMEI3);
		   _tcscat(szMsg,_T("\n"));
	   }
	   if(m_WriteParam.bChkIMEI4)
	   {
		   _tcscat(szMsg,_T("IMEI4:"));
		   _tcscat(szMsg,m_strEditIMEI4);
		   _tcscat(szMsg,_T("\n"));
	   }
	   if(m_WriteParam.bChkBTAddr)
	   {
		   _tcscat(szMsg,_T("BT:"));
		   _tcscat(szMsg,m_strEditBT);
		   _tcscat(szMsg,_T("\n"));
	   }
	   if(m_WriteParam.bCheckWifi)
	   {
		   _tcscat(szMsg,_T("Wifi:"));
		   _tcscat(szMsg,m_strWifi);
		   _tcscat(szMsg,_T("\n"));
	   }
	   if(m_WriteParam.bEnableSNFunc)
	   {
		   if (m_WriteParam.bChkSN1)
		   {
			   _tcscat(szMsg,_T("SN1:"));
			   _tcscat(szMsg,m_strSN1);
			   _tcscat(szMsg,_T("\n"));
			   
		   }
		   if (m_WriteParam.bChkSN2)
		   {
			   _tcscat(szMsg,_T("SN2:"));
			   _tcscat(szMsg,m_strSN2);
			   _tcscat(szMsg,_T("\n"));
		   }
	   }
       m_strMsg.Format(_T("%s"),szMsg);
    }
    m_StaticMsg.SetWindowText(m_strMsg);
    switch (eState)
    {
    case STATE_IDLE:
        m_StaticMsg.SetBkColor(COLOR_IDLE);
        break;
    case STATE_WORKING:
        m_StaticMsg.SetBkColor(COLOR_WORKING);
        break;
    case STATE_PASS:
        m_StaticMsg.SetBkColor(COLOR_PASS);
        break;
    case STATE_FAIL:
        m_StaticMsg.SetBkColor(COLOR_FAIL);
        break;
    default:
        m_StaticMsg.SetBkColor(COLOR_IDLE);
        break;
    }
}

void CWriteFlashDlg::DisplayPortCableInd(BOOL bConn)
{
	if(bConn)
	{
		m_StaticPortIndicator.SetBkColor(COLOR_PGREEN);
		ModeStaticIndi(m_bOperationMethod, TRUE);
	}
	else
	{
		m_StaticPortIndicator.SetBkColor(COLOR_ORANGE);
		ModeStaticIndi(m_bOperationMethod, FALSE);
	}
}


BOOL CAboutDlg::OnInitDialog() 
{
    CDialog::OnInitDialog(); 
    // TODO: Add extra initialization here
    GetDlgItem( IDC_STATIC_VERSION )->SetWindowText( theApp.m_strVersion );    
    GetDlgItem( IDC_STATIC_BUILD )->SetWindowText( theApp.m_strBuild );
    return TRUE;  
}


void CWriteFlashDlg::PhaseError(SPPH_ERROR ePhaseErr )
{
   USES_CONVERSION;  
    switch(ePhaseErr)
    {
        
    case SPPH_SUCC:
        m_strMsg.LoadString(IDS_SPPH_SUCC);
        break;
    case SPPH_FAIL:
        m_strMsg.LoadString(IDS_SPPH_FAIL);
        break;
    case SPPH_INVALID_SPPH_OBJECT:
        m_strMsg.LoadString(IDS_SPPH_INVALID_SPPH_OBJECT);
        break;
    case SPPH_INVALID_PRODUCTINFO:
        m_strMsg.LoadString(IDS_INVALID_PRODUCTINFO);
        break; 
    case SPPH_INVALID_STATION_NAME:
    case SPPH_STATION_NOT_EXIST:
        m_strMsg.LoadString(IDS_SPPH_STATION_NOT_EXIST);
        break;
    case SPPH_STATION_NOT_PASSED:
        m_strMsg.Format(IDS_SPPH_STATION_NOT_PASSED,A2W(m_szCheckStation));
        break;
     case SPPH_STATION_NOT_TESTED:
        m_strMsg.Format(IDS_SPPH_NOT_TESTED,A2W(m_szCheckStation));
        break; 
  
    case SPPH_WRITE_PRODUCTINFO_FAIL:
        m_strMsg.LoadString(IDS_SPPH_WRITE_PRODUCTINFO_FAIL);
        break;
    case SPPH_READ_PRODUCTINFO_FAIL:
        m_strMsg.LoadString(IDS_SPPH_READ_PRODUCTINFO_FAIL);
        break;
    case SPPH_READ_MAGIC_NUMBER_FAIL:
        m_strMsg.LoadString(IDS_SPPH_READ_MAGIC_NUMBER_FAIL);
        break; 
    case SPPH_BUFFER_TOO_SMALL:
         m_strMsg.LoadString(IDS_SPPH_BUFFER_SMALL);
        break; 
    case SPPH_INVALID_MAGIC_VERSION:
           m_strMsg.LoadString(IDS_SPPH_INVALID_MAGIC);
        break; 
    default:
        m_strMsg.LoadString(IDS_SPPH_DEFAULT);
        break;
    }
}

void CWriteFlashDlg::LogErr(BOOL bPass)
{ 
    if(!m_WriteParam.bChkIMEI||!m_WriteParam.bChkAutoIMEI)
    {
        m_CEditMainIMEI.SetWindowText(_T(""));
    }
    if(!m_WriteParam.bChkIMEI2||!m_WriteParam.bChkAutoIMEI)
    {
        m_CEditSecIMEI.SetWindowText(_T(""));
    }
    if(!m_WriteParam.bChkIMEI3||!m_WriteParam.bChkAutoIMEI)
    {
        m_CEditThirdIMEI.SetWindowText(_T(""));
    }
     if(!m_WriteParam.bChkIMEI4||!m_WriteParam.bChkAutoIMEI)
    {
        m_CEditFourIMEI.SetWindowText(_T(""));
    }
    if(!m_WriteParam.bChkBTAddr||!m_WriteParam.bChkAutoCreateBT)
    {
        m_CEditBT.SetWindowText(_T(""));
    }
    if(!m_WriteParam.bCheckWifi||!m_WriteParam.bChkAutoCreateWifi)
    {
      m_CEditWifi.SetWindowText(_T(""));   
    }
	if(!m_WriteParam.bChkSN1||!m_WriteParam.bChkAutoSN1)
	{
		m_CEditSN1.SetWindowText(_T(""));
	}
	if(!m_WriteParam.bChkSN2||!m_WriteParam.bChkAutoSN2)
	{
		m_CEditSN2.SetWindowText(_T(""));
	}

     if(bPass)
    {
       m_nPassCounts++;
    }
    else
    {
       m_nFailCounts++;
    }
    if(m_bUseDcs && (!m_bOperationMethod))     //循环测试打log
    {
        CSpLog log;
        log.Open(_T("IMEI.log"), ISpLog::modeCreate|ISpLog::modeNoTruncate|ISpLog::typeText); //lint !e655
        if(bPass)
        {
            log.LogFmtStr(SPLOGLV_INFO,_T("%d,%s\n"),m_nActualCounts,_T("PASS"));
        }
        else
        {
            log.LogFmtStr(SPLOGLV_INFO,_T("%d,%s\n"),m_nActualCounts,m_strMsg.operator LPCTSTR());
        }
        log.Close();
    }
}

void CWriteFlashDlg::ConnectDatabase()
{
     ::CoInitialize(NULL);
	
	m_pConnection.CreateInstance(__uuidof(Connection));
	m_pRecordset.CreateInstance(__uuidof(Recordset));
    TCHAR szConnection[256];
    _tcscpy(szConnection,_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source="));
    _tcscat(szConnection,m_szIMEIMdbPath);
    if(m_pConnection)        //lint !e1037
    { 
        if( adStateClosed != m_pConnection->GetState())
        {
            m_pConnection->Close();
        }
    }
    m_pConnection->ConnectionTimeout = 10;
    m_pConnection->ConnectionString = _bstr_t(szConnection);
    
    try
    {
        m_pConnection->Open(_T(""),_T(""),_T(""),adModeUnknown);
    }
    catch (_com_error)  //lint !e1752
    {
      
        CString errMessage;
        errMessage.LoadString(IDS_OPEN_SAVEFILE_ERR);
        AfxMessageBox(errMessage);	
        return;
    }
      
}

void CWriteFlashDlg::OnOK() 
{
    // TODO: Add extra validation here
    
    //	CDialog::OnOK();
}

void CWriteFlashDlg::OpenFile()
{
    FILE *File = NULL ;
    File = _tfopen (m_szIMEITxtPath, _T("a+"));  //新建或者打开一个文件
    if(NULL != File)
    {
        fclose(File);
    }
    else
    {
        CString errMessage;
        errMessage.LoadString(IDS_OPEN_SAVEFILE_ERR);
        AfxMessageBox(errMessage);	
    }
    return;
}

void CWriteFlashDlg::CloseFile()
{
    if(NULL != m_pRecordset)
	{
        if(adStateClosed != m_pRecordset->GetState())
        {
            m_pRecordset->Close();	
        }
        m_pRecordset.Release();
	}	
	if(NULL != m_pConnection)
	{
        if(adStateClosed != m_pConnection->GetState())
        {
            m_pConnection->Close();
        }
		m_pConnection.Release();
	}
	
	::CoUninitialize();
}

BOOL CWriteFlashDlg::CheckVersion()
{
    USES_CONVERSION;
    char sVersion[500]; 
    int nlen=0;	

	if(!m_bOperationMethod && (0 == m_nTestMode))
	{
		if (!_SPOK(SP_FastReadVersion(g_hDiagPhone, (void *)sVersion, sizeof(sVersion))))
		{
			m_strMsg.LoadString(IDS_VERSION_READ);
			m_StaticMsg.SetWindowText(m_strMsg);
			m_StaticMsg.SetBkColor(COLOR_FAIL);
			LogErr(FALSE);
			return FALSE;
		}
		else
		{
			CString  strTemp;
			strTemp.Format(_T("%s"), A2W(sVersion));
			//teana hu 2012.12.17
			
			int nIndex = strTemp.Find(_T("\n"));
			if(nIndex == 0)
			{
				strTemp.Delete(0, 1);
			}
			
			int iTemp = strTemp.Find(_T("Plat"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			iTemp = strTemp.Find(_T("Project"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			iTemp = strTemp.Find(_T("BASE"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			iTemp = strTemp.Find(_T("HW"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			
			m_strVersion += _T("SW Ver:");
			m_strVersion += strTemp;
			UpdateData(FALSE);
			//
			if(NULL == _tcsstr(strTemp, m_WriteParam.sVersion))
			{
				m_strMsg.LoadString(IDS_VERSION_ERR);
				//m_strMsg.Format(_T("版本不匹配:%s"),strTemp.operator LPCTSTR());
				m_StaticMsg.SetWindowText(m_strMsg);
				m_StaticMsg.SetBkColor(COLOR_FAIL);
				LogErr(FALSE);
				return FALSE;
			}            
		}
	}
	else
	{
		// # Zijian,Zhu @ 20111129 
		if (!_SPOK(SP_GetMsVersion(g_hDiagPhone, (void *)sVersion, sizeof(sVersion))))
		{
			m_strMsg.LoadString(IDS_VERSION_READ);
			m_StaticMsg.SetWindowText(m_strMsg);
			m_StaticMsg.SetBkColor(COLOR_FAIL);
			LogErr(FALSE);
			return FALSE;
		}
		else
		{
			CString  strTemp;
			strTemp.Format(_T("%s"), A2W(sVersion));
			//teana hu 2012.12.17
			int nIndex = strTemp.Find(_T("\n"));
			if(nIndex == 0)
			{
				strTemp.Delete(0, 1);
			}
			
			int iTemp = strTemp.Find(_T("Plat"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			iTemp = strTemp.Find(_T("Project"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			iTemp = strTemp.Find(_T("BASE"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			iTemp = strTemp.Find(_T("HW"),0);
			strTemp.Insert(iTemp,_T("\r\n"));
			
			m_strVersion += _T("SW Ver:");
			m_strVersion += strTemp;
			UpdateData(FALSE);
			//
			if(NULL == _tcsstr(strTemp, m_WriteParam.sVersion))
			{
				m_strMsg.LoadString(IDS_VERSION_ERR);
				//m_strMsg.Format(_T("版本不匹配:%s"),strTemp.operator LPCTSTR());
				m_StaticMsg.SetWindowText(m_strMsg);
				m_StaticMsg.SetBkColor(COLOR_FAIL);
				LogErr(FALSE);
				return FALSE;
			}            
		}
	}


    return  TRUE;
}


DWORD CWriteFlashDlg::VerifConnProc(LPVOID lpParam)
{
    CWriteFlashDlg *pThis = (CWriteFlashDlg *)lpParam;
    return pThis->VerifConnFunc(0, (long)pThis);
}

#define VERIFCONN_COUNTS  3
DWORD CWriteFlashDlg::VerifConnFunc(WPARAM, LPARAM)
{
	// ## Endian Self-adaption <
	SP_SetProperty(g_hDiagPhone, SP_ATTR_ENDIAN, 0, (const void *)2); // PP_BIG_ENDIAN
	unsigned char data[10] = {0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x7E};
	SP_Write(g_hDiagPhone, data, 10);
	// ## > End

	TCHAR szRecvBuf[MAX_STRING_IN_BYTES] = {0};
	TCHAR szVersionInfo[MAX_STRING_IN_BYTES] = {0};
	int i = 0;
	BOOL bOper = TRUE;
	m_bPhoneConnState = FALSE;
	do 
	{
		if (_SPOK(SP_GetMsVersion(g_hDiagPhone, szVersionInfo, sizeof(szVersionInfo))))
		{
			m_bPhoneConnState = TRUE;
			break;
		}
	} while(VERIFCONN_COUNTS > i++);
	if(!m_bPhoneConnState)
	{
        m_strMsg.LoadString(IDS_VERIFPHONESTATE_FAIL);
        DisPlayMessage(STATE_FAIL);
        LogErr(FALSE);
        PostMessage(WM_REFRESH_UI, 0, 0);
		bOper = FALSE;
		goto _END_OPERVE;
	}
	
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE); 
	m_strMsg.LoadString(IDS_CONNECT);       //connect success
	DisPlayMessage(STATE_WORKING);
	if(m_bReadFun)  //Read Fun
	{
		ReadBack();
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
		PostMessage(WM_REFRESH_UI, 0, 0);
	}
	else        //WriteteFun
	{  
		WriteFun();
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
		PostMessage(WM_REFRESH_UI, 0, 0);           
	}  
	
_END_OPERVE:
	SP_EndPhoneTest(g_hDiagPhone);
	if(m_bOperationMethod || (-1 != m_nComPort))
	{
		TimerAdminMg(TRUE);
	}
	m_bIsTesting = FALSE;
	GetDlgItem(IDC_COMBO_FORPORT)->EnableWindow(TRUE);
    return bOper;  	
}

void CWriteFlashDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	BOOL bOnTimer = FALSE;
	if(!m_bOperationMethod && (-1 == m_nComPort) && m_bCrtTimer)
	{
		TimerAdminMg(FALSE);
	}
	else
	{
		switch(nIDEvent)
		{
		case 1:
			bOnTimer = EnumCommPort();
			if((bOnTimer != m_bConnState) || m_bCompUpgIndiStatic)
			{
				m_bCompUpgIndiStatic = FALSE;
				m_bConnState = bOnTimer;
				DisplayPortCableInd(m_bConnState);
			}
			break;
		default:
			bOnTimer = EnumCommPort();
			if((bOnTimer != m_bConnState) || m_bCompUpgIndiStatic)
			{
				m_bCompUpgIndiStatic = FALSE;
				m_bConnState = bOnTimer;
				DisplayPortCableInd(m_bConnState);
			}
			break;
		}
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CWriteFlashDlg::GenerateRWMask(DWORD *pdwSd1, DWORD *pdwSd2)
{
	*pdwSd1 = NULL;
	*pdwSd2 = NULL;

	BOOL bSdDiv = GetDivisionSend();

	if(!bSdDiv)
	{
		if(m_WriteParam.bChkIMEI)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI1;
		}
		if(m_WriteParam.bChkIMEI2)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI2;
		}
		if(m_WriteParam.bChkIMEI3)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI3;
		}
		if(m_WriteParam.bChkIMEI4)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI4;
		}
		if(m_WriteParam.bChkBTAddr)
		{
			*pdwSd1 |= FNAMASK_WRITE_BTADDR;
		}
		if(m_WriteParam.bCheckWifi)
		{
			*pdwSd1 |= FNAMASK_WRITE_WIFIADDR;
		}
	}
	else
	{
		if(m_WriteParam.bChkIMEI)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI1;
		}
		if(m_WriteParam.bChkIMEI2)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI2;
		}
		if(m_WriteParam.bChkIMEI3)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI3;
		}
		if(m_WriteParam.bChkIMEI4)
		{
			*pdwSd1 |= FNAMASK_WRITE_IMEI4;
		}
		if(m_WriteParam.bChkBTAddr)
		{
			*pdwSd2 |= FNAMASK_WRITE_BTADDR;
		}
		if(m_WriteParam.bCheckWifi)
		{
			*pdwSd2 |= FNAMASK_WRITE_WIFIADDR;
		}
	}

	return bSdDiv;
}

void CWriteFlashDlg::OnBtnModeSel() 
{
	// TODO: Add your control notification handler code here
	CModeSelDlg dlg_ModeSel(&m_bOperationMethod, &m_bDisableAutoUSB, &m_bRebootDUT);
	int nResponse = dlg_ModeSel.DoModal();
	if(IDOK == nResponse)
	{
		m_bCompUpgIndiStatic = TRUE;
		/*
		CString strChgDisp(_T(""));
		if(m_bOperationMethod)
		{
			strChgDisp.Format(IDS_MODEDISP_NEWMODE);
		}
		else
		{
			strChgDisp.Format(IDS_MODEDISP_OLDMODE);
		}
		m_StaticModeDisp.SetWindowText(strChgDisp);
		*/
		m_bPortCmbAddIndi = FALSE;
		if(!m_bCrtTimer)
		{
			BOOL bConnState = EnumCommPort();
			ModeStaticIndi(m_bOperationMethod, bConnState);
			if(m_bOperationMethod || (-1 != m_nComPort))
			{
				TimerAdminMg(TRUE);
			}
		}
	}
}

void CWriteFlashDlg::OnSelchangeComboForport() 
{
    // TODO: Add your control notification handler code here
    int nIndex = m_cmbFormalPort.GetCurSel();
    CString sLBText;
    m_cmbFormalPort.GetLBText(nIndex, sLBText);
	if(0 == sLBText.CompareNoCase(_T("USB")))
	{
        m_bUsbMode = TRUE;
        m_nComPort = -1;
		if(m_bCrtTimer)
		{
			TimerAdminMg(FALSE);
		}
	}
	else
	{
		m_bUsbMode = FALSE;
        _stscanf(sLBText, _T("COM%d"), &m_nComPort);
		m_nResvComPort = m_nComPort;
		if(!m_bCrtTimer)
		{
			TimerAdminMg(TRUE);
		}
	}
}

void CWriteFlashDlg::TimerAdminMg(BOOL bCreateorKill)
{
	if(bCreateorKill)
	{
		m_bCrtTimer = TRUE;
		SetTimer(1, (UINT)STATIC_REF_RATE, NULL);
	}
	else
	{
		m_bCrtTimer = FALSE;
		KillTimer(1);
		m_StaticPortIndicator.SetBkColor(COLOR_ORANGE);
		ModeStaticIndi(m_bOperationMethod, FALSE);
		m_bConnState = FALSE;
	}
}

BOOL CWriteFlashDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	UNREFERENCED_PARAMETER(dwData);
	
	BOOL bRefresh = FALSE;
	switch(nEventType)
	{
	case DBT_DEVICEARRIVAL:
		bRefresh = TRUE;
		break;
	case DBT_DEVICEQUERYREMOVE:
		bRefresh = TRUE;
		break;
	case DBT_DEVICEREMOVEPENDING:
		bRefresh = TRUE;
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		bRefresh = TRUE;
		break;
	case DBT_CONFIGCHANGED:
		bRefresh = FALSE;
		break;
	case DBT_DEVICETYPESPECIFIC:
		bRefresh = FALSE;
		break;
	case DBT_DEVICEQUERYREMOVEFAILED:
		bRefresh = FALSE;
		break;
	default:
		bRefresh = FALSE;
		break;
	}

	if(bRefresh)
	{
		m_bPortCmbAddIndi = FALSE;
		if(!m_bCrtTimer)
		{
			m_bConnState = EnumCommPort();
			if(m_bOperationMethod || (-1 != m_nComPort))
			{
				TimerAdminMg(TRUE);
			}
		}
	}
	
	return CDialog::OnDeviceChange(nEventType, dwData);
}

BOOL CWriteFlashDlg::DoRegisterDeviceInterface()
{
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter; 
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter)); 
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE); 
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE; 
	NotificationFilter.dbcc_classguid = GUID_DEVCLASS_PORTS; 
	HDEVNOTIFY hDevNotify;
	hDevNotify = RegisterDeviceNotification(GetSafeHwnd(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	if(!hDevNotify)
	{
		return FALSE;
	}
	//UnregisterDeviceNotification(*hDevNotify);
	return TRUE;
}

void CWriteFlashDlg::ModeStaticIndi(BOOL bMode, BOOL bConn)
{
	CString strMode(_T("")), strConn(_T("")), strStatic(_T(""));
	if(bMode)
	{
		strMode.Format(IDS_MODEDISP_NEWMODE);
	}
	else
	{
		strMode.Format(IDS_MODEDISP_OLDMODE);
	}
	if(bConn)
	{
		strConn.Format(IDS_STRING_STATIC_CONN);
	}
	else
	{
		strConn.Format(IDS_STRING_STATIC_DISCONN);
	}
	
	strStatic = strMode + _T(", ") + strConn;
	m_StaticModeDisp.SetWindowText(strStatic);
}

BOOL CWriteFlashDlg::GetDivisionSend()
{
	BOOL bSdDiv = FALSE;
	if( (m_WriteParam.bChkIMEI || m_WriteParam.bChkIMEI2 || m_WriteParam.bChkIMEI3 || m_WriteParam.bChkIMEI4) && 
		(m_WriteParam.bChkBTAddr || m_WriteParam.bCheckWifi) )
	{
		bSdDiv = TRUE;
	}
	return bSdDiv;
}

void CWriteFlashDlg::AutoCreateSN(LPTSTR lpszTempSN, int nSize)
{
    if(nSize < m_nSNLength)
    {
        return ;
    }
    CTime Time = CTime::GetCurrentTime();
    CString strSN = Time.Format(_T("%Y%m%d%H%M%S"));
    if (strSN.GetLength() > m_nSNLength)
    {
        _tcscpy(lpszTempSN, strSN.Right(m_nSNLength));
    }
    else
    {
        int nLen = m_nSNLength- strSN.GetLength();
        CString strTemp;
        for(int i=0; i<nLen; i++)
        {
            strTemp.Format(_T("%d"),  i);
            strSN += strTemp;
        }
        _tcscpy(lpszTempSN, strSN);
    }
}

unsigned __int64 CWriteFlashDlg::StringHextoDec(CString strCont, int nLen)
{
	unsigned __int64 nConvRes = 0;
	unsigned __int64 nConvHigh = 0, nConvLow = 0;
	CString strTemp = strCont;

	if(16 < nLen)
	{
		nLen = 16;
	}

	if(strCont.GetLength() > nLen)
	{
		strTemp = strTemp.Left(nLen);
	}
	strTemp.MakeLower();

	swscanf(strTemp.Right(8), _T("%08x"), &nConvLow);
	swscanf(strTemp.Left(strTemp.GetLength() - 8), _T("%08x"), &nConvHigh);

	for(int i=0; i<8 ;i++)
	{
		nConvHigh *= (unsigned __int64)16;
	}
	nConvRes = nConvHigh + nConvLow;
	
	return nConvRes;
}


unsigned __int64 CWriteFlashDlg::StringHextoDec(TCHAR *szCont, int nLen)
{
	unsigned __int64 nConvRes = 0;
	unsigned __int64 nConvHigh = 0, nConvLow = 0;
	CString strTemp((LPCTSTR)szCont);
	
	if(16 < nLen)
	{
		nLen = 16;
	}
	
	if((int)_tcslen(szCont) > nLen)
	{
		strTemp = strTemp.Left(nLen);
	}
	strTemp.MakeLower();
	
	swscanf(strTemp.Right(8), _T("%08x"), &nConvLow);
	swscanf(strTemp.Left(strTemp.GetLength() - 8), _T("%08x"), &nConvHigh);
	
	for(int i=0; i<8 ;i++)
	{
		nConvHigh *= (unsigned __int64)16;
	}
	nConvRes = nConvHigh + nConvLow;
	
	return nConvRes;
}

void CWriteFlashDlg::OnStartAction()
{
	DWORD dwEnterModeID = 0;
    UpdateData(FALSE); //开始后更新显示
	
	//  ## zijian.zhu @ 20111212 for NEWMS00147831 <<<
	if( (!m_bReadFun) && m_WriteParam.bChkIMEI && 
		( (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI1) || 
		  (m_WriteParam.bChkIMEI2 && (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI2)) || 
		  (m_WriteParam.bChkIMEI3 && (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI3)) ||
		  (m_WriteParam.bChkIMEI4 && (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI4)) ) )
	{
		AutoGenerateImeiCodeAction(TRUE);
	}
	//  ## zijian.zhu @ 20111212 for NEWMS00147831 >>>
	
    if(!CheckIMEI())   //检查输入正确否
    {
        DisPlayMessage(STATE_FAIL);
        LogErr(FALSE);
        ReflashUI();  
        return;
    }
    EnableWindow(FALSE);//Disable 
	
	CHANNEL_ATTRIBUTE   ca;
	if (m_bUsbMode && (!m_bOperationMethod))
	{
		ca.ChannelType      = CHANNEL_TYPE_USBMON;
	}
	else
	{
		ca.ChannelType      = CHANNEL_TYPE_COM;
		ca.Com.dwPortNum    = m_nComPort;
		ca.Com.dwBaudRate   = 115200;
	}

	if(!m_bRebootDUT)
	{
		SP_SetProperty(g_hDiagPhone, SP_ATTR_ENDIAN, 0, (const void *)2);
		SP_SetProperty(g_hDiagPhone, SP_ATTR_USB_ENUMMODE, (long)MODE_WCDMA_GSM, (const void *)USB_ENUM_ONCE);	
	}
	
	if (!_SPOK(SP_BeginPhoneTest(g_hDiagPhone, &ca)))
	{   
		EnableWindow(TRUE);
		m_strMsg.Format(IDS_PORT_FAIL,m_nComPort);
		DisPlayMessage(STATE_FAIL);
		LogErr(FALSE);
		ReflashUI();  
		return;
	}
	/*
	if(m_bCrtTimer)
	{
	TimerAdminMg(FALSE);
	}
	*/

	m_bIsTesting = TRUE;
	GetDlgItem(IDC_COMBO_FORPORT)->EnableWindow(FALSE);

	if(m_bOperationMethod)
	{
		m_bPhoneConnState = FALSE;
		m_hVerfConn2Test = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)VerifConnProc, this, NULL, &m_dwVerfConn2Test);
		m_strMsg.Format(IDS_WAIT_NEWMET);	
		DisPlayMessage(STATE_WORKING); 
	}
	else
	{
		ResetEvent(m_hExitThread);
		ResetEvent(m_hEnterModeSuccess);
		m_hEnterModeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EnterModeProc, 
			this, NULL, &dwEnterModeID);
		if(0==m_nTestMode)
		{
			SP_EnterModeProcess(g_hDiagPhone, m_bUsbMode?true:false, m_nDUTPort, RM_CALIBR_NV_ACCESS_MODE, m_hEnterModeSuccess, 0);
		}
		else
		{
			SP_EnterModeProcess(g_hDiagPhone, m_bUsbMode?true:false, m_nDUTPort, RM_CALIBRATION_MODE, m_hEnterModeSuccess, 0);
		}
		m_strMsg.Format(IDS_WAIT, m_nTimeout/1000);	
		DisPlayMessage(STATE_WORKING); 
		
		if(m_bUseDcs && m_pDcSource != NULL)
		{
			m_pDcSource->SetVoltageOutput((float)4.0);    
		}
	}
}

CWriteFlashDlg::OnCleanCodeEdit()
{
	if(!m_WriteParam.bChkIMEI || (GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI1))
	{
		m_strEditIMEI1 =_T("");
	}
	if(!m_WriteParam.bChkIMEI2 || (GENCODE_TYP_AUTGEN != m_WriteParam.eGenIIMEI2))
	{
		m_strEditIMEI2 =_T("");
	}
	if(!m_WriteParam.bChkIMEI3 || (GENCODE_TYP_AUTGEN != m_WriteParam.eGenIIMEI3))
	{
		m_strEditIMEI3 =_T("");
	}
	if(!m_WriteParam.bChkIMEI4 || (GENCODE_TYP_AUTGEN != m_WriteParam.eGenIIMEI4))
	{
		m_strEditIMEI4 =_T("");
	}
	if(!m_WriteParam.bChkBTAddr||!m_WriteParam.bChkAutoCreateBT)
	{
		m_strEditBT = _T("");
	}
	if(!m_WriteParam.bCheckWifi||!m_WriteParam.bChkAutoCreateWifi)
	{
		m_strWifi = _T("");
	}
	
	//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<
	if(m_WriteParam.bEnableSNFunc)
	{
		if(!m_WriteParam.bChkSN1||!m_WriteParam.bChkAutoSN1)
		{
			m_strSN1 = _T("");
		}
		if(!m_WriteParam.bChkSN2||!m_WriteParam.bChkAutoSN2)
		{
			m_strSN2 = _T("");
		}
	}
	//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>
	UpdateData(FALSE);
}

void CWriteFlashDlg::AutoGenerateImeiCodeAction(BOOL bMainInput)
{
	CString strInitCode(_T("")); 
	int nGenLen = 0;

	if(!bMainInput)
	{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // bMainInput: FALSE - 未输入需要手动输入的IMEI号码
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		TCHAR szTemp[50] = {0}, szAssi[50] = {0};
		_tcscpy(szTemp, m_WriteParam.sAutoIMEI);
		m_int64DefaultIMEI = _ttoi64(szTemp);
		
		// ## Main IMEI(IMEI1)
		if(m_WriteParam.bChkIMEI && (GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI1))
		{
			AutoCreateIMEI(szTemp);
			m_strEditIMEI1.Format(_T("%s"), szTemp);
			nGenLen = m_strEditIMEI1.GetLength() - 1;
			strInitCode = m_strEditIMEI1.Left(nGenLen);
			m_int64DefaultIMEI++;
		}
		
		// ## Second IMEI(IMEI2)
		if(m_WriteParam.bChkIMEI2)
		{
			if(!m_WriteParam.bChkUseSameIMEI)
			{
				if(GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI2)
				{
					if(GENCODE_TYP_AUTGEN == m_WriteParam.eGenIIMEI2)
					{
						AutoCreateIMEI(szTemp);
						m_strEditIMEI2.Format(_T("%s"), szTemp);
						m_int64DefaultIMEI++;
					}
					if(GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI1 && GENCODE_TYP_AUTADD == m_WriteParam.eGenIIMEI2)
					{
						GetImeiCodePlus(&m_strEditIMEI2, strInitCode);
						m_int64DefaultIMEI++;
					}
					strInitCode = m_strEditIMEI2.Left(nGenLen);
				}
			}
			else 
			{
				m_strEditIMEI2=m_strEditIMEI1;
			}
		}
		
		// ## Third IMEI(IMEI3)
		if(m_WriteParam.bChkIMEI3)
		{
			if(!m_WriteParam.bChkUseSameIMEI)
			{
				if(GENCODE_TYP_MANUAL!=m_WriteParam.eGenIIMEI3)
				{
					if(GENCODE_TYP_AUTGEN == m_WriteParam.eGenIIMEI3)
					{
						AutoCreateIMEI(szTemp);
						m_strEditIMEI3.Format(_T("%s"),szTemp);
						m_int64DefaultIMEI++;
					}
					if(GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI1 && GENCODE_TYP_AUTADD == m_WriteParam.eGenIIMEI3)
					{
						GetImeiCodePlus(&m_strEditIMEI3, strInitCode);
						m_int64DefaultIMEI++;
					}
					strInitCode = m_strEditIMEI3.Left(nGenLen);
					
				}
			}
			else
			{
				m_strEditIMEI3=m_strEditIMEI1;
			}
		}
		
		// ## Fourth IMEI(IMEI4)
		if(m_WriteParam.bChkIMEI4)
		{
			if(!m_WriteParam.bChkUseSameIMEI)
			{
				if(GENCODE_TYP_MANUAL!=m_WriteParam.eGenIIMEI4)
				{
					if(GENCODE_TYP_AUTGEN == m_WriteParam.eGenIIMEI4)
					{
						AutoCreateIMEI(szTemp);
						m_strEditIMEI4.Format(_T("%s"),szTemp);
						m_int64DefaultIMEI++;
					}
					if(GENCODE_TYP_MANUAL!=m_WriteParam.eGenIIMEI1 && GENCODE_TYP_AUTADD==m_WriteParam.eGenIIMEI4)
					{
						GetImeiCodePlus(&m_strEditIMEI4, strInitCode);
						m_int64DefaultIMEI++;
					}
					strInitCode = m_strEditIMEI4.Left(nGenLen);
				}
			}
			else
			{
				m_strEditIMEI4=m_strEditIMEI1;
			}
		}
	}
	else
	{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// bMainInput: TRUE - 已输入需要手动输入的IMEI号码
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		nGenLen = m_strEditIMEI1.GetLength() - 1;
		strInitCode = m_strEditIMEI1.Left(nGenLen);

		if(m_WriteParam.bChkIMEI2)
		{
			if(!m_WriteParam.bChkUseSameIMEI)
			{
				if(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI1 && GENCODE_TYP_AUTADD == m_WriteParam.eGenIIMEI2)
				{
					GetImeiCodePlus(&m_strEditIMEI2, strInitCode);
					strInitCode = m_strEditIMEI2.Left(nGenLen);
				}
				if(GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI2 && GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI1)
				{
					strInitCode = m_strEditIMEI2.Left(nGenLen);
				}
			}
			else
			{
				m_strEditIMEI2=m_strEditIMEI1;
			}
		} 
		
		if(m_WriteParam.bChkIMEI3)
		{
			if(!m_WriteParam.bChkUseSameIMEI)
			{
				if(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI1 && GENCODE_TYP_AUTADD == m_WriteParam.eGenIIMEI3)
				{
					GetImeiCodePlus(&m_strEditIMEI3, strInitCode);
					strInitCode = m_strEditIMEI3.Left(nGenLen);
				}
				if(GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI3 && GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI1)
				{
					strInitCode = m_strEditIMEI3.Left(nGenLen);
				}
			}
			else
			{
				m_strEditIMEI3=m_strEditIMEI1;
			}
		} 
		
		if(m_WriteParam.bChkIMEI4)
		{
			if(!m_WriteParam.bChkUseSameIMEI)
			{
				if(GENCODE_TYP_MANUAL == m_WriteParam.eGenIIMEI1 && GENCODE_TYP_AUTADD == m_WriteParam.eGenIIMEI4)
				{
					GetImeiCodePlus(&m_strEditIMEI4, strInitCode);
					strInitCode = m_strEditIMEI4.Left(nGenLen);
				}
				if(GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI4 && GENCODE_TYP_MANUAL != m_WriteParam.eGenIIMEI1)
				{
					strInitCode = m_strEditIMEI4.Left(nGenLen);
				}
			}
			else
			{
				m_strEditIMEI4=m_strEditIMEI1;
			}
		}
	}

	this->UpdateData(FALSE);
}

DWORD CWriteFlashDlg::CheckApVer()
{
    //USES_CONVERSION;
	int nOperCode;
	CString strTemp;
	char szPreAt[128] = {0};; 
	char szRecvBuf[4096] = {0};
	unsigned long recvLen = 4000,realRecv;
	strcpy(szPreAt, "AT+SPDIAG=\"0000000008000000\"");

	int iTemp = SP_SendATCommand(g_hDiagPhone,szPreAt,1,szRecvBuf,recvLen,&realRecv,3000);

	TCHAR szTmp[4096] = {0};
	MultiByteToWideChar(CP_ACP,0,szRecvBuf,-1,szTmp,4096);
	strTemp.Format(_T("%s"),szTmp);
	if(strTemp == _T(""))
	{
		return READ_FAIL;
	}
	else
	{
		BOOL bConvert = FALSE;

		iTemp = strTemp.Find(_T("\""),0);
		strTemp.Delete(0,iTemp+1);
		iTemp = strTemp.Find(_T("\""),0);
 		strTemp.Delete(iTemp,strTemp.GetLength()-iTemp);

		if(strTemp.Find(_T("00000000")) != -1)
		{
			strTemp.Delete(0, strlen(szPreAt)); //16 is length 0f "00000000c0000000";
			bConvert = TRUE;
		}

		char szResult[4096] = {0};
		char szTemp[4096] = {0};
		if(bConvert)
		{
			WideCharToMultiByte(CP_ACP,0,strTemp.GetBuffer(strTemp.GetLength()),-1,szTemp,strTemp.GetLength(),NULL,NULL);
			ASCII2Bin( (BYTE*)szTemp,(BYTE*)szResult,strlen(szTemp) );
			strTemp.ReleaseBuffer();
			
			MultiByteToWideChar(CP_ACP,0,szResult,-1,szTmp,4096);
			strTemp.Format(_T("%s"),szTmp);
		}

		//手动换行
		iTemp = strTemp.Find(_T("Plat"),0);
		strTemp.Insert(iTemp,_T("\r\n"));
		iTemp = strTemp.Find(_T("Project"),0);
		strTemp.Insert(iTemp,_T("\r\n"));
		iTemp = strTemp.Find(_T("BASE"),0);
		strTemp.Insert(iTemp,_T("\r\n"));

		m_WriteParam.strAPVerRead = strTemp;
		m_strVersion = _T("AP Ver:\r\n");
		m_strVersion += strTemp;
		m_strVersion += _T("\r\n");
		UpdateData(FALSE);

		ZeroMemory(szTemp, 4096);
		m_WriteParam.strAPVerInput.MakeLower();
		WideCharToMultiByte(CP_ACP,0,m_WriteParam.strAPVerInput.GetBuffer(m_WriteParam.strAPVerInput.GetLength()),-1,szTemp,m_WriteParam.strAPVerInput.GetLength(),NULL,NULL);
		m_WriteParam.strAPVerInput.ReleaseBuffer();
		
		ZeroMemory(szResult, 4096);
		strTemp.MakeLower();
		WideCharToMultiByte(CP_ACP,0,strTemp.GetBuffer(strTemp.GetLength()),-1,szResult,strTemp.GetLength(),NULL,NULL);
		strTemp.ReleaseBuffer();
		
		if(strstr(szResult,szTemp) != NULL)
		//if(m_WriteParam.strAPVerInput == m_WriteParam.strAPVerRead)
		{
			return EQUAL;
		}
		else
		{
			return NOT_EQUAL;
		}
	}
}

BOOL CWriteFlashDlg::ASCII2Bin(
						 const BYTE* pBuffer,//source buffer
						 BYTE* pResultBuffer,//dest buffer
						 unsigned long ulSize //source buffer length
						 )
{
    ASSERT( pBuffer );
    ASSERT( pResultBuffer );
    
    int iCount = 0;
    BYTE l,h;
    while( (unsigned long)iCount < ulSize )
    {
        if( *pBuffer >= '0' && *pBuffer <= '9' )
        {
            h = (BYTE)(*pBuffer - '0');
        }
        else
        {
            h = (BYTE)(*pBuffer - 'A' + 10);
        }
        h = (BYTE)(h << 4);
        pBuffer++;
        if( *pBuffer >= '0' && *pBuffer <= '9' )
        {
            l = (BYTE)(*pBuffer - '0');
        }
        else
        {
            l = (BYTE)(*pBuffer - 'A' + 10);
        }
        *pResultBuffer = (BYTE)( h  | l );
        pResultBuffer++;
        pBuffer++;
        iCount += 2;
    }     
    return TRUE;
}

BOOL CWriteFlashDlg::OpenCDROM()
{
	if(m_bRebootDUT)
	{
		return TRUE;
	}
	int nOperCode;
	CString strTemp;
	char szPreAt[128] = {0};
	char szRecvBuf[4096] = {0};
	unsigned long recvLen = 4000,realRecv;
	strcpy(szPreAt, "AT+SPSETFNUM=1,5");
	
	int nErrorCode = SP_SendATCommand(g_hDiagPhone,szPreAt,1,szRecvBuf,recvLen,&realRecv,3000);
	
	if(nErrorCode == RFT_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CWriteFlashDlg::CheckATCmd()
{
	if(!m_bSendATCmd)
	{
		return TRUE;
	}
	int nOperCode;
	CString strTemp;
	char szPreAt[128] = {0};
	char szRecvBuf[4096] = {0};
	unsigned long recvLen = 4000;
	unsigned long realRecv = 0;
	
	CString strCmd = _T("");
	int nSize = m_strArrWATCmd.GetSize();
	for(int i = 0; i < nSize; i++)
	{
		realRecv = 0;
		ZeroMemory(&szRecvBuf, 4096);
		strCmd = m_strArrWATCmd.GetAt(i);
		WideCharToMultiByte(CP_ACP,0,strCmd,-1,szPreAt,128,NULL,NULL);
		
		int nErrorCode = SP_SendATCommand(g_hDiagPhone,szPreAt,1,szRecvBuf,recvLen,&realRecv,3000);
		
		if(nErrorCode != RFT_SUCCESS)
		{
			m_strMsg.Format(IDS_SEND_AT_CMD_ERR, szPreAt);
			LogErr(FALSE);
			return FALSE;
		}
		Sleep(1000);
	}

	return TRUE;
}

BOOL CWriteFlashDlg::ReadCUReference()
{
	int nErrCode = RFT_SUCCESS;
	nErrCode = SP_ReadCURef(g_hDiagPhone, m_bBuf, CU_REFERENCE_LEN);

	if(RFT_SUCCESS != nErrCode)
	{
		m_strMsg.LoadString(IDS_READ_CUREF_FAIL);
		LogErr(FALSE);
		return FALSE;
	}

	BufferToString();
	return TRUE;
}

BOOL CWriteFlashDlg::WriteCUReference()
{
	int nErrCode = RFT_SUCCESS;
	
	DWORD dwRecvLen = 0;
	StringToBuffer();

	nErrCode = SP_WriteCURef(g_hDiagPhone, m_bBuf, CU_REFERENCE_LEN);
	
	if(RFT_SUCCESS != nErrCode)
	{
		m_strMsg.LoadString(IDS_WRITE_CUREF_FAIL);
		DisPlayMessage(STATE_FAIL);
		LogErr(FALSE);
		return FALSE;
	}
	
	return TRUE;
}

void CWriteFlashDlg::BufferToString()
{
	TCHAR szValue[2] = {0};
	char szTmp[2] = {0};
	for(int i = 0; i < CU_REFERENCE_LEN; i++)
	{
		memcpy(szTmp, m_bBuf+i, 1);
		MultiByteToWideChar(CP_ACP,0,szTmp,-1,szValue,2);
		GetDlgItem(IDC_EDIT_CUREF1 + i)->SetWindowText(szValue);
	}
}

void CWriteFlashDlg::StringToBuffer()
{
	CString strValue;
	for(int i = 0; i < CU_REFERENCE_LEN; i++)
	{
		GetDlgItem(IDC_EDIT_CUREF1 + i)->GetWindowText(strValue);
		WideCharToMultiByte(CP_ACP,0,strValue.GetBuffer(strValue.GetLength()),-1,(char *)(m_bBuf + i),1,NULL,NULL);
	}

	m_bBuf[CU_REFERENCE_LEN - 1] = '\0';
}

void CWriteFlashDlg::OnChangeEditCuref1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF1)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF2)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF2))->SetSel(0,-1);
	}

	
}

void CWriteFlashDlg::OnChangeEditCuref2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF2)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF3)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF3))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref3() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF3)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF4)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF4))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF4)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF5)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF5))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref5() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF5)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF6)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF6))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref6() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF6)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF7)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF7))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref7() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF7)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF8)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF8))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref8() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF8)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF9)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF9))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref9() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF9)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF10)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF10))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref10() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF10)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF11)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF11))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref11() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF11)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF12)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF12))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref12() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF12)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF13)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF13))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref13() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF13)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF14)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF14))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref14() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF14)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF15)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF15))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref15() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF15)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF16)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF16))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref16() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF16)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF17)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF17))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref17() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF17)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF18)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF18))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref18() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF18)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF19)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF19))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref19() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF19)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF20)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF20))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref20() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF20)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF21)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF21))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref21() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF21)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF22)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF22))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref22() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF22)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF23)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF23))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref23() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTmp ;
    GetDlgItem(IDC_EDIT_CUREF23)->GetWindowText(strTmp);
	if(strTmp.GetLength() == 1)
	{
		GetDlgItem(IDC_EDIT_CUREF24)->SetFocus();
		((CEdit *)GetDlgItem(IDC_EDIT_CUREF24))->SetSel(0,-1);
	}
}

void CWriteFlashDlg::OnChangeEditCuref24() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CWriteFlashDlg::OnRecvDeviceChange(WPARAM wParam,LPARAM lParam)
{
	DWORD dwFlag = (DWORD)lParam;
	DWORD dwPort = (DWORD)wParam;
	UINT nPort = dwPort;
	
	if(dwFlag == 0) //device out
	{
		_ASSERTE( nPort > 0 && nPort < 256 ); 
		m_bConnState = EnumCommPort();
		m_bPortCmbAddIndi = FALSE;
	}
	else if(dwFlag == 1) // device in
	{
		_ASSERTE( nPort > 0 && nPort < 256 );
		m_bPortCmbAddIndi = FALSE;
		m_bConnState = EnumCommPort();
	}
}
