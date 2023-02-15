#ifndef _IDCS_H_7A2441BC_83C2_4efa_9D05_F17F78353223_
#define _IDCS_H_7A2441BC_83C2_4efa_9D05_F17F78353223_

//////////////////////////////////////////////////////////////////////////
// IDCS: 描述程控电源接口 
//  
// 20090401 xiaoping.jing 接口整理。
// 20100120 Sunny add visa gpib driver  
//
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
#include "DeviceCommDef.h"
//////////////////////////////////////////////////////////////////////////
//lint ++flb

// 程控电源类型
typedef enum
{
    KEITHLEY_230X,
	KEITHLEY_2304,
    AGILENT_663XX,
	AGILENT_E36XX     

}DCS_TYPE;

typedef enum
{
    ATTR_DCS_DISPLAY,        // 电源面板显示开关
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
//  初始化和释放电源
//  nCardIndex:     GPIB设备卡索引
//  nGpibAddr:      电源GPIB地址
    virtual DWORD   InitDCSource  (int nCardIndex, int nGpibAddr) = 0;
    virtual DWORD   InitDCSourceEx(const DEV_PARAM *pParam) = 0;

    virtual DWORD   Release(void) = 0;

//////////////////////////////////////////////////////////////////////////
//  输出电压
//  fVoltage:       输出电压值 (单位: V)
//                  fVoltage < 0, 表示关闭输出
//  nChannel:       通道选择，某些电源只支持单路通道输出
//                  -1 表示使用配置文件的预设通道
    virtual DWORD   SetVoltageOutput(float  fVoltage, int nChannel = -1) = 0;
    
//////////////////////////////////////////////////////////////////////////
//  读出电压(单位: V)
    virtual DWORD   GetVoltageValue (float &fVoltage, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  读出电流(单位: A)
    virtual DWORD   GetCurrentValue (float &fCurrent, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  测量一段时间内的电流的平均值(单位: mA)
//  dwTimeSpan: 测量电源的时间段(单位: ms)
    virtual DWORD   GetAvgCurrentValue(DWORD dwTimeSpan, float &fCurrent, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  设置电流门限值(单位: A)
    virtual DWORD   SetCurrentRange(float fMaxCurrentRange, int nChannel = -1) = 0;

//////////////////////////////////////////////////////////////////////////
//  属性设置
    virtual DWORD   SetProperty(LONG lAttr, LONG lFlags, LPCVOID lpValue) = 0;
    virtual DWORD   GetProperty(LONG lAttr, LONG lFlags, LPVOID  lpValue) = 0; 
};

class IDCSFactory
{
public:
    IDCSFactory() { };
    virtual ~IDCSFactory() {};

    // 根据eDcsType创建一个DCSource对象
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

