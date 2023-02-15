// This file declared all the export function of LogOn.dll
#ifndef __LOGON_EXPORT_H_CHENFENG_ZHU_2005_9_26__
#define __LOGON_EXPORT_H_CHENFENG_ZHU_2005_9_26__

#ifdef __cplusplus
#define LOGON_API extern "C" __declspec(dllexport)
#else
#define LOGON_API __declspec(dllexport)
#endif
//lint ++flb
typedef enum _USER_PRIVILEGE
{
	INVALID_USERS,     //非法用户
	ADMINS,            //管理员组
	USERS,             //受限用户组
	POWER_USERS       //高级用户组
}USER_PRIVILEGE;

#define MAX_USER_NAME_LENGTH  30
#define MAX_PASSWORD_LENGTH   30
/*****************************************************************************
**************               LOGON_LogOn                     *****************
******************************************************************************
Open the log on dialog.
  
Parameters:
   void
  	
Return Values:
   return the USER_PRIVILEGE of the current user.  
*****************************************************************************/
LOGON_API USER_PRIVILEGE LOGON_LogOn(void);


/*****************************************************************************
**************             LOGON_ManageAccounts              *****************
******************************************************************************
Open the account management dialog.
  
Parameters:
   void
  	
Return Values:
   void  
*****************************************************************************/
LOGON_API void LOGON_ManageAccounts(void);


/*****************************************************************************
**************             LOGON_GetCurrentAccount            *****************
******************************************************************************
Get the information of current user.
  
Parameters:
   [out]szUserName   Pointer to a buffer that receives the name of current user,
                     or NULL if you do not want to know the name.
  	
Return Values:
   return the USER_PRIVILEGE of the current user.
*****************************************************************************/
LOGON_API USER_PRIVILEGE LOGON_GetCurrentAccount(char* szUserName);


/*****************************************************************************
**************                 LOGON_Logoff                  *****************
******************************************************************************
log off
  
Parameters:
  bSetDefault:
  [in]: TRUE:if you want to log off and log on by default user.
        FALSE: if you just want to log off.
	 
Return Values:
   no return.
*****************************************************************************/
LOGON_API void LOGON_Logoff(BOOL bSetDefault);
//lint --flb
#endif
