@setlocal

@set DOINSTALL=0

@set TMPRT=..
@set TMPPRJ=fc4
@set TMPSRC=%TMPRT%
@set TMPBGN=%TIME%
@set TMPINS=%TMPRT%\software
@set TMPCM=%TMPSRC%\CMakeLists.txt
@set DOPAUSE=pause

@call chkmsvc %TMPPRJ% 

@if EXIST build-cmake.bat (
@call build-cmake
)

@REM call setupqt5
@REM call setupqt32

@if NOT EXIST %TMPCM% goto NOCM

@set TMPLOG=bldlog-1.txt
@set TMPOPTS=-DCMAKE_INSTALL_PREFIX=%TMPINS%

:RPT
@if "%~1x" == "x" goto GOTCMD
@if "%~1x" == "NOPAUSEx" (
@set DOPAUSE=echo No pause requested...
) else (
@set TMPOPTS=%TMPOPTS% %1
)
@shift
@goto RPT
:GOTCMD

@echo Build %DATE% %TIME% > %TMPLOG%
@echo Build source %TMPSRC%... all output to build log %TMPLOG%
@echo Build source %TMPSRC%... all output to build log %TMPLOG% >> %TMPLOG%

@echo Doing: 'cmake %TMPSRC% %TMPOPTS%' >> %TMPLOG%
@echo Doing: 'cmake %TMPSRC% %TMPOPTS%'
@cmake %TMPSRC% %TMPOPTS% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

@echo Doing: 'cmake --build . --config Debug' >> %TMPLOG% 2>&1
@echo Doing: 'cmake --build . --config Debug'
@cmake --build . --config Debug >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2

@echo Doing: 'cmake --build . --config Release' >> %TMPLOG% 2>&1
@echo Doing: 'cmake --build . --config Release'
@cmake --build . --config Release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3
:DONEREL

@fa4 "***" %TMPLOG%
@call elapsed %TMPBGN%
@echo Appears a successful build... see %TMPLOG%
@echo.
@if "%DOINSTALL%x" == "1x" goto DOINST
@echo Skipping install for now... set DOINSTALL=1
@echo.
@goto END

:DOINST
@echo Continue with install? Only Ctrl+c aborts...
@DOPAUSE

cmake --build . --config Debug  --target INSTALL >> %TMPLOG% 2>&1
@if EXIST install_manifest.txt (
@if ERRORLEVEL 1 goto ERR4

cmake --build . --config Release  --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR5

@fa4 " --" %TMPLOG%

@call elapsed %TMPBGN%
@echo All done... see %TMPLOG%

@goto END

:NOCM
@echo Error: Can NOT locate %TMPCM%
@goto ISERR

:ERR1
@echo cmake configuration or generations ERROR
@goto ISERR

:ERR2
@echo ERROR: Cmake build Debug FAILED!
@goto ISERR

:ERR3
@fa4 "mt.exe : general error c101008d:" %TMPLOG% >nul
@if ERRORLEVEL 1 goto ERR33
:ERR34
@echo ERROR: Cmake build Release FAILED!
@goto ISERR
:ERR33
@echo Try again due to this STUPID STUPID STUPID error
@echo Try again due to this STUPID STUPID STUPID error >>%TMPLOG%
cmake --build . --config Release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR34
@goto DONEREL

:ERR4
@echo ERROR: Cmake install Debug FAILED!
@goto ISERR

:ERR5
@echo ERROR: Cmake install Release FAILED!
@goto ISERR

:ISERR
@echo See %TMPLOG% for details...
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
