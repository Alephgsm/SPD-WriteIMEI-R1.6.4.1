// WriteFlash.h : main header file for the WRITEFLASH application
//

#if !defined(AFX_WRITEFLASH_H__45CE1FC4_81FA_4B6E_BF84_30CE9C2BC6B9__INCLUDED_)
#define AFX_WRITEFLASH_H__45CE1FC4_81FA_4B6E_BF84_30CE9C2BC6B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "phdef.h"
#include "XAboutDlg.h"
#include "IDCS.h"
#include "PhoneCommand.h"
/////////////////////////////////////////////////////////////////////////////
// CWriteFlashApp:
// See WriteFlash.cpp for the implementation of this class
//
#define BT_ADDR_STR_LENGTH			13    //also wifi lenth

typedef enum
{
    GENCODE_TYP_MANUAL  = 0,  // manual input or use the scanner
	GENCODE_TYP_AUTGEN,       // auto generation (system time)
	GENCODE_TYP_AUTADD        // auto generation (addr plus) 
        
}GCODIMEI_E;

typedef struct _Write_Flash_Para
{
    BOOL	bChkIMEI;
	BOOL	bChkIMEI2;
    BOOL	bChkIMEI3;
    BOOL    bChkIMEI4;
//  ## zijian.zhu @ 20111212 for NEWMS00147831 <<<	
	GCODIMEI_E eGenIIMEI1;
	GCODIMEI_E eGenIIMEI2;
	GCODIMEI_E eGenIIMEI3;
	GCODIMEI_E eGenIIMEI4;
//  ## zijian.zhu @ 20111212 for NEWMS00147831 >>>
   	BOOL	bChkAutoIMEI;
	BOOL	bChkUseSameIMEI;
   	BOOL	bChkPhase;
    BOOL	bChkBTAddr;
	BOOL	bChkAutoCreateBT;
    BOOL    bCheckWifi;
    BOOL    bChkAutoCreateWifi;
    int     nMocver;
    BOOL    bSaveIMEI;
    BOOL    bSaveToTxt;
    BOOL    bCheckRepeat;  //重复性
    BOOL    bCheckValid;   //合法性
    BOOL    bCheckVer;
	int     nVerLenLowLimit;
    CString sAutoIMEI;
    CString sVersion;
    CString sForeWifi;
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) <<<	
	BOOL    bGenBtAddrType;  // FALSE: Time; TRUE: Initial Addr;
	BOOL    bGenWifiMacType;  // FALSE: Time; TRUE: Initial Addr;
	CString strInitAddrForBT;
	CString strInitAddrForWifi;
	BOOL    bEnableSNFunc;
	BOOL    bChkSN1;
	BOOL    bChkSN2;
	BOOL    bChkAutoSN1;
	BOOL    bChkAutoSN2;
//  ## zijian.zhu @ 20111103 for NEWMS00133531(Yihu requirement) >>>
	
//  ## zijian.zhu @ 20120414 for NEWMS00185078 <<<
    BOOL    bChkIMEIAfterRead;
//  ## zijian.zhu @ 20120414 for NEWMS00185078 >>>

//  ## zijian.zhu @ 20120725 for NEWMS00226697 <<<
    BOOL    bManualAssignBtAddr;
	CString strBtAddrPath;
    BOOL    bManualAssignWifiAddr;
	CString strWifiAddrPath;
//  ## zijian.zhu @ 20120725 for NEWMS00226697 >>>

	//teana hu 2012.10.16
	CString strAPVerRead;
	CString strAPVerInput;
	BOOL bChkAPVer;
	//

    _Write_Flash_Para()
    {
        memset(this,0,(DWORD)(&(this->sAutoIMEI)) - (DWORD)(&(this->bChkIMEI)));
        sAutoIMEI=_T("");
    }

  
}WriteFlashPara;

typedef struct _Custom_BtAddr_Comp
{
	TCHAR szStarAddr[MAX_BT_ADDR_STR_LENGTH];
	TCHAR szCurrAddr[MAX_BT_ADDR_STR_LENGTH];
	TCHAR szStopAddr[MAX_BT_ADDR_STR_LENGTH];
	
}CustomBtAddrComp;

typedef struct _Custom_WifiAddr_Comp
{
	TCHAR szStarAddr[MAX_WIFI_ADDR_STR_LENGTH];
	TCHAR szCurrAddr[MAX_WIFI_ADDR_STR_LENGTH];
	TCHAR szStopAddr[MAX_WIFI_ADDR_STR_LENGTH];
	
}CustomWifiAddrComp;

class CWriteFlashApp : public CWinApp
{
public:
	CWriteFlashApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWriteFlashApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWriteFlashApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    CString m_strVersion;
    CString m_strBuild;
private:
    HINSTANCE      m_hResModule;

	void GetVersion();
    
    void ReleaseResLib(void);
    BOOL ChangeLanguage(void);

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRITEFLASH_H__45CE1FC4_81FA_4B6E_BF84_30CE9C2BC6B9__INCLUDED_)





















