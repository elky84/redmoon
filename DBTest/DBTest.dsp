# Microsoft Developer Studio Project File - Name="DBTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DBTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DBTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DBTest.mak" CFG="DBTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DBTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DBTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DBTest - Win32 UnicodeDebug" (based on "Win32 (x86) Application")
!MESSAGE "DBTest - Win32 UnicodeRelease" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DBTest - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP BASE Intermediate_Dir "$(ConfigurationName)"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP Intermediate_Dir "$(ConfigurationName)"
# PROP Target_Dir ""
# ADD BASE CPP /MTd /I "..\Include" /ZI /W3 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_MBCS" /Gm /YX /GZ /c 
# ADD CPP /MTd /I "..\Include" /ZI /W3 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_MBCS" /Gm /YX /GZ /c 
# ADD BASE MTL /D "_DEBUG" /win32 
# ADD MTL /D "_DEBUG" /win32 
# ADD BASE RSC /l 1042 /d "_DEBUG" /i "$(IntDir)" 
# ADD RSC /l 1042 /d "_DEBUG" /i "$(IntDir)" 
BSC32=bscmake.exe
# ADD BASE BSC32 
# ADD BSC32 
LINK32=link.exe
# ADD BASE LINK32 EasyLibD.Lib /incremental:yes /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /machine:ix86 
# ADD LINK32 EasyLibD.Lib /incremental:yes /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /machine:ix86 

!ELSEIF  "$(CFG)" == "DBTest - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP BASE Intermediate_Dir "$(ConfigurationName)"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP Intermediate_Dir "$(ConfigurationName)"
# PROP Target_Dir ""
# ADD BASE CPP /MT /I "..\Include" /Zi /W3 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_MBCS" /YX /c 
# ADD CPP /MT /I "..\Include" /Zi /W3 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_MBCS" /YX /c 
# ADD BASE MTL /D "NDEBUG" /win32 
# ADD MTL /D "NDEBUG" /win32 
# ADD BASE RSC /l 1042 /d "NDEBUG" /i "$(IntDir)" 
# ADD RSC /l 1042 /d "NDEBUG" /i "$(IntDir)" 
BSC32=bscmake.exe
# ADD BASE BSC32 
# ADD BSC32 
LINK32=link.exe
# ADD BASE LINK32 EasyLib.Lib /incremental:no /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /machine:ix86 
# ADD LINK32 EasyLib.Lib /incremental:no /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /machine:ix86 

!ELSEIF  "$(CFG)" == "DBTest - Win32 UnicodeDebug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP BASE Intermediate_Dir "$(ConfigurationName)"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP Intermediate_Dir "$(ConfigurationName)"
# PROP Target_Dir ""
# ADD BASE CPP /MTd /I "..\Include" /ZI /W3 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /Gm /YX /GZ /c 
# ADD CPP /MTd /I "..\Include" /ZI /W3 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /Gm /YX /GZ /c 
# ADD BASE MTL /D "_DEBUG" /win32 
# ADD MTL /D "_DEBUG" /win32 
# ADD BASE RSC /l 1042 /d "_DEBUG" /i "$(IntDir)" 
# ADD RSC /l 1042 /d "_DEBUG" /i "$(IntDir)" 
BSC32=bscmake.exe
# ADD BASE BSC32 
# ADD BSC32 
LINK32=link.exe
# ADD BASE LINK32 EasyLibUD.Lib /incremental:yes /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /machine:ix86 
# ADD LINK32 EasyLibUD.Lib /incremental:yes /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /machine:ix86 

!ELSEIF  "$(CFG)" == "DBTest - Win32 UnicodeRelease"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP BASE Intermediate_Dir "$(ConfigurationName)"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(SolutionDir)$(ConfigurationName)"
# PROP Intermediate_Dir "$(ConfigurationName)"
# PROP Target_Dir ""
# ADD BASE CPP /MT /I "..\Include" /Zi /W3 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /YX /c 
# ADD CPP /MT /I "..\Include" /Zi /W3 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_UNICODE" /YX /c 
# ADD BASE MTL /D "NDEBUG" /win32 
# ADD MTL /D "NDEBUG" /win32 
# ADD BASE RSC /l 1042 /d "NDEBUG" /i "$(IntDir)" 
# ADD RSC /l 1042 /d "NDEBUG" /i "$(IntDir)" 
BSC32=bscmake.exe
# ADD BASE BSC32 
# ADD BSC32 
LINK32=link.exe
# ADD BASE LINK32 EasyLibU.Lib /incremental:no /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /machine:ix86 
# ADD LINK32 EasyLibU.Lib /incremental:no /libpath:"..\Lib" /debug /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /machine:ix86 

!ENDIF

# Begin Target

# Name "DBTest - Win32 Debug"
# Name "DBTest - Win32 Release"
# Name "DBTest - Win32 UnicodeDebug"
# Name "DBTest - Win32 UnicodeRelease"
# Begin Group "소스 "

# PROP Default_Filter "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\EDTest.cpp
# End Source File
# Begin Source File

SOURCE=.\EDTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "DBTest - Win32 Debug"

# ADD CPP /Yc"" /GZ 
!ELSEIF  "$(CFG)" == "DBTest - Win32 Release"

# ADD CPP /Yc"" 
!ELSEIF  "$(CFG)" == "DBTest - Win32 UnicodeDebug"

# ADD CPP /Yc"" /GZ 
!ELSEIF  "$(CFG)" == "DBTest - Win32 UnicodeRelease"

# ADD CPP /Yc"" 
!ENDIF

# End Source File
# End Group
# Begin Group "헤더 "

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\Common.h
# End Source File
# Begin Source File

SOURCE=.\EDTest.h
# End Source File
# Begin Source File

SOURCE=.\EDTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SPList.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "리소스"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav"
# Begin Source File

SOURCE=.\res\EDTest.ico
# End Source File
# Begin Source File

SOURCE=.\EDTest.rc
# End Source File
# Begin Source File

SOURCE=.\res\EDTest.rc2
# End Source File
# End Group
# End Target
# End Project

