#ifndef _DEVICE_COMM_DEF_H_CC0718A5_26ED_420A_9066_DCC78DF55F6A_BY_XP_
#define _DEVICE_COMM_DEF_H_CC0718A5_26ED_420A_9066_DCC78DF55F6A_BY_XP_

//lint ++flb

typedef enum 
{
    UNKNOWN_GPIB = 0,
    NI,
    AGILENT,
    VISA,
    LAN,
    MAX_GPIB_TYPE
}GPIB_TYPE_ENUM;

#define MAX_DEVICE_NAME_LEN             ( 50 )

typedef struct _tagDEV_PARAM
{
    char szDevName[MAX_DEVICE_NAME_LEN];

    GPIB_TYPE_ENUM eGpibType;
    BOOL bEnableLog;
    union
    {
        struct {
            int nGpibCardNo;
            int nGpibAddr;
            int nSecoAddr;
        }Card;
        
        struct {
            DWORD dwIP;
        }Lan;

    }gpib;
    
    LPVOID pParam;

}DEV_PARAM, *LPDEV_PARAM;


typedef enum
{
    E_GPIB_SUCC,                    // 
    E_GPIB_FAIL,                    // 
    E_GPIB_SEND_CMD_FAILED,         // Send gpib command failed
    E_GPIB_RECV_CMD_FAILED,         // Cannot receive gpib command
    E_GPIB_OPEN_DEVICE_FAILED,      // Cannot open device 
    E_GPIB_RECV_IDN_FAILED,         // Cannot receive IDN string
    E_GPIB_OPEN_LOG_FAILED,         // Open gpib.txt log file failed
    E_GPIB_INVALID_PARAMETER,       // Invalid parameter such as null pointer... 
    E_GPIB_INVALID_VISADRIV         // Invalid visa driver
        
}GPIB_ERROR;

//lint --flb

#endif /* _DEVICE_COMM_DEF_H_CC0718A5_26ED_420A_9066_DCC78DF55F6A_BY_XP_ */
