/******************************************************************************
 ** File Name:      XAboutDlg.h                                               *
 ** Author:         Hongliang.Xin                                             *
 ** DATE:           2009/10/22                                                *
 ** Copyright:      2009 Spreadtrum Communications Inc. All Rights Reserved.  *
 ** Description:    Header of the CXAboutDlg and CXVerInfo                    *
 **                 This is for "about dialog" with a unique style            *
 ** Note:           None                                                      *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
    0.0.1          2009/10/22       Hongliang.Xin                             *
    
	  The frist version


    0.0.2          2009/11/04		Kai.Liu

      1 If the parent window is a pop up window,move the about dialog to 
	    the center of the screen
	    
	0.0.3          2009/11/05		Hongliang.Xin

      1 Add new method SetProductName for CXAboutDlg
      2 Add new methods  GetAllLangCodePage and SetCurLangCodePage for
        CXVerInfo 
 
    0.0.4          2010/8/27        Kai.Liu
    
      1 Remove pclint warnings.
      
    0.0.5          2011/3/29        Hongliang.Xin   
      1 Merge the version and build number to one verison. Format: x.y.zbbb_Pn
      2 Add new method GetSPRDVersionString

 ******************************************************************************/
 
#if !defined(AFX_XABOUTDLG_H__29277FAD_DD5E_4522_8C31_4177886123BD__INCLUDED_)
#define AFX_XABOUTDLG_H__29277FAD_DD5E_4522_8C31_4177886123BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XAboutDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXAboutDlg dialog

class CXAboutDlg : public CDialog
{
// Construction
public:
/**
 * lpszCodeVer: Mobile Code Version string
 * 
 */
	CXAboutDlg(LPCTSTR lpszCodeVer = NULL,CWnd* pParent = NULL);   // standard constructor
	~CXAboutDlg();
	
	void SetProductName(LPCTSTR lpszProductName);

// Dialog Data
	//{{AFX_DATA(CXAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CBitmap m_bmpDlgBckGrnd;   // not used 
	CBrush  m_brshDlgBckGrnd;
	CString m_strVersion;
	CString m_strBuild;
	CString m_strCopyright;
	CString m_strProductName;
	CString m_strPatch;
	HBITMAP m_hBitmap;        // background bmp handle

	// Generated message map functions
	//{{AFX_MSG(CXAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static const BYTE m_bmpdata[];
	CString  m_strCodeVer;  // Mobile Code Version 
	int      m_nCodeVerHight;

};

/////////////////////////////////////////////////////////////////////////////
// CXVerInfo

typedef struct _LANGANDCODEPAGE_T 
{
	WORD wLanguage;
	WORD wCodePage;
}LANGANDCODEPAGE_T, * LANGANDCODEPAGE_PTR;

class CXVerInfo 
{
public:
	LPCTSTR GetSpecialBuild(void);
	LPCTSTR GetPrivateBuild(void);
	LPCTSTR GetProductVersion(void);
	LPCTSTR GetProductName(void);
	LPCTSTR GetLegalTrademarks(void);
	LPCTSTR GetLegalCopyright(void);
	LPCTSTR GetCompanyName(void);
	LPCTSTR GetComments(void);
	/**
	 *  must call this method first	
	 */ 
	BOOL Init(LPCTSTR pszFileName = NULL);
	/**
	 * need not to alloc buffer for pLangCodePage and need not to free also
	 * nSize is the total size by byte for pLangCodePage.
	 * you can call this to find what language code page you need, and then 
	 * call SetCurLangCodePage.
	 */
	BOOL GetAllLangCodePage(LANGANDCODEPAGE_PTR & pLangCodePage, UINT &nSize);
	/**
	 * Set current lang code page if there are many language code pages.
	 * If you don't set this, it will select the first as default.
	 * As usually, you need not set this.
	 */
	void SetCurLangCodePage(LANGANDCODEPAGE_PTR pLangCodePage);
	
	/**
	 *  Get SPRD version string x.y.zbbb_Pn
	 */
	LPCTSTR GetSPRDVersionString();
	
	
	
	
public:
	CXVerInfo();
	~CXVerInfo();
private:
	LPBYTE m_pVerInfoBuf;
	_TCHAR m_szSubBlock[ 64 ];
	_TCHAR m_szFileName[ _MAX_PATH ];
	_TCHAR m_szSPRDVer[ 128 ];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XABOUTDLG_H__29277FAD_DD5E_4522_8C31_4177886123BD__INCLUDED_)
