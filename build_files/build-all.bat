@ECHO OFF
TITLE Build Everything

REM Colour codes
@for /f %%a in ('echo prompt $E^| cmd') do set "esc=%%a" 

ECHO Building everything...

CALL build.bat
IF %ERRORLEVEL% NEQ 0 GOTO ERROR0

CD ../testbed
CALL build.bat
IF %ERRORLEVEL% NEQ 0 GOTO ERROR1
CD ../build_files

ECHO %esc%[38;5;2mAll assemblies built successfully.%esc%[0m
EXIT /B 0

:ERROR0
ECHO %esc%[38;5;1mEngine build failed.%esc%[0m
EXIT /B 0

:ERROR1
CD ../build_files
ECHO %esc%[38;5;1mTestbed build failed.%esc%[0m
