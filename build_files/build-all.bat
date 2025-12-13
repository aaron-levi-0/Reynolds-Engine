@ECHO OFF
TITLE Build Everything

REM Colour codes
@for /f %%a in ('echo prompt $E^| cmd') do set "esc=%%a" 

ECHO Building everything...

CALL build.bat
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

CD ../testbed
CALL build.bat
CD ../build_files

IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

IF %ERRORLEVEL% NEQ 0 (
    ECHO %esc%[38;5;1mOne or more builds failed.%esc%[0m
) ELSE (
    ECHO %esc%[38;5;2mAll assemblies built successfully.%esc%[0m
)