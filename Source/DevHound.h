// DevHound.h: interface for the CDevHound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVHOUND_H__1D49D646_4061_40D2_B81C_DCF22E1F7BAC__INCLUDED_)
#define AFX_DEVHOUND_H__1D49D646_4061_40D2_B81C_DCF22E1F7BAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>

#pragma warning( push, 3 )
#include <vector>
#include <map>
#pragma warning( pop )

using namespace std;

// Max string length used by the class
#define DEV_NAME_MAX_LEN 256

enum DEV_TYPE
{
	DT_UNKNOWN,	    // Unknown type
	DT_PORT,		// Serial port
	DT_MODEM,		// Modem
	DT_MAX
};

struct DEV_INFO
{
	unsigned int nPortNum;				    // Port number
	TCHAR szPortName[DEV_NAME_MAX_LEN];		// Short name for the port,in format like "COM X"
	TCHAR szFriendlyName[DEV_NAME_MAX_LEN];	// Friendly name for the port device
	TCHAR szDescription[DEV_NAME_MAX_LEN];	// Description of the port
	DEV_TYPE nType;				            // Port type
	unsigned int  bExist;
};

class CDevHound  
{
public:
	CDevHound();
	virtual ~CDevHound();
    void SetReceiver(DWORD dwHandle, DWORD dwMsgID, BOOL bThread = TRUE);
	void Start();
	void Stop();
	bool GetPortName(unsigned int nPort,TCHAR szName[DEV_NAME_MAX_LEN]);
protected:
	static DWORD WINAPI GetHoundThreadProc(LPVOID lpParam);
	DWORD WINAPI HoundThreadProc();	
	bool  CheckPortInfo( DEV_INFO& epi);
	bool  InsertPortInfo( DEV_INFO& epi,bool bInit);	
	DEV_TYPE CheckDeviceClass( TCHAR* lpszClass );
	void  SendToUplayer(DEV_INFO& epi);
	void  ClearState();

private:
	vector<DEV_INFO> m_vPortInfo;
	map<unsigned int,DEV_INFO> m_mPortInfo;
	HWND   m_hWnd;
	DWORD  m_dwThreadID;
	DWORD  m_dwMsgID;
	
	HANDLE m_hHundThread;
	DWORD  m_dwHundThreadID;
	HANDLE m_hHundEvent;
	BOOL   m_bWork;
};

#endif // !defined(AFX_DEVHOUND_H__1D49D646_4061_40D2_B81C_DCF22E1F7BAC__INCLUDED_)
