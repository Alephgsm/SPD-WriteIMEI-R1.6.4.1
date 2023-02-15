// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic()
{
	m_crTextColor = ::GetSysColor(COLOR_BTNTEXT);
	m_crBkColor = ::GetSysColor(COLOR_BTNFACE);
	m_brBkgnd.CreateSolidBrush(m_crBkColor);

    m_font.CreateFont(10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Courier"));
} 


CColorStatic::~CColorStatic()
{
	if (INVALID_HANDLE_VALUE != m_font.m_hObject) 
	{
		m_font.DeleteObject();   //lint !e1551
	}
} // End of ~CColorStatic


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers


HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    // xiaoping.jing Warning Fix:
    // warning C4100: 'nCtlColor' : unreferenced formal parameter
    UNREFERENCED_PARAMETER(nCtlColor);

	pDC->SetTextColor(m_crTextColor);
	pDC->SetBkColor(m_crBkColor);

    return (HBRUSH)m_brBkgnd;
} 


void CColorStatic::OnDestroy() 
{
	CStatic::OnDestroy();
	
    m_brBkgnd.DeleteObject();
} 


void CColorStatic::SetTextColor(COLORREF crTextColor)
{
	if (crTextColor != 0xffffffff)
	{
		m_crTextColor = crTextColor;
	}
	else // Set default foreground color
	{
		m_crTextColor = ::GetSysColor(COLOR_BTNTEXT);
	}

	Invalidate();
}


COLORREF CColorStatic::GetTextColor()
{
	return m_crTextColor;
} 


void CColorStatic::SetBkColor(COLORREF crBkColor)
{
	if (crBkColor != 0xffffffff)
	{
		m_crBkColor = crBkColor;
	}
	else // Set default background color
	{
		m_crBkColor = ::GetSysColor(COLOR_BTNFACE);
	}

    m_brBkgnd.DeleteObject();
    m_brBkgnd.CreateSolidBrush(m_crBkColor);

	Invalidate();
}


COLORREF CColorStatic::GetBkColor()
{
	return m_crBkColor;
}

void CColorStatic::SetStaticFont(LPCTSTR lpszFontName, int nWidth, int nHeight)
{
#if 1
    if (m_font.m_hObject)
    {
		m_font.Detach();

		m_font.DeleteObject();
		m_font.m_hObject = NULL;
	}    
    	
    VERIFY(
		m_font.CreateFont(
		nHeight,                        // nHeight
		nWidth,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		lpszFontName)
		);                 // lpszFacename
	
	SetFont(&m_font);
    
	Invalidate(TRUE);
#endif
}

int CColorStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    CColorStatic::OnCreate(lpCreateStruct);

    SetFont(&m_font);

    return 0;
}