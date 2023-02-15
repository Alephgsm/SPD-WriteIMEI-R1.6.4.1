#ifndef _PHASECHECK_EXPORT_H_BY_JXP_20091102_EA3C3E8A_2D1C_4EF7_9A4D_AF27799B3D0A_
#define _PHASECHECK_EXPORT_H_BY_JXP_20091102_EA3C3E8A_2D1C_4EF7_9A4D_AF27799B3D0A_


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PHASECHECKDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PHASECHECKDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.


#ifdef  PHASECHECKDLL_EXPORTS
    #ifdef __cplusplus
        #define SPPH_EXPORT extern "C" __declspec(dllexport)
    #else
        #define SPPH_EXPORT            __declspec(dllexport)
    #endif
#else
    #ifdef __cplusplus
        #define SPPH_EXPORT extern "C" __declspec(dllimport)
    #else
        #define SPPH_EXPORT            __declspec(dllimport)
    #endif
#endif

//////////////////////////////////////////////////////////////////////////
//
#define SPPH_API  __stdcall
//
typedef void *  SPPH_OBJECT;

//lint ++flb
typedef enum
{
    SPPH_VERSION  = 1


}SPPH_PROPERTY_OPTION;

//
typedef enum
{
    SPPH_AUTO = 0,
    SPPH_SP05,
    SPPH_SP09

}SPPH_VERSION_TYPE;


//
typedef enum
{
    SPPH_SUCC  = 0,                 // �����ɹ�
    SPPH_FAIL,                      // ����ʧ��
    SPPH_INVALID_SPPH_OBJECT,       // ��Ч�ľ��: SPPH_OBJECT hObject
    SPPH_INVALID_PRODUCTINFO,       // ��Ч��PhaseCheck�ṹ
    SPPH_INVALID_STATION_NAME,      // ��Ч�Ĳ���վλ����
    SPPH_STATION_NOT_EXIST,         // ����վ������
    SPPH_STATION_NOT_PASSED,        // ����վδPASS
    SPPH_STATION_NOT_TESTED,        // ����վδ����
    SPPH_CNST_PHASE_BUFFER_FAIL,    // ����PhaseCheck����ʧ��
    SPPH_WRITE_PRODUCTINFO_FAIL,    // д��FLASHʧ��
    SPPH_READ_PRODUCTINFO_FAIL,     // ��ȡFLASHʧ��
    SPPH_BUFFER_TOO_SMALL,          // ����Ļ���̫С
    SPPH_READ_MAGIC_NUMBER_FAIL,    // ��ȡPhaseCheck�汾ʧ��
    SPPH_INVALID_MAGIC_VERSION      // ��ЧPhaseCheck�汾

}SPPH_ERROR;

/* ========================================================================================================= 
-- Name:
--      CreatePhaseCheckObject
--
-- Function:
--      ����PhaseCheck�������󣬲����ز������
--
-- Parameter:
--  IN
--      hDiagPort: Handle of phone, returned by 'SP_CreateDiagPhone' of PhoneCommand.
--
-- Return:
--      SPPH_OBJECT: �ɹ����ز��������ʧ�ܷ���NULL.
--   
   ========================================================================================================= */

SPPH_EXPORT 
SPPH_OBJECT SPPH_API CreatePhaseCheckObject
(
    void *hDiagPort
);

/* ========================================================================================================= 
-- Name:
--      ReleasePhaseCheckObject
--
-- Function:
--      �ͷ�CreatePhaseCheckObject�����ľ��
--
-- Parameter:
--  IN
--      hObject:    �������
--
-- Return:
--      �����ɹ�����SPPH_SUCC���������ο�SPPH_ERROR����          
--   
   ========================================================================================================= */

SPPH_EXPORT 
DWORD SPPH_API ReleasePhaseCheckObject
(
    SPPH_OBJECT hObject
);

/* ========================================================================================================= 
-- Name:
--      InitPhase
--
-- Function:
--      ���������ļ������ã���ʼ���ֻ�FLASH��������Ϣ����
--
-- Parameter:
--  IN
--      hObject:        �������
--      sn1:            �ֻ����к�1����NULL���ʼ��Ϊ0. ��󳤶�24�ֽڡ�
--      sn2:            �ֻ����к�2����NULL���ʼ��Ϊ0. ��󳤶�24�ֽڡ�
--      pszStationName: ��վ�����ƣ�Ĭ��PASS
--  OUT
--      lpData:   ��ʼ����ϵ�������Ϣ��������ָ��,
--                ��lpDataΪ��ָ�룬�򲻷������ݡ�
--      nDataLen: lpData Buffer�ĳ���
--      pnRetLen: ʵ�ʷ��ص�lpData Buffer�ĳ���      
--
-- Return:
--      �����ɹ�����SPPH_SUCC���������ο�SPPH_ERROR����          
--          
   ========================================================================================================= */

SPPH_EXPORT 
DWORD SPPH_API InitPhase
(
     SPPH_OBJECT     hObject,        /*  IN */ 
     const char     *sn1,            /*  IN */ 
     const char     *sn2,            /*  IN */ 
     LPBYTE          lpData,         /* OUT */ 
     int             nDataLen,       /*  IN */
     int            *pnRetLen,       /* OUT */
     const char     *pszStationName  /*  IN */ 
 );

/* ========================================================================================================= 
-- Name:
--      CheckPhase
--
-- Function:
--      ������վ�Ĳ���״̬
--     
-- Parameter:
--  IN
--      hObject:                �������
--      pszCurrentStationName:  ��ǰ����վ������
--      pszCheckStationName:    ��Ҫ���Ĳ���վ������
                                pszCheckStationName = "NULL", ��ʾ��վ
--
-- Return:
--      �����ɹ�����SPPH_SUCC���������ο�SPPH_ERROR����          
--          
   ========================================================================================================= */

SPPH_EXPORT 
DWORD SPPH_API CheckPhase
(
    SPPH_OBJECT hObject,                /*  IN */ 
    const char *pszCurrentStationName,  /*  IN */ 
    const char *pszCheckStationName     /*  IN */ 
 );

/* ========================================================================================================= 
-- Name:
--      UpdatePhase
--
-- Function:
--      ���²���վλ��Ϣ  
--
-- Parameter:
--  IN
--      hObject:        �������
--      pszCurrentStationName: ��Ҫ���µĲ���վ������(ͨ�������Ϊ��ǰ����վ��)
--      nState:         ����״̬��1 ��ʾ PASS; 0 ��ʾ FAIL  
--      pszLastDescription:    ����״̬��������󲻳���32���ֽ�
--      sn1:            SN1, ��ָ��Ϊ���򲻸���SN1
--      sn2:            SN2, ��ָ��Ϊ���򲻸���SN2
--
-- Return:
--      �����ɹ�����SPPH_SUCC���������ο�SPPH_ERROR����        
--          
   ========================================================================================================= */

SPPH_EXPORT 
DWORD SPPH_API UpdatePhase
(
    SPPH_OBJECT hObject,                /*  IN */ 
    const char *pszCurrentStationName,  /*  IN */ 
    int         nState,                 /*  IN */ 
    const char *pszLastDescription,     /*  IN */   //Max characters is 32.
    const char *sn1,                    /*  IN */ 
    const char *sn2                     /*  IN */ 
);

/* ========================================================================================================= 
-- Name:
--      PutProperty
--
-- Function:
--      ��������
--
-- Parameter:
--  IN
--      hObject:    ������� 
--      lOption:    ����ѡ��
--      lProperty:  ����ֵ 
--
-- Return:
--      �����ɹ�����SPPH_SUCC���������ο�SPPH_ERROR����       
--          
   ========================================================================================================= */

SPPH_EXPORT 
DWORD SPPH_API PutProperty
(
    SPPH_OBJECT hObject,    /*  IN */ 
    LONG        lOption,    /*  IN */ 
    LONG        lProperty   /*  IN */ 
);

/* ========================================================================================================= 
-- Name:
--      GetProperty
--
-- Function:
--      ��ȡ��������
--
-- Parameter:
--  IN
--      hObject:    ������� 
--      lOption:    ����ѡ��
--  OUT
--      pProperty:  ����ֵ 
--
-- Return:
--      �����ɹ�����SPPH_SUCC���������ο�SPPH_ERROR����       
--          
   ========================================================================================================= */

SPPH_EXPORT 
DWORD SPPH_API GetProperty
(
    SPPH_OBJECT hObject,    /*  IN */ 
    LONG        lOption,    /*  IN */ 
    LONG       *pProperty   /* OUT */
);

/* ========================================================================================================= 
-- Name:
--      SPPH_GetLastError/SPPH_GetLastErrorMessage
--
-- Function:
--      ������һ�εĴ������ʹ���������Ϣ 
--
-- Parameter:
--  IN
--      hObject:    �������
--      pMsgBuff:   �洢����������Ϣ����
--      nBuffLen:   �����С
--      nLangOption:1, ����; 0, Ӣ��
--
-- Return:
--      SPPH_GetLastError �����ɹ��������һ�εĴ������
--              
--          
   ========================================================================================================= */
SPPH_EXPORT 
DWORD SPPH_API SPPH_GetLastError
(
    SPPH_OBJECT hObject     /*  IN */
);

SPPH_EXPORT
DWORD SPPH_API SPPH_GetLastErrorMessage
(
    SPPH_OBJECT hObject,    /*  IN */
    char       *pMsgBuff,   /* OUT */
    int         nBuffLen,   /*  IN */
    int         nLangOption /*  IN */
);
//lint --flb
#endif /* _PHASECHECK_EXPORT_H_BY_JXP_20091102_EA3C3E8A_2D1C_4EF7_9A4D_AF27799B3D0A_ */
