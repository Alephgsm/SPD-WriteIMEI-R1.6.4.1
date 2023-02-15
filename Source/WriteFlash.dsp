# Microsoft Developer Studio Project File - Name="WriteFlash" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WriteFlash - Win32 UNICODE Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WriteFlash.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WriteFlash.mak" CFG="WriteFlash - Win32 UNICODE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WriteFlash - Win32 UNICODE Debug" (based on "Win32 (x86) Application")
!MESSAGE "WriteFlash - Win32 UNICODE Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "WriteFlash"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WriteFlash - Win32 UNICODE Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WriteFlash___Win32_UNICODE_Debug"
# PROP BASE Intermediate_Dir "WriteFlash___Win32_UNICODE_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Common/Bin"
# PROP Intermediate_Dir "Win32_UNICODE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "../../Common/Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp4 /MTd /W4 /Gm /GX /ZI /Od /I "../Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 PhoneCommandD.lib DCSourceD.lib PhaseCheckD.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../../Common/Bin/WriteIMEI/WriteIMEI.exe" /pdbtype:sept /libpath:"../../Common/Bin"
# ADD LINK32 PhoneCommandD.lib DCSourceD.lib PhaseCheckD.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../Bin/WriteIMEID.exe" /pdbtype:sept /libpath:"../Lib"

!ELSEIF  "$(CFG)" == "WriteFlash - Win32 UNICODE Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WriteFlash___Win32_UNICODE_Release"
# PROP BASE Intermediate_Dir "WriteFlash___Win32_UNICODE_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Common/Bin"
# PROP Intermediate_Dir "Win32_UNICODE_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MT /W4 /GX /Zi /Od /I "../../Common/Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W4 /GX /Zi /Od /I "../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 PhoneCommand.lib PhaseCheck.lib DCSource.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../../Common/Bin/WriteIMEI.exe" /libpath:"../../Common/Bin"
# ADD LINK32 PhoneCommand.lib PhaseCheck.lib DCSource.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../Bin/WriteIMEI.exe" /libpath:"../Lib"

!ENDIF 

# Begin Target

# Name "WriteFlash - Win32 UNICODE Debug"
# Name "WriteFlash - Win32 UNICODE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ColorStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\DevHound.cpp
# End Source File
# Begin Source File

SOURCE=.\IMEISetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpLog.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WriteFlash.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteFlash.rc
# End Source File
# Begin Source File

SOURCE=.\WriteFlashDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XAboutDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ColorStatic.h
# End Source File
# Begin Source File

SOURCE=.\DevHound.h
# End Source File
# Begin Source File

SOURCE=.\IMEISetDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModeSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\phdef.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WriteFlash.h
# End Source File
# Begin Source File

SOURCE=.\WriteFlashDlg.h
# End Source File
# Begin Source File

SOURCE=.\XAboutDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\WriteFlash.ico
# End Source File
# Begin Source File

SOURCE=.\res\WriteFlash.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
