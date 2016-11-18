# Microsoft Developer Studio Project File - Name="PlotCenter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PlotCenter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PlotCenter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PlotCenter.mak" CFG="PlotCenter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PlotCenter - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PlotCenter - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PlotCenter - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Z<none>
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CH375DLL.LIB /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "PlotCenter - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CH375DLL.LIB /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PlotCenter - Win32 Release"
# Name "PlotCenter - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\DPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\FileFixedPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Instruct.cpp
# End Source File
# Begin Source File

SOURCE=.\Line.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\PictureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotCenter.rc
# End Source File
# Begin Source File

SOURCE=.\PlotCenterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotContext.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PlotThread.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupPicture.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupPlotter.cpp
# End Source File
# Begin Source File

SOURCE=.\Shape.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VerifyCopies.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewHistory.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CH375DLL.H
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\DPoint.h
# End Source File
# Begin Source File

SOURCE=.\FileFixedPrint.h
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.h
# End Source File
# Begin Source File

SOURCE=.\Instruct.h
# End Source File
# Begin Source File

SOURCE=.\Line.h
# End Source File
# Begin Source File

SOURCE=.\NetworkSetup.h
# End Source File
# Begin Source File

SOURCE=.\PictureDlg.h
# End Source File
# Begin Source File

SOURCE=.\PlotCenter.h
# End Source File
# Begin Source File

SOURCE=.\PlotCenterDlg.h
# End Source File
# Begin Source File

SOURCE=.\PlotContext.h
# End Source File
# Begin Source File

SOURCE=.\PlotListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelectDialog.h
# End Source File
# Begin Source File

SOURCE=.\SensAPI.h
# End Source File
# Begin Source File

SOURCE=.\SetupPicture.h
# End Source File
# Begin Source File

SOURCE=.\SetupPlotter.h
# End Source File
# Begin Source File

SOURCE=.\Shape.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VerifyCopies.h
# End Source File
# Begin Source File

SOURCE=.\ViewHistory.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\down_gap.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ErrorX.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ErrorY.bmp
# End Source File
# Begin Source File

SOURCE=.\res\flagbitm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mark_gap.bmp
# End Source File
# Begin Source File

SOURCE=.\res\page_cx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\page_cy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlotCenter.ico
# End Source File
# Begin Source File

SOURCE=.\res\PlotCenter.rc2
# End Source File
# Begin Source File

SOURCE=.\PlotCenter.rgs
# End Source File
# Begin Source File

SOURCE=.\res\right_ga.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up_gap.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
