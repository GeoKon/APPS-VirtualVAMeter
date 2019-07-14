@echo off
SET lib0=D:\Dropbox\Arduino
SET lib1=%lib0%\libraries\GKESP-L1\src
SET lib2=%lib0%\libtemp\src

:redo
echo ----------------------------------------------------------
echo "	C:	copy files from LIBRARIES to LIBTEMP
echo "	R:	rename LIBRARIES to LIBRARIES-ORG and LIBTEMP to LIBRARIES
echo "	F:	flip back to original LIBRARIES
echo "	X:	Exit"
echo ----------------------------------------------------------

REM       1234
choice /C:CRFX /m "Select: "
echo ----------------------------------------------------------

if ERRORLEVEL 4 goto CMDEXIT
if ERRORLEVEL 3 goto CMDFLIP
if ERRORLEVEL 2 goto CMDRENAME
if ERRORLEVEL 1 goto CMDCOPY
echo You pressed ESC
goto redo


REM ******************************** RENAME **********************************
:CMDRENAME
@echo on
ren %lib0%\libraries  libraries-ORG
ren %lib0%\libtemp    libraries
dir %lib0%\lib*.* 
@echo off
goto redo

REM ********************************** FLIP **********************************
:CMDFLIP
ren %lib0%\libraries      libXXX
ren %lib0%\libraries-ORG  libraries
dir %lib0%\lib*.* 
goto redo

REM ********************************** COPY **********************************
:CMDCOPY

mkdir %lib0%\libtemp
del   %lib0%\libtemp\*.* /s /q
mkdir %lib2%
mkdir %lib2%\fonts

REM ************** Copy /OneWire and /MegunoLink
SET ext=OneWire
mkdir %lib0%\libtemp\%ext%
xcopy %lib0%\libraries\%ext%\*.* %lib0%\libtemp\%ext%\*.* /s

SET ext=MegunoLink
mkdir %lib0%\libtemp\%ext%
xcopy %lib0%\libraries\%ext%\*.* %lib0%\libtemp\%ext%\*.* /s

REM ************** Copy MACROS
SET file=macros
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=gkeL1io
copy %lib1%\%file%.h   %lib2%\%file%.h

REM ************** Copy FILES
SET file=bufClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=cpuClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=cliClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=eepClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=ticClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=ads15Class
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=oledClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h
xcopy %lib1%\fonts\*.* %lib2%\fonts\*.*

SET file=mgnClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h

SET file=tcpClass
copy %lib1%\%file%.cpp %lib2%\%file%.cpp
copy %lib1%\%file%.h   %lib2%\%file%.h
goto redo


REM ******************************** EXIT **********************************
:CMDEXIT

