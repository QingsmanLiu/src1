# Microsoft Developer Studio Project File - Name="2ndboot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=2ndboot - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "2ndboot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "2ndboot.mak" CFG="2ndboot - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "2ndboot - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "2ndboot - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "2ndboot - Win32 Release"

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
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "2ndboot - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../inc" /I "../inc/mcu" /I "../stm32lib" /I "../stm32lib/CMSIS/Device/Include" /I "../stm32lib/CMSIS/Include" /I "../stm32lib/STM32F0xx_StdPeriph_Driver/inc" /I "./src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "USE_STDPERIPH_DRIVER" /D "STM32F072" /D CODE_FOR_MCU_BOOT=1 /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "2ndboot - Win32 Release"
# Name "2ndboot - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\appl.h
# End Source File
# Begin Source File

SOURCE=.\src\download.h
# End Source File
# Begin Source File

SOURCE=.\src\drv_uart_poll.c
# End Source File
# Begin Source File

SOURCE=.\src\drv_uart_poll.h
# End Source File
# Begin Source File

SOURCE=.\src\main.c
# End Source File
# Begin Source File

SOURCE=.\src\stm32f0xx_conf.h
# End Source File
# Begin Source File

SOURCE=.\src\stm32f0xx_it.c
# End Source File
# Begin Source File

SOURCE=.\src\stm32f0xx_it.h
# End Source File
# Begin Source File

SOURCE=..\stm32lib\STM32F0xx_StdPeriph_Driver\src\stm32f0xx_iwdg.c
# End Source File
# Begin Source File

SOURCE=.\src\system_stm32f0xx.c
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\stm32lib\STM32F0xx_StdPeriph_Driver\src\stm32f0xx_flash.c
# End Source File
# Begin Source File

SOURCE=..\stm32lib\STM32F0xx_StdPeriph_Driver\src\stm32f0xx_gpio.c
# End Source File
# Begin Source File

SOURCE=..\stm32lib\STM32F0xx_StdPeriph_Driver\src\stm32f0xx_pwr.c
# End Source File
# Begin Source File

SOURCE=..\stm32lib\STM32F0xx_StdPeriph_Driver\src\stm32f0xx_rcc.c
# End Source File
# Begin Source File

SOURCE=..\stm32lib\STM32F0xx_StdPeriph_Driver\src\stm32f0xx_rtc.c
# End Source File
# Begin Source File

SOURCE=..\stm32lib\STM32F0xx_StdPeriph_Driver\src\stm32f0xx_usart.c
# End Source File
# End Group
# Begin Group "pc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pc\dummy.c
# End Source File
# End Group
# Begin Group "mcu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\mcu\src\drv_flash.c
# End Source File
# Begin Source File

SOURCE=..\mcu\src\drv_gpio.c
# End Source File
# Begin Source File

SOURCE=..\mcu\src\drv_rtc.c
# End Source File
# Begin Source File

SOURCE=..\mcu\src\drv_wdog.c
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\misc.c
# End Source File
# End Group
# End Target
# End Project
