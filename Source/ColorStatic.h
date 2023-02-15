#ifndef _COLORSTATIC_H
#define _COLORSTATIC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ColorStatic.h : header file
//

class CColorStatic : public CStatic
{
// Construction
public:
	CColorStatic();

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStaticST)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorStatic();

	void SetTextColor(COLORREF crTextColor = 0xffffffff);
	COLORREF GetTextColor();

	void SetBkColor(COLORREF crBkColor = 0xffffffff);
	COLORREF GetBkColor();

    void SetStaticFont(LPCTSTR lpszFontName, int nWidth, int nHeight);
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStaticST)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COLORREF m_crTextColor;
	COLORREF m_crBkColor;
	CBrush   m_brBkgnd;
    CFont    m_font;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
