// WriteFlash.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WriteFlash.h"
#include "WriteFlashDlg.h"
#include "PhoneCommand.h"

//
SP_HANDLE   g_hDiagPhone = INVALID_HANDLE_VALUE;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWriteFlashApp

BEGIN_MESSAGE_MAP(CWriteFlashApp, CWinApp)
	//{{AFX_MSG_MAP(CWriteFlashApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG

	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteFlashApp construction

CWriteFlashApp::CWriteFlashApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
    m_hResModule = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWriteFlashApp object

CWriteFlashApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWriteFlashApp initialization

BOOL CWriteFlashApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    //
    g_hDiagPhone = SP_CreatePhone(NULL);
    if (INVALID_HANDLE_VALUE == g_hDiagPhone)
    {
        return FALSE;
    }


    GetVersion();

   // LANGID lid = GetSystemDefaultLangID( );
   	ChangeLanguage();
	CWriteFlashDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
void CWriteFlashApp::GetVersion()
{
    CXVerInfo Ver;
    Ver.Init();
    m_strVersion=Ver.GetProductVersion();
    m_strVersion.Replace( _T(","),_T(".") );
    m_strVersion.Replace(_T(" "),_T("") );
}

BOOL CWriteFlashApp::ChangeLanguage()
{
    BOOL bEnglish;
    _TCHAR szPathName[_MAX_PATH];
    ::GetModuleFileName( NULL, szPathName, _MAX_PATH );
    LPTSTR pFind = _tcsrchr(szPathName, _T('\\'));
    if(pFind == NULL )
    {
        return FALSE;
    }
    *(pFind + 1) = _T('\0');
    _tcscat(szPathName,_T("WriteIMEI.ini"));
    bEnglish =  GetPrivateProfileInt(_T("Control"), _T("Language"), 0, szPathName);
    if(!bEnglish)
    {
         *(pFind + 1) = _T('\0');
        _tcscat(szPathName, _T("\\ResChinaIMEI.dll"));
        HINSTANCE hResModule = LoadLibrary(szPathName);
        if( hResModule != NULL )
        {
            ReleaseResLib();
            
            m_hResModule = hResModule;
            AfxSetResourceHandle( m_hResModule );
            
        }
    }
    
    return TRUE;
}

void CWriteFlashApp::ReleaseResLib(void)
{
	if( m_hResModule != NULL )
	{
		FreeLibrary(m_hResModule);
		m_hResModule = NULL;
	}
}

int CWriteFlashApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (INVALID_HANDLE_VALUE != g_hDiagPhone)
    {
        SP_ReleasePhone(g_hDiagPhone);
        g_hDiagPhone = INVALID_HANDLE_VALUE;
    }

	return CWinApp::ExitInstance();
}
