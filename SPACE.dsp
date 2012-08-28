# Microsoft Developer Studio Project File - Name="SimpleSquare" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=SimpleSquare - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SPACE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SPACE.mak" CFG="SimpleSquare - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SimpleSquare - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "SimpleSquare - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SimpleSquare - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Release/yarrwars.exe"

!ELSEIF  "$(CFG)" == "SimpleSquare - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SimpleSquare - Win32 Release"
# Name "SimpleSquare - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "pseudolib No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\lib\BitmapTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lib\EasyFont.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spacecfg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spaceinc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spaceobj.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spaceoverlay.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spacerender3d.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spacesector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spaceworld3d.cpp
# End Source File
# Begin Source File

SOURCE=.\src\yarrAI.cpp
# End Source File
# Begin Source File

SOURCE=.\src\yarrAIPirate.cpp
# End Source File
# Begin Source File

SOURCE=.\src\yarrCraft.cpp
# End Source File
# Begin Source File

SOURCE=.\src\yarrRender.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\lib\BitmapTexture.h
# End Source File
# Begin Source File

SOURCE=.\src\lib\EasyFont.h
# End Source File
# Begin Source File

SOURCE=.\src\glut.h
# End Source File
# Begin Source File

SOURCE=.\src\lib\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\space.h
# End Source File
# Begin Source File

SOURCE=.\src\spacecfg.h
# End Source File
# Begin Source File

SOURCE=.\src\spaceinc.h
# End Source File
# Begin Source File

SOURCE=.\src\spaceobj.h
# End Source File
# Begin Source File

SOURCE=.\src\spaceoverlay.h
# End Source File
# Begin Source File

SOURCE=.\src\spaceoverview.h
# End Source File
# Begin Source File

SOURCE=.\src\spacerender3d.h
# End Source File
# Begin Source File

SOURCE=.\src\spacesector.h
# End Source File
# Begin Source File

SOURCE=.\src\spaceworld3d.h
# End Source File
# Begin Source File

SOURCE=.\src\yarrAI.h
# End Source File
# Begin Source File

SOURCE=.\src\yarrAIPirate.h
# End Source File
# Begin Source File

SOURCE=.\src\yarrCraft.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ai.cfg
# End Source File
# Begin Source File

SOURCE=.\src\lib\EasyText.rc
# End Source File
# Begin Source File

SOURCE=.\game.cfg
# End Source File
# Begin Source File

SOURCE=.\names.cfg
# End Source File
# Begin Source File

SOURCE=.\ship.cfg
# End Source File
# End Group
# End Target
# End Project
