//  SpLog.cpp: implementation of the CSpLog class.
//
//  History:
//  -------------
//  6   V1.0.1002   2010/08/31  Kai.Liu
//                  1.  Add Log level to LogBufData
//                  2.  Comparing log level at the begining of every public function.
//                  3.  Don't create text log if input level is SPLOGLV_NONE
//    
//  5   V1.0.1001   2010/08/17  Xiaoping.Jing
//                  1.  Add Log level for TXT log.
//                  2.  Add read/write data for BIN log.
//                  3.  Remove IsBadWritePtr/IsBadReadPtr judgement in 'WriteLog'. 
//                  4.  Add max. log file size property.      
//
//  4   V1.0.0004	2010/07/12  kai.liu
//					1.  Move inline funcions definition to header file to avoid
//                      link error of DiagChan
//                  2.  Remove assert in WriteConsoleWindow
//  3.  V1.0.0003   2009/11/16  hongliang.xin 
//		            a.  modify million second from 4 number to 3 number
//                      16:54:47:0500  --> 16:54:47:500
//                  b.  Log function check file handle first.
//                  c.  add setting binary log extension attribute.
//  2.  V1.0.0002   2009/09/28  hongliang.xin fix the bug of 
//                  writting random data when dwBufSize is zero 
//                  for LoaBufData function.
//  1.  V1.0.0001   2009/07/09  xiaoping.jing  Draft Release
//
//////////////////////////////////////////////////////////////////////
#include "SpLog.h"
#include <shlwapi.h>
#include <assert.h>
#include <wincon.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// win. path lib
#pragma comment(lib, "shlwapi.lib")


// 增加临界控制,可能扰乱某些app的时序 
// #define  _CRITEICAL

//
#define LOG_DIRECTORY               ( _T( "Log") )
#define BINARY_EXTENSION            ( _T(".bin") )
#define TEXT_EXTENSION              ( _T(".log") )


#define GetFileNameWithoutExtension(_fn)      {       \
    LPTSTR pPos = _tcsrchr((_fn), _T('.'));   \
    if (NULL != pPos) {     \
    *pPos = _T('\0');   \
    }   \
}

extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow(void);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpLog::CSpLog()
{
    m_bDspTime      = TRUE;
    m_bDspAscii     = TRUE;
    m_bDspIndex     = TRUE;
    m_nLineMaxChars = MAX_LINE_HEX_BYTES;

    m_bConsoleWindow    = FALSE;
    m_hConsoleWindow    = INVALID_HANDLE_VALUE;
    
    m_uFlags        = ISpLog::defaultTextFlag;
    m_uLogLevel     = SPLOGLV_INFO;
    m_hRwTxtFile    = INVALID_HANDLE_VALUE;
    m_hRdBinFile    = INVALID_HANDLE_VALUE;
    m_hWtBinFile    = INVALID_HANDLE_VALUE;

    m_uMaxFileSizeInMB      = 0;  // default file size: infinite
    m_uRwTxtPartFileCount   = 0;
    m_uRdBinPartFileCount   = 0;
    m_uWtBinPartFileCount   = 0;

    ZeroMemory(m_szLocalTime,       sizeof(m_szLocalTime));
    ZeroMemory(m_szModuleName,      sizeof(m_szModuleName));
    ZeroMemory(m_szLogDirPath,      sizeof(m_szLogDirPath));
    ZeroMemory(m_szDefLogName,      sizeof(m_szDefLogName));
    ZeroMemory(m_szLgFilePath,      sizeof(m_szLgFilePath));
    ZeroMemory(m_szBinLogFileExt,   sizeof(m_szBinLogFileExt));
    ZeroMemory(m_szRwTxtLogPath,    sizeof(m_szRwTxtLogPath));
    ZeroMemory(m_szRdBinLogPath,    sizeof(m_szRdBinLogPath));
    ZeroMemory(m_szWtBinLogPath,    sizeof(m_szWtBinLogPath));
    
    // default log name and directory 
    TCHAR szAppPath[_MAX_PATH] = {0};
    GetModuleFileName(NULL, szAppPath, _MAX_PATH);
    _tcscpy(m_szModuleName, PathFindFileName(szAppPath));
    GetFileNameWithoutExtension(m_szModuleName);
    _tcscpy(m_szDefLogName, m_szModuleName);
    
    PathRemoveFileSpec(szAppPath);
    _sntprintf(m_szLogDirPath, (_MAX_PATH-1), _T("%s\\%s"), szAppPath, LOG_DIRECTORY);
    
    
#ifdef _CRITEICAL
    ::InitializeCriticalSection(&m_cs);
#endif
}

CSpLog::~CSpLog()
{
    Close(); //lint !e1506 !e1551
    
    if (INVALID_HANDLE_VALUE != m_hConsoleWindow)
    {
        FreeConsole();
        m_hConsoleWindow = INVALID_HANDLE_VALUE;
    }
    
#ifdef _CRITEICAL
    ::DeleteCriticalSection(&m_cs);  
#endif

    m_hRdBinFile     = NULL;
    m_hRwTxtFile     = NULL;
    m_hWtBinFile     = NULL;
    m_hConsoleWindow = NULL;
}

BOOL CSpLog::Open(LPCTSTR lpszLogPath, 
                  UINT uFlags /* =ISpLog::defaultTextFlag */, 
                  UINT uLogLevel /* =SPLOGLV_INFO*/,
                  UINT uMaxFileSizeInMB/* =INFINITE_LOGFILE_SIZE*/)
{
    Close();
    
    m_uFlags            = uFlags;
    m_uLogLevel         = uLogLevel;
    m_uMaxFileSizeInMB  = uMaxFileSizeInMB;

    if ( !(m_uFlags & ISpLog::typeText) && !(m_uFlags & ISpLog::typeBinary) )
    {
        // default 'txt type'
        m_uFlags |= ISpLog::typeText;
    }
    
    if ( m_uFlags & ISpLog::typeText && uLogLevel > SPLOGLV_NONE )
    {
        // 'txt file'
        ZeroMemory(m_szRwTxtLogPath, sizeof(m_szRwTxtLogPath));
        GetLogFilePath(lpszLogPath, m_szRwTxtLogPath, TRUE);
        m_hRwTxtFile = OpenFile(m_szRwTxtLogPath);
        if (INVALID_HANDLE_VALUE == m_hRwTxtFile)
        {
            return FALSE;
        }
    }
    //@hongliang.xin 2010-10-15 not create Log dir if modeBinNone
    if ( (m_uFlags & ISpLog::typeBinary) && (m_uFlags & 0x0F00))
    {
        // bin file
        TCHAR szBinLogPath[_MAX_PATH] = {0};
        GetLogFilePath(lpszLogPath, szBinLogPath, FALSE);
        
        if (m_uFlags & ISpLog::modeBinRead)
        {
            // 'read  bin'
            ZeroMemory(m_szRdBinLogPath, sizeof(m_szRdBinLogPath));
            GetBinFilePath(szBinLogPath, m_szRdBinLogPath, TRUE);
            m_hRdBinFile = OpenFile(m_szRdBinLogPath);
            if (INVALID_HANDLE_VALUE == m_hRdBinFile)
            {
                return FALSE;
            }
        }

        if (m_uFlags & ISpLog::modeBinWrite)
        {
            // 'write bin'
            ZeroMemory(m_szWtBinLogPath, sizeof(m_szWtBinLogPath));
            GetBinFilePath(szBinLogPath, m_szWtBinLogPath, FALSE);
            m_hWtBinFile = OpenFile(m_szWtBinLogPath);
            if (INVALID_HANDLE_VALUE == m_hWtBinFile)
            {
                return FALSE;
            }
        }
    }
    
    // -----------------------------------------------------------------------------------
    // Alloc console window if need
    //
    if (m_bConsoleWindow)
    {
        AllocConsoleWindow();
    }
    
    return TRUE;
}

BOOL CSpLog::Close()
{
    if (INVALID_HANDLE_VALUE != m_hRwTxtFile)
    {
        // 'txt'
        FlushFileBuffers(m_hRwTxtFile);
        CloseHandle(m_hRwTxtFile);
        m_hRwTxtFile = INVALID_HANDLE_VALUE;
    }
    
    if (INVALID_HANDLE_VALUE != m_hRdBinFile)
    {
        // 'read bin'
        FlushFileBuffers(m_hRdBinFile);
        CloseHandle(m_hRdBinFile);
        m_hRdBinFile = INVALID_HANDLE_VALUE;
    }
    
    if (INVALID_HANDLE_VALUE != m_hWtBinFile)
    {
        // 'write bin'
        FlushFileBuffers(m_hWtBinFile);
        CloseHandle(m_hWtBinFile);
        m_hWtBinFile = INVALID_HANDLE_VALUE;
    }

    if (INVALID_HANDLE_VALUE != m_hConsoleWindow)
    {
        FreeConsole();
        m_hConsoleWindow = INVALID_HANDLE_VALUE;
    }
    
    return TRUE;
}

BOOL CSpLog::SetProperty(LONG lAttr, LONG lFlags, LPCVOID lpValue)
{
    BOOL bOperRes = TRUE;
    
    switch(lAttr)
    {
    case ATTR_LOGFILE_PATH:
        {
            if (PATH_CURRENT == lFlags)
            {
                // Current work directory path
                TCHAR szAppPath[_MAX_PATH] = {0};
                GetModuleFileName(NULL, szAppPath, _MAX_PATH);
                PathRemoveFileSpec(szAppPath);
                if (NULL == lpValue)
                {
                    _sntprintf(m_szLogDirPath, (_MAX_PATH-1), _T("%s\\%s"), szAppPath, LOG_DIRECTORY);
                }
                else
                {
                    _sntprintf(m_szLogDirPath, (_MAX_PATH-1), _T("%s\\%s\\%s"), szAppPath, LOG_DIRECTORY, (LPCTSTR)lpValue);
                }
            }
            else if (PATH_WINTEMP == lFlags)
            {
                //  Window temp directory path
                TCHAR szTmpPath[_MAX_PATH] = {0};
                GetTempPath(_MAX_PATH, szTmpPath);
                if (NULL == lpValue)
                {
                    _sntprintf(m_szLogDirPath, (_MAX_PATH-1), _T("%s\\%s"), szTmpPath, LOG_DIRECTORY);
                }
                else
                {
                    _sntprintf(m_szLogDirPath, (_MAX_PATH-1), _T("%s\\%s\\%s"), szTmpPath, LOG_DIRECTORY, (LPCTSTR)lpValue);
                }
            }
            else if (PATH_USERDFN == lFlags)
            {
                // User defined path
                LPCTSTR lpszPath = (LPCTSTR)lpValue;
                if (NULL == lpszPath)
                {
                    bOperRes = FALSE;
                }
                else
                {
                    _sntprintf(m_szLogDirPath, (_MAX_PATH-1), _T("%s"), lpszPath);
                }
            }
            else
            {
                // Invalid flag
                bOperRes = FALSE;
            }
        }
        break;
        
    case ATTR_LOGFILE_NAME:
        {
            LPCTSTR lpFileName = (LPCTSTR)lpValue;
            if (NULL == lpFileName)
            {
                bOperRes = FALSE;
            }
            else
            {
                _sntprintf(m_szDefLogName, (_MAX_PATH-1), _T("%s"), lpFileName);
            }
        }
        break;
        
    case ATTR_MAX_LINEBYTES:
        {
            m_nLineMaxChars = (UINT)lpValue;
            if (0 == m_nLineMaxChars || m_nLineMaxChars > MAX_LINE_HEX_BYTES)
            {
                // Line maximum characters
                m_nLineMaxChars = MAX_LINE_HEX_BYTES;
            }
        }
        break;
        
    case ATTR_LOG_LOCALTIME:
        {
            m_bDspTime  = (BOOL)lpValue;
        }
        break;
        
    case ATTR_LOG_DATAINDEX:
        {
            m_bDspIndex = (BOOL)lpValue;
        }
        break;
        
    case ATTR_LOG_DATAASCII:
        {
            m_bDspAscii = (BOOL)lpValue;
        }
        break;
        
    case ATTR_CONSOLE_WINDOW:
        {
            UINT uCmdShow = (UINT)lpValue;
            if (FREE_CONSOLE_WINDOW == uCmdShow)
            {
                // free
                m_bConsoleWindow = FALSE;
                if (INVALID_HANDLE_VALUE != m_hConsoleWindow)
                {
                    FreeConsole();
                    m_hConsoleWindow = INVALID_HANDLE_VALUE;
                }
            }
            else if (SHOW_CONSOLE_WINDOW == uCmdShow)
            {
                // show
                m_bConsoleWindow = TRUE;
                AllocConsoleWindow();
                ShowWindow(GetConsoleWindow(), SW_SHOW);
            }
            else if (HIDE_CONSOLW_WINDOW == uCmdShow)
            {
                // hide 
                m_bConsoleWindow = TRUE;
                AllocConsoleWindow();
                ShowWindow(GetConsoleWindow(), SW_HIDE);
            }
            else
            {
                bOperRes = FALSE;
            }
        }
        break;
        
    case ATTR_BIN_LOG_EXT:
        {
            if(lpValue != NULL && _tcslen((LPCTSTR)lpValue)< _MAX_PATH)
            {
            	_tcscpy(m_szBinLogFileExt,(LPCTSTR)lpValue);
            }
            else
            {
            	bOperRes = FALSE;
            }
        }
        break;

    case ATTR_MAX_LOGFILE_SIZE:
        {
            m_uMaxFileSizeInMB = (UINT)lpValue;
        }
        break;
        
    default:
        bOperRes = FALSE;
        break;
    }
    
    
    return bOperRes;
}

BOOL CSpLog::GetProperty(LONG lAttr, LONG lFlags, LPVOID  lpValue)
{
    UNREFERENCED_PARAMETER(lFlags);
    BOOL bOperRes = TRUE;
    
    switch(lAttr)
    {
    case ATTR_LOGFILE_PATH:
        {
            if ( IsBadWritePtr(lpValue, sizeof(TCHAR)*_MAX_PATH) )
            {
                bOperRes = FALSE;
            }
            else
            {
                _tcscpy((LPTSTR)lpValue, m_szLogDirPath);
            }
            
        }
        break;
        
    case ATTR_LOGFILE_NAME:
        {
            if ( IsBadWritePtr(lpValue, sizeof(TCHAR)*_MAX_PATH) )
            {
                bOperRes = FALSE;
            }
            else
            {
                _tcscpy((LPTSTR)lpValue, m_szDefLogName);
            }
        }
        break;
        
    case ATTR_MAX_LINEBYTES:
        {
            if ( IsBadWritePtr(lpValue, sizeof(UINT)) )
            {
                bOperRes = FALSE;
            }
            else
            {
                *((UINT *)lpValue) = (UINT)m_nLineMaxChars;
            }
        }
        break;
        
    case ATTR_LOG_LOCALTIME:
        {
            if ( IsBadWritePtr(lpValue, sizeof(UINT)) )
            {
                bOperRes = FALSE;
            }
            else
            {
                *((UINT *)lpValue) = m_bDspTime ? 1 : 0;
            }
        }
        break;
        
    case ATTR_LOG_DATAINDEX:
        {
            if ( IsBadWritePtr(lpValue, sizeof(UINT)) )
            {
                bOperRes = FALSE;
            }
            else
            {
                *((UINT *)lpValue) = m_bDspIndex ? 1 : 0;
            }
        }
        break;
        
    case ATTR_LOG_DATAASCII:
        {
            if ( IsBadWritePtr(lpValue, sizeof(UINT)) )
            {
                bOperRes = FALSE;
            }
            else
            {
                *((UINT *)lpValue) = m_bDspAscii ? 1 : 0;
            }
        }
        break;
        
    case ATTR_CONSOLE_WINDOW:
        {
            if ( IsBadWritePtr(lpValue, sizeof(UINT)) )
            {
                bOperRes = FALSE;
            }
            else
            {
                *((UINT *)lpValue) = m_bConsoleWindow ? 1 : 0;   
            }
        }
        break;
        
    case ATTR_BIN_LOG_EXT:
        {
            if ( IsBadWritePtr(lpValue, sizeof(TCHAR)*_MAX_PATH) )
            {
                bOperRes = FALSE;
            }
            else
            {
            	if(_tcslen(m_szBinLogFileExt)!= 0)
            	{
                	_tcscpy((LPTSTR)lpValue, m_szBinLogFileExt);
                }
                else
                {
                	_tcscpy((LPTSTR)lpValue, BINARY_EXTENSION);
                }
            }
            
        }
        break;

    case ATTR_MAX_LOGFILE_SIZE:
        {
            if ( IsBadWritePtr(lpValue, sizeof(DWORD)) )
            {
                bOperRes = FALSE;
            }
            else
            {
                *((UINT *)lpValue) = m_uMaxFileSizeInMB;   
            }
        }
        break;
        
    default:
        bOperRes = FALSE;
        break;
    }
    
    return bOperRes;
}

BOOL CSpLog::WriteLog(HANDLE &hFile, LPCTSTR lpszFilePath, UINT &nPartFileCount, LPCVOID lpBuffer, DWORD dwNbrOfBytesToWrite)
{
    if (INVALID_HANDLE_VALUE == hFile) 
    {
        return TRUE;
    }

    if (m_uMaxFileSizeInMB > 0)
    {
#define MB      (1*1024*1024)
        DWORD dwCurrFileSize = GetFileSize(hFile, NULL) / MB;
        
        if (dwCurrFileSize > m_uMaxFileSizeInMB)
        {
            nPartFileCount++;
            
            FlushFileBuffers(hFile);
            CloseHandle(hFile);
            
            TCHAR szNextLogFile[_MAX_PATH] = {0};
            GetNextPartLog(lpszFilePath, szNextLogFile, nPartFileCount);
           
            hFile = OpenFile(szNextLogFile);
            if (INVALID_HANDLE_VALUE == hFile)
            {
                return FALSE;
            }
        }
    }
    
#ifdef _CRITEICAL
    Lock();
#endif
    
    DWORD nbrOfBytesWritten = 0;
    BOOL  bOperRes = ::WriteFile(hFile, lpBuffer, dwNbrOfBytesToWrite, &nbrOfBytesWritten, NULL);
    
    // Output to console window
    if (m_bConsoleWindow)
    {
        WriteConsoleWindow(lpBuffer, nbrOfBytesWritten);
    }
    
#ifdef _CRITEICAL
    UnLock();
#endif

    
    return bOperRes;
}

BOOL CSpLog::LogString(LPCSTR lpszString)
{
    char szLogBuffer[MAX_STRING_IN_BYTES+LOCALTIME_STRING_MAX_LEN] = {0};
    if (m_bDspTime)
    {
        // Log local time as "[2009-05-25 12:30:52:0136]...
        _snprintf(szLogBuffer, sizeof(szLogBuffer)-1, "%s %s\r\n", GetLocalTime(), lpszString);
        return WriteLog(m_hRwTxtFile, m_szRwTxtLogPath, m_uRwTxtPartFileCount, szLogBuffer, lstrlenA(szLogBuffer));        
    }
    else
    {
        _snprintf(szLogBuffer, sizeof(szLogBuffer)-1, "%s\r\n", lpszString);
        return WriteLog(m_hRwTxtFile, m_szRwTxtLogPath, m_uRwTxtPartFileCount, szLogBuffer, lstrlenA(szLogBuffer));
    }
}

void CSpLog::WriteConsoleWindow(LPCVOID lpBuffer, DWORD dwNbrOfBytesToWrite)
{
    if (INVALID_HANDLE_VALUE != m_hConsoleWindow && NULL != lpBuffer && 0 != dwNbrOfBytesToWrite)
    {
        DWORD dwNbrofBytesWritten = 0;
        WriteConsole(m_hConsoleWindow, lpBuffer, dwNbrOfBytesToWrite, &dwNbrofBytesWritten, NULL);
    }
}

BOOL CSpLog::LogHexData(const BYTE *pHexData, DWORD dwHexSize, UINT uFlag /* = LOG_READ */)
{
    if (LOG_READ == uFlag || LOG_ASYNC_READ == uFlag)
    {
        return WriteLog(m_hRdBinFile, m_szRdBinLogPath, m_uRdBinPartFileCount, pHexData, dwHexSize);
    }
    else
    {
        return WriteLog(m_hWtBinFile, m_szWtBinLogPath, m_uWtBinPartFileCount, pHexData, dwHexSize);
    }
}

BOOL CSpLog::LogRawStrA(UINT uLogLevel, LPCSTR lpszString)
{
    assert(NULL != lpszString);

	if(m_uLogLevel == SPLOGLV_DATA)
	{
		if(uLogLevel != SPLOGLV_DATA)
		{
			return TRUE;
		}
	}
	else
	{
		if( uLogLevel > m_uLogLevel)
		{
			return TRUE;
		}
	}

    return LogString(lpszString);
}

BOOL CSpLog::LogRawStrW(UINT uLogLevel, LPCWSTR lpszString)
{
    assert(NULL != lpszString);

	if(m_uLogLevel == SPLOGLV_DATA)
	{
		if(uLogLevel != SPLOGLV_DATA)
		{
			return TRUE;
		}
	}
	else
	{
		if( uLogLevel > m_uLogLevel)
		{
			return TRUE;
		}
	}
    
    char szBuffer[MAX_STRING_IN_BYTES] = {0};
    WideCharToMultiByte(CP_ACP, 0, lpszString, -1, szBuffer, sizeof(szBuffer), NULL, NULL);
    
    return LogString((LPCSTR)szBuffer);
}

BOOL CSpLog::LogFmtStrA(UINT uLogLevel, LPCSTR lpszFmt, ...)
{
    assert(NULL != lpszFmt);

	if(m_uLogLevel == SPLOGLV_DATA)
	{
		if(uLogLevel != SPLOGLV_DATA)
		{
			return TRUE;
		}
	}
	else
	{
		if( uLogLevel > m_uLogLevel)
		{
			return TRUE;
		}
	}

    char     szString[MAX_STRING_IN_BYTES] = {0};
    va_list  args;
    va_start(args, lpszFmt);
    _vsnprintf(szString, sizeof(szString), lpszFmt, args);
    va_end(args);
    
    return LogString((LPCSTR)szString);
}

BOOL CSpLog::LogFmtStrW(UINT uLogLevel, LPCWSTR lpszFmt, ...)
{
	if(m_uLogLevel == SPLOGLV_DATA)
	{
		if(uLogLevel != SPLOGLV_DATA)
		{
			return TRUE;
		}
	}
	else
	{
		if( uLogLevel > m_uLogLevel)
		{
			return TRUE;
		}
	}

    assert(NULL != lpszFmt);
    WCHAR  szwBuffer[MAX_STRING_IN_BYTES/2] = {0};
    va_list  args;
    va_start(args, lpszFmt);
    _vsnwprintf(szwBuffer, sizeof(szwBuffer), lpszFmt, args);
    va_end(args);
    
    char szString[MAX_STRING_IN_BYTES] = {0};
    WideCharToMultiByte(CP_ACP, 0, szwBuffer, -1, szString, sizeof(szString), NULL, NULL);
    
    return LogString((LPCSTR)szString);
}

BOOL CSpLog::LogBufData(UINT uLogLevel, const BYTE *pBufData, DWORD dwBufSize, UINT uFlag/* =LOG_WRITE */,const DWORD * pUserNeedSize/* =NULL*/)
{
	if(m_uLogLevel == SPLOGLV_DATA)
	{
		if(uLogLevel != SPLOGLV_DATA)
		{
			return TRUE;
		}
	}
	else
	{
		if( uLogLevel > m_uLogLevel)
		{
			return TRUE;
		}
	}

    // Example: [2009-05-27 15:06:47:0453] --> 110625(0x0001b021) Bytes
    char szPrefix[50] = {0};
    if(pUserNeedSize == NULL)
    {
	    switch(uFlag)
	    {
	    case LOG_READ:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x) Bytes", "<--", dwBufSize, dwBufSize);
	        break;
	    case LOG_WRITE:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x) Bytes", "-->", dwBufSize, dwBufSize);
	        break;
	    case LOG_ASYNC_READ:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x) Bytes", "<<-", dwBufSize, dwBufSize);
	        break;
	    default:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x) Bytes", "---", dwBufSize, dwBufSize);
	        break;
	    }
    }
    else
    {
    	switch(uFlag)
	    {
	    case LOG_READ:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x)/%d(0x%08x) Bytes", "<--", 
	                dwBufSize, dwBufSize,*pUserNeedSize,*pUserNeedSize);
	        break;
	    case LOG_WRITE:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x)/%d(0x%08x) Bytes", "-->", 
	        	dwBufSize, dwBufSize,*pUserNeedSize,*pUserNeedSize);
	        break;
	    case LOG_ASYNC_READ:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x)/%d(0x%08x) Bytes", "<<-", 
	        	dwBufSize, dwBufSize,*pUserNeedSize,*pUserNeedSize);
	        break;
	    default:
	        _snprintf(szPrefix, sizeof(szPrefix)-1, "%s %d(0x%08x)/%d(0x%08x) Bytes", "---", 
	        	dwBufSize, dwBufSize,*pUserNeedSize,*pUserNeedSize);
	        break;
	    }
    }
    if ( !LogString(szPrefix) )
    {
        return FALSE;
    }
    
    if( 0 == dwBufSize)
    {
    	return TRUE;
    }
    
    // Blank alignment
    char szBlankAlign[50] = {0};
    if (m_bDspTime)
    {
        memset(szBlankAlign, VK_SPACE, lstrlenA(m_szLocalTime)+1);
    }
    
    // Line number
    INT nMaxLine = dwBufSize % m_nLineMaxChars ? (dwBufSize/m_nLineMaxChars+1) : dwBufSize/m_nLineMaxChars; 
    INT nLineNo  = 0;
    const BYTE *pChar = pBufData;
    do 
    {
        // 
        char szLine[MAX_LINE_HEX_BYTES*5+100] = {0};
        char szBuff[MAX_LINE_HEX_BYTES*3+20]  = {0};
        char szAsci[MAX_LINE_HEX_BYTES*2]     = {0};
        
        if (m_bDspIndex)
        {
            // Example: 00000010h:
            _snprintf(szBuff, sizeof(szBuff)-1, "%08xh: ", ((UINT)nLineNo)*m_nLineMaxChars);
        }
        
        for (UINT n=0; n<m_nLineMaxChars; n++, pChar++)
        {
            char szHex[5] = {0};
            sprintf(szHex, "%02X ", *pChar);
            lstrcatA(szBuff, szHex);
            
            if (m_bDspAscii)
            {
                // ASCII
                szAsci[n] = isprint(*pChar) ? *pChar : '.';
            }
            
            if (dwBufSize == (DWORD)(((UINT)nLineNo)*m_nLineMaxChars+n+1))
            {
                // Reaching the last byte
                if (n < m_nLineMaxChars)
                {
                    // Last line 
                    for (UINT j=n+1; j<m_nLineMaxChars; j++)
                    {
                        lstrcatA(szBuff, "   ");
                    }
                }
                
                break;
            }
        }
        
        if (m_bDspAscii)
        {
            _snprintf(szLine, sizeof(szLine)-1, "%s%s; %s\r\n", szBlankAlign, szBuff, szAsci);
        }
        else
        {
            _snprintf(szLine, sizeof(szLine)-1, "%s%s;\r\n", szBlankAlign, szBuff);
        }
        
        
        // Writing
        if ( !WriteLog(m_hRwTxtFile, m_szRwTxtLogPath, m_uRwTxtPartFileCount, szLine, lstrlenA(szLine)) )
        {
            return FALSE;
        }
        
        
    } while(++nLineNo < nMaxLine);
    
    
    return TRUE;
}

void CSpLog::AllocConsoleWindow()
{
    if (INVALID_HANDLE_VALUE != m_hConsoleWindow)
    {
        return ;
    }
    else
    {
        AllocConsole();
        m_hConsoleWindow = GetStdHandle(STD_OUTPUT_HANDLE);
        if (INVALID_HANDLE_VALUE != m_hConsoleWindow)
        {
            COORD coord;
            coord.X = 1000;
            coord.Y = 0x7FFF-1;
            SetConsoleScreenBufferSize(m_hConsoleWindow, coord);
            
            SMALL_RECT rc; 
            rc.Top    = 0;
            rc.Left   = 0;
            rc.Right  = 108;
            rc.Bottom = 30;
            SetConsoleWindowInfo(m_hConsoleWindow, TRUE, &rc); 
            
            //
            SetConsoleTitle(m_szModuleName);
            Sleep(50);
            
            //
            HWND hWnd = ::GetConsoleWindow();
            if (NULL != hWnd)
            {
                // Disable Close button
                ::EnableMenuItem(::GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
            }
            
            ::ShowWindow(hWnd, SW_HIDE);
        }
    }
}

BOOL CSpLog::CreateMultiDirectory(LPCTSTR lpszPathName)
{
    if (NULL == lpszPathName)
    {
        return FALSE;
    }
    
    int nLen = _tcslen(lpszPathName);
    if (nLen < 2)
    {
        return FALSE;
    }
    
    TCHAR szPathName[_MAX_PATH] = {0};
    _tcsncpy(szPathName, lpszPathName, _MAX_PATH-1);
    if ( _T('\\') == szPathName[_tcslen(szPathName)-1] )
    {
        // Removes the trailing backslash
        szPathName[_tcslen(szPathName)-1] = _T('\0');
    }
    
    DWORD dwFileAttr = GetFileAttributes(szPathName);
    if (   (dwFileAttr != (DWORD)-1) 
        && (dwFileAttr&FILE_ATTRIBUTE_DIRECTORY) )
    {
        return TRUE;
    }
    
    
    TCHAR szTmpPath[_MAX_PATH] = {0};
    LPTSTR lpDest = _tcsrchr(szPathName, _T('\\'));
    if (NULL != lpDest)
    {
        _tcsncpy(szTmpPath, szPathName, lpDest-szPathName);
    }
    else
    {
        return FALSE;
    }
    
    if ( CreateMultiDirectory(szTmpPath) )
    {        
        // Create directory ...
        // @hongliang.xin 2010-10-15 enhance the code
 		if(!CreateDirectory(szPathName, NULL))
 		{
 			if(GetLastError() != ERROR_ALREADY_EXISTS)
 			{
 				return FALSE;
 			}
 		} 	
 		return TRUE; 	
    }
    else
    {
        return FALSE;
    }
}

// --------------------------------------------------------------------------------
//  Get the extension name of the path 
//
//  lpszOrgFilePath : [IN ], specifies the path to get extension 
//  lpszFileExtName : [OUT], specifies the extension name to return 
//  bIsTxtLogFile   : [IN ], specifies whether the path file is 'txt log' or 'bin log'

void CSpLog::GetLogExtension(LPCTSTR lpszOrgFilePath, LPTSTR lpszFileExtName, BOOL bIsTxtLogFile)
{
    assert(NULL != lpszOrgFilePath);
    assert(NULL != lpszFileExtName);
    
    LPTSTR lpszExtension = PathFindExtension(lpszOrgFilePath);
    if (NULL != lpszExtension)
    {
        if ( 0 == lstrlen(lpszExtension) )
        {
            lpszExtension = NULL;
        }
        else
        {
            if ( (m_uFlags & ISpLog::typeText) && (m_uFlags & ISpLog::typeBinary) )
            {
                // Open both 'txt log' and 'bin log' once time
                if ( bIsTxtLogFile )
                {
                    // 'txt log' that not use '.bin'
                    if ( 0 == _tcsicmp(lpszExtension, BINARY_EXTENSION) )
                    {
                        lpszExtension = NULL;
                    }
                }
                else
                {
                    if (_tcslen(m_szBinLogFileExt) == 0)
                    {
                        lpszExtension = NULL;
                    }
                    else
                    {
                        lpszExtension =  m_szBinLogFileExt;
                    }
                }
            }
        }
    }
    
    if ( (m_uFlags & ISpLog::typeBinary) && !bIsTxtLogFile && _tcslen(m_szBinLogFileExt) != 0)
    {
        lpszExtension =  m_szBinLogFileExt;
    }
    

    _tcscpy(lpszFileExtName, ((NULL!=lpszExtension) ? lpszExtension : (bIsTxtLogFile?TEXT_EXTENSION:BINARY_EXTENSION)));
}

// --------------------------------------------------------------------------------
//  Get the log file path
//
void CSpLog::GetLogFilePath(LPCTSTR lpszOrgFilePath, LPTSTR lpszDstFilePath, BOOL bIsTxtLogFile)
{
    assert(NULL != lpszDstFilePath);

    TCHAR szLogFileName[_MAX_PATH] = {0};
    TCHAR szOrgFilePath[_MAX_PATH] = {0};
    TCHAR szFileExtName[_MAX_PATH] = {0};
    
    if ( NULL == lpszOrgFilePath )
    {
        _sntprintf(szOrgFilePath, (_MAX_PATH-1), _T("%s"), m_szDefLogName);
    }
    else
    {
        _sntprintf(szOrgFilePath, (_MAX_PATH-1), _T("%s"), lpszOrgFilePath);
    }
    
    // get file name without extension name
    _tcscpy(szLogFileName, PathFindFileName(szOrgFilePath));
    GetFileNameWithoutExtension(szLogFileName);
    
    // get file extension name 
    GetLogExtension(szOrgFilePath, szFileExtName, bIsTxtLogFile);
    
    
    if ( PathIsRelative(szOrgFilePath) )
    {
        // relative path...
        
        // create directories
        CreateMultiDirectory(m_szLogDirPath);
        
        //
        if (m_uFlags & ISpLog::modeTimeSuffix)
        {
            // append date & time suffix 
            SYSTEMTIME  t;
            ::GetLocalTime(&t);
            _sntprintf(lpszDstFilePath, (_MAX_PATH-1), _T("%s\\%s_%04d_%02d_%02d_%02d_%02d_%02d_%03d%s"), \
                m_szLogDirPath, szLogFileName, \
                t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, \
                szFileExtName);
        }
        else if (m_uFlags & ISpLog::modeDateSuffix)
        {
            // append date suffix
            SYSTEMTIME  t;
            ::GetLocalTime(&t);
            _sntprintf(lpszDstFilePath, (_MAX_PATH-1), _T("%s\\%s_%04d_%02d_%02d%s"), \
                m_szLogDirPath, szLogFileName, t.wYear, t.wMonth, t.wDay, szFileExtName);
        }
        else
        {
            _sntprintf(lpszDstFilePath, (_MAX_PATH-1), _T("%s\\%s%s"), m_szLogDirPath, szLogFileName, szFileExtName);
        }
    }
    else
    {
        // absolute path...
        PathRemoveFileSpec(szOrgFilePath);
        _sntprintf(lpszDstFilePath, (_MAX_PATH-1), _T("%s\\%s%s"), szOrgFilePath, szLogFileName, szFileExtName);
    }
}

void CSpLog::GetBinFilePath(LPCTSTR lpszOrgFilePath, LPTSTR lpszBinFilePath, BOOL bIsReadBinLog)
{
    TCHAR szLogFileDir [_MAX_PATH] = {0};
    TCHAR szLogFileName[_MAX_PATH] = {0};
    
    // get file name without extension name
    _tcscpy(szLogFileName,  PathFindFileName(lpszOrgFilePath));
    GetFileNameWithoutExtension(szLogFileName);

    LPCTSTR lpszExtension = PathFindExtension(lpszOrgFilePath);

    _tcscpy(szLogFileDir, lpszOrgFilePath);
    PathRemoveFileSpec(szLogFileDir);

    if (bIsReadBinLog)
    {
        _sntprintf(lpszBinFilePath, (_MAX_PATH-1), _T("%s\\%s_Rd%s"), szLogFileDir, szLogFileName, lpszExtension);
    }
    else
    {
        _sntprintf(lpszBinFilePath, (_MAX_PATH-1), _T("%s\\%s_Wt%s"), szLogFileDir, szLogFileName, lpszExtension);
    }
}

void CSpLog::GetNextPartLog(LPCTSTR lpszOrgFilePath, LPTSTR lpszNextLogFile, UINT nPartFileCount)
{
    TCHAR szLogFileDir [_MAX_PATH] = {0};
    TCHAR szLogFileName[_MAX_PATH] = {0};
    
    
    // get file name without extension name
    _tcscpy(szLogFileName,  PathFindFileName(lpszOrgFilePath));
    GetFileNameWithoutExtension(szLogFileName);
    
    LPCTSTR lpszExtension = PathFindExtension(lpszOrgFilePath);
    
    _tcscpy(szLogFileDir, lpszOrgFilePath);
    PathRemoveFileSpec(szLogFileDir);
    
    _sntprintf(lpszNextLogFile, (_MAX_PATH-1), _T("%s\\%s_%d%s"), \
                    szLogFileDir, szLogFileName, nPartFileCount, lpszExtension);
}

HANDLE CSpLog::OpenFile(LPCTSTR lpszFilePath)
{
    DWORD dwCreationDisposition = (m_uFlags & ISpLog::modeNoTruncate) ? OPEN_ALWAYS : CREATE_ALWAYS;
    HANDLE hFile = ::CreateFile(lpszFilePath, 
        GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, 
        NULL, 
        dwCreationDisposition,
        FILE_ATTRIBUTE_NORMAL,
        NULL );
    if (INVALID_HANDLE_VALUE != hFile)
    {
        if (m_uFlags & ISpLog::modeNoTruncate)
        {
            // move to the end of file
            ::SetFilePointer(hFile, 0, NULL, FILE_END);
        }
    }
    
    return hFile;
}




//////////////////////////////////////////////////////////////////////////
//
CFnLog::CFnLog(ISpLog *pLog, LPCSTR lpszFnName, LPCSTR lpszFmt/* =NULL */, ...)
: m_pFnLog(pLog)
{
    if (NULL != m_pFnLog)
    {
        assert(NULL != lpszFnName);
        _snprintf(m_szFnName, sizeof(m_szFnName)-1, "%s", lpszFnName);
        
        if ( NULL == lpszFmt )
        {
            m_pFnLog->LogFmtStrA(SPLOGLV_VERBOSE, "Enter [%s]", m_szFnName);
        }
        else
        {
            CHAR     szArgs[1024] = {0};
            va_list  args;
            va_start(args, lpszFmt);
            _vsnprintf(szArgs, sizeof(szArgs), lpszFmt, args);
            va_end(args);
            m_pFnLog->LogFmtStrA(SPLOGLV_VERBOSE, "Enter [%s] %s", m_szFnName, szArgs);
        }
    }
    else
    {
        ZeroMemory((void *)&m_szFnName, sizeof(m_szFnName));
    }
}

CFnLog::CFnLog(ISpLog *pLog, LPCWSTR lpszFnName, LPCWSTR lpszFmt/* =NULL */, ...)
: m_pFnLog(pLog)
{
    if (NULL != m_pFnLog)
    {
        assert(NULL != lpszFnName);
        WideCharToMultiByte(CP_ACP, 0, lpszFnName, -1, m_szFnName, sizeof(m_szFnName), NULL, NULL);
        
        if ( NULL == lpszFmt )
        {
            m_pFnLog->LogFmtStrA(SPLOGLV_VERBOSE, "Enter [%s]", m_szFnName);
        }
        else
        {
            WCHAR    szwArgs[1024] = {0};
            va_list  args;
            va_start(args, lpszFmt);
            _vsnwprintf(szwArgs, sizeof(szwArgs), lpszFmt, args);
            va_end(args);
            
            CHAR szaArgs[2048] = {0};
            WideCharToMultiByte(CP_ACP, 0, szwArgs, -1, szaArgs, sizeof(szaArgs), NULL, NULL);
            m_pFnLog->LogFmtStrA(SPLOGLV_VERBOSE, "Enter [%s] %s", m_szFnName, szaArgs);
        }
    }
    else
    {
        ZeroMemory((void *)&m_szFnName, sizeof(m_szFnName));
    }
}

CFnLog::~CFnLog()
{
    if (NULL != m_pFnLog)
    {
        m_pFnLog->LogFmtStrA(SPLOGLV_VERBOSE, "Leave [%s]",  m_szFnName);   //lint !e1551
        m_pFnLog = NULL;
    }
}


