// WriteFlashDlg.h : header file
//

#if !defined(AFX_WRITEFLASHDLG_H__0F934BD1_B6AC_450D_9440_FCAAB2AB3B3A__INCLUDED_)
#define AFX_WRITEFLASHDLG_H__0F934BD1_B6AC_450D_9440_FCAAB2AB3B3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Dbt.h"
#include "ColorStatic.h"
#include "PhaseCheckExport.h"
#include "DevHound.h"


/////////////////////////////////////////////////////////////////////////////
// CWriteFlashDlg dialog
class IDCS;
class IDCSFactory;
//lint ++flb
typedef enum _MOCOR_VER_E
{
	MOCOR_VER_PRE09A37=0,
	MOCOR_VER_AFTER09A37
} MOCOR_VER_E;   
//lint --flb
typedef enum _TESTSTAT_E
{
    STATE_IDLE,
    STATE_WORKING,
    STATE_PASS,
    STATE_FAIL
}TESTSTAT_E;
//teana hu 2012.10.16
typedef enum
{
	READ_FAIL,
	NOT_EQUAL,
	EQUAL
}AP_VER_RET_E;

#define CU_REFERENCE_LEN 24
#define CU_REF_TYPE 0x3C
#define CU_REF_SUBTYPE_READ 0x00
#define CU_REF_SUBTYPE_WRITE 0x10
#define WM_RECV_DEVICE_CHANGE (WM_USER + 2013)
//
class CWriteFlashDlg : public CDialog
{
// Construction
public:
	CWriteFlashDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWriteFlashDlg)
	enum { IDD = IDD_WRITEFLASH_DIALOG };
	CEdit	m_CEditSN2;
	CEdit	m_CEditSN1;
	CStatic	m_StaticModeDisp;
	CComboBox	m_cmbFormalPort;
	CColorStatic	m_StaticPortIndicator;
	CEdit	m_CEditWifi;
	CEdit	m_CEditFourIMEI;
	CColorStatic	m_StaticYield;
	CColorStatic	m_StaticFail;
	CColorStatic	m_StaticPass;
	CEdit	m_CEditSecIMEI;
	CEdit	m_CEditMainIMEI;
    CEdit	m_CEditThirdIMEI;
	CEdit	m_CEditBT;
	CColorStatic	m_StaticMsg;
	CString	m_strEditIMEI1;
	CString	m_strEditIMEI2;
    CString	m_strEditIMEI3;
	CString	m_strMsg;
	CString	m_strEditBT;
	CString	m_strPass;
	CString	m_strFail;
	CString	m_strYield;
	CString	m_strEditIMEI4;
	CString	m_strWifi;
	CString	m_strSN1;
	CString	m_strSN2;
	CString m_strVersion;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWriteFlashDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWriteFlashDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSet();
	afx_msg void OnButtonStart();
	afx_msg void OnClose();
	afx_msg void OnButtonStop();
    afx_msg LRESULT OnRefreshUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangeEditImeimain();
	afx_msg void OnChangeEditImeisenior();
    afx_msg void OnButtonRead();
	afx_msg void OnChangeEditBt();
	virtual void OnOK();
	afx_msg void OnChangeEditImeithird();
    afx_msg void OnChangeEditImeiFour();
	afx_msg void OnChangeEditWifi();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnModeSel();
	afx_msg void OnSelchangeComboForport();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);	
	afx_msg void OnChangeEditSnmain();
	afx_msg void OnChangeEditSnsenior();
	afx_msg void OnChangeEditCuref1();
	afx_msg void OnChangeEditCuref2();
	afx_msg void OnChangeEditCuref3();
	afx_msg void OnChangeEditCuref4();
	afx_msg void OnChangeEditCuref5();
	afx_msg void OnChangeEditCuref6();
	afx_msg void OnChangeEditCuref7();
	afx_msg void OnChangeEditCuref8();
	afx_msg void OnChangeEditCuref9();
	afx_msg void OnChangeEditCuref10();
	afx_msg void OnChangeEditCuref11();
	afx_msg void OnChangeEditCuref12();
	afx_msg void OnChangeEditCuref13();
	afx_msg void OnChangeEditCuref14();
	afx_msg void OnChangeEditCuref15();
	afx_msg void OnChangeEditCuref16();
	afx_msg void OnChangeEditCuref17();
	afx_msg void OnChangeEditCuref18();
	afx_msg void OnChangeEditCuref19();
	afx_msg void OnChangeEditCuref20();
	afx_msg void OnChangeEditCuref21();
	afx_msg void OnChangeEditCuref22();
	afx_msg void OnChangeEditCuref23();
	afx_msg void OnChangeEditCuref24();
	//}}AFX_MSG
	afx_msg void OnRecvDeviceChange(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
    BOOL EnumCommPort();
    void LoadSetting();	
	void LoadCustomAddrConfig();
	void SaveCustomAddrConfig();
    void AutoCreateIMEI(LPTSTR lpszIMEI);          //计算IMEI
    __int64 ComputeCD(__int64 nImei);              //计算IMEI最后一位
    void AutoCreateBTAddr(LPTSTR lpszTempBT);      //生成蓝牙地址  自由数+时间
    void AutoCreateWifiAddr(LPTSTR lpszTempWIFI);  //生成Wifi地址  0~9,A~F+时间
    void ReflashUI();            //界面更新
    void SaveSetting(); 
    BOOL CheckIMEI(void);        //检查IMEI的合法性
    void ReadBack(void);         //读IMEI等
    void WriteFun(void);         //写IMEI等
    int WriteMoc1(void);
    int WriteMoc2(void);
    void DisPlayMessage(TESTSTAT_E eState);
    void EnableWindow(BOOL bEnabled);
    void PhaseError(SPPH_ERROR ePhaseErr);  //错误信息显示
    void LogErr(BOOL bPass);
    BOOL SearchImei(int nIndex,LPCTSTR strImei);
    BOOL SaveImei();
    void ConnectDatabase() ; //打开数据库
    void OpenFile(); //打开文本文件
    void CloseFile();
    BOOL CheckVersion();
	void DisplayPortCableInd(BOOL bConn);

	//teana hu 2012.10.16
	DWORD CheckApVer();
	BOOL ASCII2Bin(
        const BYTE* pBuffer,//source buffer
        BYTE* pResultBuffer,//dest buffer
        unsigned long ulSize //source buffer length
        );
	//

	//teana hu 2012.12.14
	BOOL OpenCDROM();
	//

	//teana hu 2012.12.17
	BOOL ReadCUReference();
	BOOL WriteCUReference();
	void BufferToString();
	void StringToBuffer();
	//

	//teana hu 2012.12.19
	BOOL CheckATCmd();
	//

public:
	BOOL ConclNullValonCheck();
	void UpgradeCursorPosition();
	void AutoGenerateImeiCodeAction(BOOL bMainInput);
	OnCleanCodeEdit();
	void OnStartAction();
	void GetImeiCodePlus(CString *strRtn, CString strOrg);
	unsigned __int64 StringHextoDec(CString strCont, int nLen=12);
	unsigned __int64 StringHextoDec(TCHAR *szCont, int nLen=12);
	void AutoCreateSN(LPTSTR lpszTempSN, int nSize);
	BOOL GetDivisionSend();
	void ModeStaticIndi(BOOL bMode, BOOL bConn);
	BOOL DoRegisterDeviceInterface();
	BOOL m_bCrtTimer;
	void TimerAdminMg(BOOL bCreateorKill);
	BOOL GenerateRWMask(DWORD *pdwSd1, DWORD *pdwSd2);
	BOOL m_bOperationMethod; //TRUE: new method, FALSE; old method
	BOOL m_bConnState;
	BOOL m_bPhoneConnState;
	BOOL m_bDisableAutoUSB;
	BOOL m_bPortCmbAddIndi;
	HANDLE m_hVerfConn2Test;
	DWORD m_dwVerfConn2Test;
	BOOL m_bPreReadImeiCount;
	int  m_nDisplayLabelFontSize1;
	int  m_nDisplayLabelFontSize2;

    WriteFlashPara m_WriteParam;
	int   m_nSNLength;
    int   m_nComPort;
	int   m_nResvComPort;
    BOOL  m_bUsbMode;
    int   m_nTimeout;
    int   m_nWriteTime;
    BOOL  m_bWriteTime;
    int   m_nTestMode; 
    DWORD m_dwEnterModeWait;
    BOOL  m_bAdminMode;
    int   m_nStationIndex;
	unsigned __int64 m_int64WifiAddrForMinimize;
	unsigned __int64 m_int64WifiAddrForMaximize;
	unsigned __int64 m_int64BtAddrForMinimize;
	unsigned __int64 m_int64BtAddrForMaximize;
	BOOL  m_bCompUpgIndiStatic;
//  BOOL  m_bCheck;
//  BOOL  m_bSaveIMEI;
//  BOOL  m_bNewVer;
//  PHASE_CHECK_S  m_phaseInfo;
    TCHAR m_szIniPath[_MAX_PATH];
    TCHAR m_szIMEITxtPath[_MAX_PATH];
    TCHAR m_szIMEIMdbPath[_MAX_PATH];
    TCHAR m_szStationName[100];
    char  m_szCurrentStation[200];
    char  m_szCheckStation[200];
    __int64 m_int64DefaultIMEI;
    SPPH_OBJECT m_hPhaseCheckObj;

    BOOL  m_bUseDcs;
    IDCS            * m_pDcSource;
    IDCSFactory     * m_pDcsFactory;

	//teana hu 2012.12.06
	BOOL m_bRebootDUT;
	int m_nDUTPort;
	//

	//teana hu 2012.12.17
	BYTE m_bBuf[CU_REFERENCE_LEN];
	CStringArray m_strArrWATCmd;
	BOOL m_bSendATCmd;
	//

	//teana hu 2012.01.14
	CDevHound m_devHound;
	//


protected:
	BOOL m_bCustAddrInvalid;
	BOOL m_bIsTesting;
    HANDLE m_hEnterModeSuccess;
	HANDLE m_hExitThread;
	HANDLE m_hEnterModeThread;
    DWORD EnterModeFunc(WPARAM, LPARAM);
    DWORD VerifConnFunc(WPARAM, LPARAM);
    static DWORD EnterModeProc(LPVOID lpParam);
	static DWORD VerifConnProc(LPVOID lpParam);
    BOOL  m_bReadFun;
    CFont *m_pFont,*m_pFont1,*m_pFontPortInd;
    int m_nTestCounts;
    int m_nActualCounts;
    DWORD m_dwStartTime;
    BOOL m_bEnter;
    int m_nPassCounts;   //for Yiled所用
    int m_nFailCounts; //for Yiled所用
    _ConnectionPtr	m_pConnection;
	_RecordsetPtr	m_pRecordset;

	//teana hu 2012.12.17
	BOOL m_bOperateCURef;
	//
   
  

//  [5/17/2012 Xiaoping.Jing] 
    BOOL    m_bCheckBTInput;
    BOOL    m_bCheckWFInput;
    TCHAR   m_szFmtBT[MAX_BT_ADDR_STR_LENGTH  ];
    TCHAR   m_szFmtWF[MAX_WIFI_ADDR_STR_LENGTH];

//  [7/11/2012 Zijian.Zhu]
	int     m_nScannerKey_OnWrite;
	int     m_nScannerKey_OnRead;

	CustomBtAddrComp m_CustomBtAddrComp;
	CustomWifiAddrComp m_CustomWifiAddrComp;
};

/////////////////////////////////////////////////////////////////////////////
BOOL   WINAPI GetPrivateProfileBool    (LPCTSTR lpAppName, LPCTSTR lpKeyName, BOOL bDefault,  LPCTSTR lpFileName);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRITEFLASHDLG_H__0F934BD1_B6AC_450D_9440_FCAAB2AB3B3A__INCLUDED_)
