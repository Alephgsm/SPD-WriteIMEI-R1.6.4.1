// DevHound.cpp: implementation of the CDevHound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DevHound.h"
#include <objbase.h>
#include <initguid.h>
#include <setupapi.h>

#pragma comment(lib, "setupapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PORT_CLASS_NAME		_T("Ports")
#define MODEM_CLASS_NAME	_T("Modem")

DEFINE_GUID(GUID_CLASS_PORT_2, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDevHound::CDevHound()
{
	m_hWnd = NULL;
	m_dwThreadID = 0;
	m_dwMsgID = 0;
	m_hHundThread  = NULL;
	m_dwHundThreadID = 0;
	m_hHundEvent = 	CreateEvent(NULL,TRUE,FALSE,NULL);
	m_bWork=TRUE;
}

CDevHound::~CDevHound()
{
	m_hWnd = NULL;
	m_dwThreadID = 0;
	m_dwMsgID=0;
	Stop();
	CloseHandle(m_hHundEvent);
	m_hHundEvent = NULL;
}

void CDevHound::SetReceiver(DWORD dwHandle, DWORD dwMsgID, BOOL bThread /*= TRUE*/)
{
	m_dwMsgID = dwMsgID;
	if(bThread)
	{
		m_dwThreadID  = dwHandle;
	}
	else
	{
		m_hWnd = (HWND)dwHandle;
	}

}
void CDevHound::Start()
{
	if(m_hHundThread != NULL)
		return;

	m_bWork = TRUE;
	m_hHundThread = CreateThread(NULL,0,GetHoundThreadProc,this,0,&m_dwHundThreadID);

}
void CDevHound::Stop()
{
	if(m_hHundThread == NULL)
		return;

	m_bWork = FALSE;
	SetEvent(m_hHundEvent);
	WaitForSingleObject(m_hHundThread,3000);
	CloseHandle(m_hHundThread);
	m_hHundThread = NULL;
}



DWORD WINAPI CDevHound::GetHoundThreadProc(LPVOID lpParam)
{
	CDevHound *pThis = (CDevHound*)lpParam;
	return pThis->HoundThreadProc();
}

DWORD WINAPI CDevHound::HoundThreadProc()
{
	GUID *guidDev = (GUID*)&GUID_CLASS_PORT_2;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

	// Get Port class set
	// Note:We use DIGCF_PRESENT flag,so maybe you can see
	// some ports on the device manager,but they are not 
	// enumerated by SetupDiEnumDeviceInterfaces in the do-while
	// loop,because their driver are disabled,no application 
	// can open and use them.

	hDevInfo = SetupDiGetClassDevs( guidDev,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
		);
	
	if(hDevInfo == INVALID_HANDLE_VALUE) 
	{
		return 0;
	}
	
	SP_DEVICE_INTERFACE_DATA ifcData;
	ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	
	DWORD dwIndex = 0;
	
	DEV_INFO epi;

	bool bInit = true;
	
	// Enumerate port and modem class device interfaces
	do
	{
		memset( &epi,0,sizeof(epi) );
		
		if( SetupDiEnumDeviceInterfaces(hDevInfo,NULL, guidDev, dwIndex, &ifcData) )
		{
			SP_DEVINFO_DATA devData;
			devData.cbSize = sizeof( SP_DEVINFO_DATA );
			
			if( !SetupDiGetDeviceInterfaceDetail( hDevInfo,&ifcData,NULL,0,NULL,&devData ) )
			{
				if( ERROR_INSUFFICIENT_BUFFER != GetLastError() )
				{
					// Can not get detail interface info
					continue;
				}
			}
			
			// Get Friendly name from registry
			SetupDiGetDeviceRegistryProperty( hDevInfo, &devData, SPDRP_FRIENDLYNAME, NULL,(PBYTE)epi.szFriendlyName, DEV_NAME_MAX_LEN*sizeof(TCHAR), NULL);
			// Get port description from registry
			SetupDiGetDeviceRegistryProperty( hDevInfo, &devData, SPDRP_DEVICEDESC, NULL,(PBYTE)epi.szDescription, DEV_NAME_MAX_LEN*sizeof(TCHAR), NULL);
			// Get class name from registry
			//TCHAR szClass[EPI_MAX_LEN];
			//SetupDiGetDeviceRegistryProperty( hDevInfo, &devData, SPDRP_CLASS, NULL,(PBYTE)szClass, EPI_MAX_LEN*sizeof(TCHAR), NULL);
			//epi.nType = CheckDeviceClass( szClass );
			
			HKEY hDevKey = SetupDiOpenDevRegKey( hDevInfo,&devData,DICS_FLAG_GLOBAL,0,DIREG_DEV,KEY_READ );
			if( INVALID_HANDLE_VALUE != hDevKey )
			{	
				// Get port name
				DWORD dwCount = DEV_NAME_MAX_LEN;
				RegQueryValueEx( hDevKey,_T( "PortName" ),NULL,NULL,(BYTE*)epi.szPortName,&dwCount );
				RegCloseKey( hDevKey );
			}
			
			// Insert to port info array
			InsertPortInfo( epi, bInit );

			dwIndex++;
		}
		else
		{
			dwIndex = 0;
			bInit = false;
			ClearState();
			Sleep(10);
			/*
			if( ERROR_NO_MORE_ITEMS == GetLastError() )
			{
				// No more devices, do the do-while loop gain
				dwIndex = 0;
				bInit = false;
				ClearState();
				Sleep(10);
			}	
			*/

			SetupDiDestroyDeviceInfoList(hDevInfo);
			hDevInfo = INVALID_HANDLE_VALUE;
			hDevInfo = SetupDiGetClassDevs( guidDev,
				NULL,
				NULL,
				DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
			);

			memset(&ifcData,0,sizeof(SP_DEVICE_INTERFACE_DATA));
			ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		}
		
	}while( m_bWork && (hDevInfo != INVALID_HANDLE_VALUE) );

	if(hDevInfo != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(hDevInfo);
		hDevInfo = INVALID_HANDLE_VALUE;
	}
	
	return 0;
}




bool CDevHound::CheckPortInfo( DEV_INFO& epi)
{
	int nFriendlyName = _tcslen( epi.szFriendlyName ) * sizeof( TCHAR );
	int nPortName = _tcslen( epi.szPortName ) * sizeof( TCHAR );
/*
	Log(_T("InsertPortInfo: [PortName]: %s [FriendlyName]: %s [PortType]: %s"),
		nPortName>0 ? epi.szPortName:_T(" "),
		nFriendlyName>0 ? epi.szFriendlyName:_T(" "),
		(EPIT_Port == epi.nType) ? _T("Port"): _T("Modem") );
*/
	if( nPortName )
	{
		// Get port number from port name
		_stscanf( epi.szPortName,_T("COM%d"),&epi.nPortNum );
		if( 0 == epi.nPortNum )
		{
			// Not a valid port name
			nPortName = 0;
		}
	}
	

	if( !nPortName && nFriendlyName && DT_PORT == epi.nType )
	{
		// Try to get port number from friendly name
		_stscanf( epi.szFriendlyName,_T("(COM%d)"),&epi.nPortNum );
		if( 0 == epi.nPortNum )
		{
			//Log(_T("InsertPortInfo: port num is zero."));
			return false;
		}
	}

	if( 0 == epi.nPortNum )
	{
		// Can not get port number
		//Log(_T("InsertPortInfo: port num is zero."));
		return false;
	}

	if( !nPortName )
	{
		// compose a valid port name
		nPortName = _stprintf( epi.szPortName,_T( "COM%d" ),epi.nPortNum ) * sizeof(TCHAR);
	}

	if( !nFriendlyName )
	{
		// compose a void friendly name
		nFriendlyName = _stprintf( epi.szFriendlyName,_T( "COM%d" ),epi.nPortNum ) * sizeof(TCHAR);
	}
	else
	{
		if( DT_MODEM == epi.nType )
		{
			// Because the friendly name of the modem has no com port information,
			// so we append it to the end of the friendly name,if the name is too
			// long,it will be changed to "part_of_friendly_name...(COMX)"
			bool bCat = true;
			if( nPortName + nFriendlyName + 2 * sizeof( TCHAR ) > DEV_NAME_MAX_LEN - sizeof( TCHAR ) )
			{
				nFriendlyName = DEV_NAME_MAX_LEN - sizeof( TCHAR ) - nPortName - 5 * sizeof( TCHAR );
				if( nFriendlyName > 0 )
				{
					epi.szFriendlyName[ nFriendlyName ] = _T( '\0' );
					_tcscat( epi.szFriendlyName,_T("...") );
				}
				else
				{
					bCat = false;
				}
			}

			if( bCat )
			{
				_tcscat( epi.szFriendlyName,_T( "(" ) );
				_tcscat( epi.szFriendlyName,epi.szPortName );
				_tcscat( epi.szFriendlyName,_T( ")" ) );
			}
		}
	}



	return true;
}

DEV_TYPE CDevHound::CheckDeviceClass( TCHAR* lpszClass )
{
	if( !_tcscmp( lpszClass,PORT_CLASS_NAME ) )
	{
		return DT_PORT;
	}
	else if( !_tcscmp( lpszClass,MODEM_CLASS_NAME ) )
	{
		return DT_MODEM;
	}
	else
	{
		return DT_UNKNOWN;
	}
}


bool CDevHound::InsertPortInfo( DEV_INFO& epi,bool bInit)
{
	if(!CheckPortInfo(epi) || epi.nPortNum == 0)
		return false;
	
	m_mPortInfo[epi.nPortNum] = epi;
	int nCount = m_vPortInfo.size();
	int i = 0;
	for(i = 0; i<nCount; i++)
	{
		if(m_vPortInfo[i].nPortNum == epi.nPortNum)
		{
			//Log(_T("Port [COM%d] is already exist."),epi.nPortNum);
			m_vPortInfo[i].bExist = TRUE;
			return false;
		}
	}	

	epi.bExist = TRUE;
	m_vPortInfo.push_back( epi );	

	if(!bInit)
	{
		SendToUplayer(epi);
	}
	
	return true;	
}

void CDevHound::SendToUplayer(DEV_INFO& epi)
{
	
	int nTry = 3;
	if(m_hWnd != NULL)
	{
		do 
		{
			if(::PostMessage(m_hWnd,m_dwMsgID,epi.nPortNum,epi.bExist))
			{
				break;
			}
			
		} while((--nTry)>0);
	}
	else
	{
		do 
		{
			if(::PostThreadMessage(m_dwThreadID,m_dwMsgID,epi.nPortNum,epi.bExist))
			{
				break;
			}
			
		} while((--nTry)>0);
	}

}

void  CDevHound::ClearState()
{
	int nCount = m_vPortInfo.size();
	int i = 0;
	for(i = 0; i<nCount; i++)
	{
		if(m_vPortInfo[i].bExist)
		{
			//Log(_T("Port [COM%d] is already exist."),epi.nPortNum);
			m_vPortInfo[i].bExist = FALSE;		
		}
		else
		{
			SendToUplayer(m_vPortInfo[i]);
			m_vPortInfo.erase(m_vPortInfo.begin()+i);
			nCount--;
			i--;
		}
	}
}

bool CDevHound::GetPortName(unsigned int nPort,TCHAR szName[DEV_NAME_MAX_LEN])
{
	map<unsigned int,DEV_INFO>::iterator it = m_mPortInfo.find(nPort);
	if(it != NULL)
	{
		DEV_INFO &di = (*it).second;

		if(_tcslen(di.szDescription)>0)
		{
			_tcscpy(szName,di.szDescription);
		}
		else
		{
			_tcscpy(szName,di.szFriendlyName);
		}

		return true;
	}
	return false;
}
