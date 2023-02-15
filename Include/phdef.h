#ifndef _PHASE_CHECK_STRUCTURE_DEFINITION_H_CCC75ADF_01F4_4C32_B346_90708106C30A_
#define _PHASE_CHECK_STRUCTURE_DEFINITION_H_CCC75ADF_01F4_4C32_B346_90708106C30A_

// ------------------------------------------------------------------------------
//  File Name  : phdef.h
//  Description: phase check structure definition header file
//
//
// ------------------------------------------------------------------------------
//lint ++flb
typedef enum
{
    SP05,
    SP09

}SPPH_MAGIC;


#define MAX_SN_LEN              (24)


/* ***************************************************************************
                    
                      SP09 phase check structure

   *************************************************************************** */ 
#pragma pack(4)

//
#define SP09_MAX_SN_LEN 				    MAX_SN_LEN
#define SP09_MAX_STATION_NUM		   	    (15)
#define SP09_MAX_STATION_NAME_LEN		    (10)
#define SP09_SPPH_MAGIC_NUMBER              (0X53503039)    // "SP09"
#define SP09_MAX_LAST_DESCRIPTION_LEN       (32)

typedef enum
{
    PASS_1_FAIL_0  = 0x00,      // 1: pass, 0: fail   //lint !e769
    PASS_0_FAIL_1  = 0x01      // 1: fail, 0: pass

}SP09_SIGN_FLAG;

//
typedef struct _tagSP09_PHASE_CHECK
{
    unsigned long Magic;                // "SP09"   (老接口为SP05)
    char    SN1[SP09_MAX_SN_LEN];       // SN , SN_LEN=24
    char    SN2[SP09_MAX_SN_LEN];       // add for Mobile
    int     StationNum;                 // the test station number of the testing
    char    StationName[SP09_MAX_STATION_NUM][SP09_MAX_STATION_NAME_LEN];
    BYTE    Reserved[13];               // 固定为0
    BYTE    SignFlag;                   // internal flag 
    char    szLastFailDescription[SP09_MAX_LAST_DESCRIPTION_LEN];
    unsigned short  iTestSign;          // Bit0~Bit14 ---> station0~station 14  if tested. 0: tested, 1: not tested 
    unsigned short  iItem;              // part1: Bit0~ Bit_14 indicate test Station, 0: Pass, 1: fail    
                                        // part2: Bit15 标志位置成0 	
}SP09_PHASE_CHECK_T, *LPSP09_PHASE_CHECK_T;

//
const int SP09_MAX_PHASE_BUFF_SIZE = sizeof(SP09_PHASE_CHECK_T);



#pragma pack()


/* ***************************************************************************
                    
                      SP05 phase check structure

   *************************************************************************** */ 

//  Magic sign...
#define SP05_SPPH_MAGIC_NUMBER              (0X53503035)  // "SP05"
#define SP05_MAX_SUPPORT_STATION            (12)

//
typedef enum 
{
    SP05_TSN_LEN     = 16,
    SP05_TT_LEN      = 4,
    SP05_TS_LEN      = 16,
    SP05_FD_LEN      = 32,
    SP05_LTT_LEN     = 24,
    SP05_MAG_LEN     = 4,
    SP05_SN_LEN      = 24,
    SP05_VER_LEN     = 24,
    SP05_FAC_LEN     = 24,
    SP05_PL_LEN      = SP05_FAC_LEN,
    SP05_RES_LEN1    = 32,
    SP05_RES_LEN2    = MAX_SN_LEN

}SP05_PH_LEN;

// phase check header
typedef struct _tagSP05_PHASE_CHECK_HEADER
{
    unsigned long Magic;			        // "SP05"
    BYTE  BigEndian;		                // 1
    BYTE  StationNum;		                // the test station number of the testing
    BYTE  Reserved1[2];
    char  SN[SP05_SN_LEN];			        // SN   
    char  PcbVersion[SP05_VER_LEN];		    // PCB version
    char  HardwareVersion[SP05_VER_LEN];	// HW version
    char  SoftwareVersion[SP05_VER_LEN];	// SW version
    char  FactoryName[SP05_FAC_LEN];	    // Factory name of the manufactor
    char  ProductLine[SP05_PL_LEN];		    // Product line
    GUID  Guid;						        //add for database  //lint !e768
    char  SN2[SP05_SN_LEN];				    //add for Mobile SN jl 090608
    char  Reserve[SP05_RES_LEN2];		    // ""    //lint !e768

}SP05_PHASE_CHECK_HEADER, *LPSP05_PHASE_CHECK_HEADER;

// phase check item
typedef struct _tagSP05_PHASE_CHECK_ITEM
{
    char  TestStationName[SP05_TSN_LEN];	// "CFT" or "AT", etc
    char  TestTimes[SP05_TT_LEN];		    //lint !e768// "012"                
    char  FailureTimes[SP05_TT_LEN];		//lint !e768   // "001"               
    char  TestState[SP05_TS_LEN];		    // "PASS", "FAIL" or "DCS_TXP", etc.    //lint !e768
    char  FailDesciption[SP05_FD_LEN];	    // the description of failed reason   //lint !e768
    char  LastTestTime[SP05_LTT_LEN];       //lint !e768
    char  Reserve[SP05_RES_LEN1];		    //lint !e768

}SP05_PHASE_CHECK_ITEM, *LPSP05_PHASE_CHECK_ITEM;

// phase check structure
typedef struct _tagSP05_PHASE_CHECK_T
{
    SP05_PHASE_CHECK_HEADER header;
    SP05_PHASE_CHECK_ITEM   items[SP05_MAX_SUPPORT_STATION];    // Max. number 12

}SP05_PHASE_CHECK_T, *LPSP05_PHASE_CHECK_T;


const  int SP05_MAX_PHASE_BUFF_SIZE = sizeof(SP05_PHASE_CHECK_T);
//lint --flb
#endif  /* _PHASE_CHECK_STRUCTURE_DEFINITION_H_CCC75ADF_01F4_4C32_B346_90708106C30A_ */