#ifndef _IDCS_H_7A2441BC_83C2_4efa_9D05_F17F78353223_
#define _IDCS_H_7A2441BC_83C2_4efa_9D05_F17F78353223_

//////////////////////////////////////////////////////////////////////////
// IDCS: �����̿ص�Դ�ӿ� 
//  
// 20090401 xiaoping.jing �ӿ�������
// 20100120 Sunny add visa gpib driver  
//
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
#include "DeviceCommDef.h"
//////////////////////////////////////////////////////////////////////////
//lint ++flb

// �̿ص�Դ����
typedef enum
{
    KEITHLEY_230X,
	KEITHLEY_2304,
    AGILENT_663XX,
	AGILENT_E36XX     

}DCS_TYPE;

typedef enum
{
    ATTR_DCS_DISPLAY,        // ��Դ�����ʾ����
    ATTR_DCS_GPIB_LOG

}DCS_PROPERTY;


//
typedef enum
{
    E_DCS_SUCC = 0,
    E_DCS_INVALID_PARAM,
    E_DCS_CREATE_GPIB_DRIVER_FAILED,
    E_DCS_SEND_GPIB_FAILED,
    E_DCS_RECV_GPIB_FAILED,
    E_DCS_OPEN_DEVICE_FAILED,
    E_DCS_OPEN_METER_DEVICE_FAILED

}DCS_ERROR;



//////////////////////////////////////////////////////////////////////////
//
class IDCS
{
public:
    IDCS() { };
    virtual ~IDCS() { };
//////////////////////////////////////////////////////////////////////////
//  ��ʼ�����ͷŵ�Դ
//  nCardIndex:     GPIB�豸������
//  nGpibAddr:      ��ԴGPIB��ַ
    virtual DWORD   InitDCSource  (int nCardIndex, int nGpibAddr) = 0;
    virtual DWORD   InitDCSourceEx(const DEV_PARAM *pParam) = 0;

    virtual DWORD   Release(void) = 0;

//////////////////////////////////////////////////////////////////////////
//  �����ѹ
//  fVoltage:       �����ѹֵ (��λ: V)
//                  fVoltage < 0, ��ʾ�ر����
//  nChannel:       ͨ��ѡ��ĳЩ��Դֻ֧�ֵ�·ͨ�����
//                  -1 ��ʾʹ�������ļ���Ԥ��ͨ��
    virtual DWORD   SetVoltageOutput(float  fVoltage, int nChannel = -1) = 0;
    
//////////////////////////////////////////////////////////////////////////
//  ������ѹ(��λ: V)
    virtual DWORD   GetVoltageValue (float &fVoltage, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  ��������(��λ: A)
    virtual DWORD   GetCurrentValue (float &fCurrent, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  ����һ��ʱ���ڵĵ�����ƽ��ֵ(��λ: mA)
//  dwTimeSpan: ������Դ��ʱ���(��λ: ms)
    virtual DWORD   GetAvgCurrentValue(DWORD dwTimeSpan, float &fCurrent, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  ���õ�������ֵ(��λ: A)
    virtual DWORD   SetCurrentRange(float fMaxCurrentRange, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  ��������
    virtual DWORD   SetProperty(LONG lAttr, LONG lFlags, LPCVOID lpValue) = 0;
    virtual DWORD   GetProperty(LONG lAttr, LONG lFlags, LPVOID  lpValue) = 0; 
};

class IDCSFactory
{
public:
    IDCSFactory() { };
    virtual ~IDCSFactory() {};

    // ����eDcsType����һ��DCSource����
    virtual BOOL CreateDCSource(IDCS **lppObject, DCS_TYPE eDcsType) = 0;
    virtual void Release() = 0;   
};


//////////////////////////////////////////////////////////////////////////
//
#ifdef IDCSOURCE_EXPORTS
#define IDCS_API extern "C" __declspec(dllexport)
#else
#define IDCS_API extern "C" __declspec(dllimport)
#endif

IDCS_API BOOL CreateDCSourceFactory(IDCSFactory **lppObject);

//lint --flb

#endif /* _IDCS_H_7A2441BC_83C2_4efa_9D05_F17F78353223_ */
