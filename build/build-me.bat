@setlocal

@set DOINSTALL=N
@set TMPTO=10
@set TMPRT=..
@set TMPPRJ=fc4
@set TMPSRC=%TMPRT%
@set TMPBGN=%TIME%
@REM Check persona install location
@set TMPINS=C:\MDOS
@set TMPCM=%TMPSRC%\CMakeLists.txt
@set DOPAUSE=choice /D %DOINSTALL% /T %TMPTO% /M "Pausing for %TMPTO% seconds. Def=%DOINSTALL%"

@call chkmsvc %TMPPRJ% 

@rem if EXIST build-cmake.bat (
@rem call build-cmake
@rem )

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
@echo *** Note install location %TMPINS% ***
@if NOT EXIST %TMPINS%\nul goto NOINS
@if NOT EXIST %TMPINS%\fc4.exe goto DNIWRN
@call dirmin %TMPINS%\fc4.exe
@echo *** WARNING *** An install may replace %TMPINS%\fc4.exe!
:DNIWRN
@echo.
@%DOPAUSE%
@echo.
@if ERRORLEVEL 2 goto GOTNO
@goto DOINST
:GOTNO
@echo Skipping install for now... set DOINSTALL=Y
@echo.
@goto END

:DOINST
@echo Continuing with release install...

@REM cmake --build . --config Debug  --target INSTALL >> %TMPLOG% 2>&1

@echo Doing: 'cmake --build . --config Release  --target INSTALL' >> %TMPLOG%
@echo Doing: 'cmake --build . --config Release  --target INSTALL'
cmake --build . --config Release  --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR5

@fa4 " --" %TMPLOG%

@call elapsed %TMPBGN%
@echo All done... see %TMPLOG%

@goto END

:NOINS
@echo The install location %TMPINS% does NOT exist!
@echo Either fix, change the location or create it first!
@goto ISERR

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
