// SpLog.h: interface for the CSpLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLOG_H__A2A17844_8411_4752_A4F1_18F298BAA18B__INCLUDED_)
#define AFX_SPLOG_H__A2A17844_8411_4752_A4F1_18F298BAA18B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
// Include files
#include "ISpLog.h"
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////
//
class CSpLog : public ISpLog  
{
public:
	CSpLog();
	virtual ~CSpLog();

public:
    // Open & Close & Release
    virtual BOOL    Open(LPCTSTR lpszLogPath, UINT uFlags=ISpLog::defaultTextFlag, UINT uLogLevel=SPLOGLV_INFO, UINT uMaxFileSizeInMB=INFINITE_LOGFILE_SIZE);
    virtual BOOL    Close(void);
    virtual void    Release(void) { delete this; };
    
    // Log functions
    virtual BOOL    LogHexData(const BYTE *pHexData, DWORD dwHexSize, UINT uFlag = LOG_READ);

    virtual BOOL    LogRawStrA(UINT uLogLevel, LPCSTR  lpszString);
    virtual BOOL    LogRawStrW(UINT uLogLevel, LPCWSTR lpszString);
    virtual BOOL    LogFmtStrA(UINT uLogLevel, LPCSTR  lpszFmt, ...);
    virtual BOOL    LogFmtStrW(UINT uLogLevel, LPCWSTR lpszFmt, ...);
    virtual BOOL    LogBufData(UINT uLogLevel, const BYTE *pBufData, DWORD dwBufSize, UINT uFlag=LOG_WRITE,const DWORD * pUserNeedSize =NULL);
    
    // Properties
    virtual BOOL    SetProperty(LONG lAttr, LONG lFlags, LPCVOID lpValue);
    virtual BOOL    GetProperty(LONG lAttr, LONG lFlags, LPVOID  lpValue); 

protected:
    HANDLE          OpenFile(LPCTSTR lpszFilePath);

    // path 
    void            GetLogExtension     (LPCTSTR lpszOrgFilePath, LPTSTR lpszExtension,   BOOL bIsTxtLogFile);
	void            GetLogFilePath      (LPCTSTR lpszOrgFilePath, LPTSTR lpszDstFilePath, BOOL bIsTxtLogFile);
    void            GetBinFilePath      (LPCTSTR lpszOrgFilePath, LPTSTR lpszBinFilePath, BOOL bIsReadBinLog);
    void            GetNextPartLog      (LPCTSTR lpszOrgFilePath, LPTSTR lpszNextLogFile, UINT nPartFileCount);
    BOOL            CreateMultiDirectory(LPCTSTR lpszPathName);

    // logging 
	BOOL            LogString(LPCSTR lpszString);
    BOOL            WriteLog(HANDLE &hFile, LPCTSTR lpszFilePath, UINT &nPartFileCount, LPCVOID lpBuffer, DWORD dwNbrOfBytesToWrite);

   
    // Console Window
    void            AllocConsoleWindow(void);
    void            WriteConsoleWindow(LPCVOID lpBuffer, DWORD dwNbrOfBytesToWrite);

    // Local Time
    LPCSTR   GetLocalTime(void) const 
    {
        LPCSTR lpszFmt = "[%04d-%02d-%02d %02d:%02d:%02d:%03d]";
        SYSTEMTIME tms;
        ::GetLocalTime(&tms);
        _snprintf((CHAR *)m_szLocalTime, sizeof(m_szLocalTime), lpszFmt, tms.wYear, tms.wMonth, tms.wDay, \
                    tms.wHour, tms.wMinute, tms.wSecond, tms.wMilliseconds);

        return (LPCSTR)m_szLocalTime;
    };
    #define LOCALTIME_STRING_MAX_LEN    ( 30 )

#ifdef _CRITEICAL
    // Synchronization 
    void     Lock(void)  { ::EnterCriticalSection(&m_cs); };
    void     UnLock(void){ ::LeaveCriticalSection(&m_cs); };
#endif
	

protected:	
    TCHAR               m_szModuleName[_MAX_PATH];      // The name of module file
    TCHAR               m_szLogDirPath[_MAX_PATH];      // The directory path of log file
    TCHAR               m_szDefLogName[_MAX_PATH];      // The default log file name
    TCHAR               m_szLgFilePath[_MAX_PATH];      // The log file path
    TCHAR               m_szBinLogFileExt[_MAX_PATH];

    HANDLE              m_hRwTxtFile;                   // Handle of 'txt log'
    HANDLE              m_hRdBinFile;                   // Handle of 'bin log' that logging 'read data'
    HANDLE              m_hWtBinFile;                   // Handle of 'bin log' that logging 'write data'
    TCHAR               m_szRwTxtLogPath[_MAX_PATH];    // The path of 'txt log'
    TCHAR               m_szRdBinLogPath[_MAX_PATH];    // The path of 'read bin'
    TCHAR               m_szWtBinLogPath[_MAX_PATH];    // The path of 'write bin'
    UINT                m_uFlags;                       // Open flag
    UINT                m_uLogLevel;                    // Log level of 'txt log'

    UINT                m_uMaxFileSizeInMB;             // Max. log file size
    UINT                m_uRwTxtPartFileCount;
    UINT                m_uRdBinPartFileCount;
    UINT                m_uWtBinPartFileCount;

#ifdef _CRITEICAL
    CRITICAL_SECTION    m_cs;
#endif

    //  'txt log' available       ----- [

    //      |       Date & Time     |         
    //      [2009-06-04 16:54:47:500] --> 114756(0x0001c044) Bytes
    //                                | index  |                                                 |      ASCII     |
    //                                00000000h: 4D 5A 90 00 03 00 00 00 04 00 00 00 FF FF 00 00 ; MZ..............
    //                                00000010h: B8 00 00 00 00 00 00 00 40 00 00 00 00 00 00 00 ; ........@.......
    //                                00000020h: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; ................
    //

    BOOL                m_bDspTime;             // Display log date & time like '[2009-06-04 16:54:47:500]'
    BOOL                m_bDspIndex;            // Display HEX buffer data index like '00000000h:'
    BOOL                m_bDspAscii;            // Display HEX buffer data ASCII area like '; MZ..............'
    UINT                m_nLineMaxChars;        // Specifies the max. characters of a line
    CHAR                m_szLocalTime[LOCALTIME_STRING_MAX_LEN];

    // -------------------------------- ]

    // Console Window
    BOOL                m_bConsoleWindow;
    HANDLE              m_hConsoleWindow;
};


//////////////////////////////////////////////////////////////////////////
//
class CFnLog
{
public:
    CFnLog(ISpLog *pLog, LPCSTR  lpszFnName, LPCSTR  lpszFmt=NULL, ...);
    CFnLog(ISpLog *pLog, LPCWSTR lpszFnName, LPCWSTR lpszFmt=NULL, ...);
    virtual ~CFnLog(void);

private:
    ISpLog *m_pFnLog;
    CHAR    m_szFnName[_MAX_PATH];     
};

#endif // !defined(AFX_SPLOG_H__A2A17844_8411_4752_A4F1_18F298BAA18B__INCLUDED_)
