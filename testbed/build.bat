@ECHO OFF
Title Sandbox Application Build
SetLocal EnableDelayedExpansion

echo %esc%[38;5;3mBuild script for testbed
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files
SET cFilenames=
FOR /R %%f IN (*.c) DO (
    SET cFilenames=!cFilenames! "%%f"
)

REM Get the absolute path of the batch file's directory
SET "batchFolder=%~dp0"
SET "workspaceFolder=%batchFolder%"
REM Convert backslashes to forward slashes for GCC compatibility
SET "workspaceFolder=%workspaceFolder:\=/%" 

REM ECHO %workspaceFolder%

REM Configuration
SET assembly=testbed
SET compilerFlags=-g -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lglew32
SET includeFlags=-Isrc -Ivendor -I..\engine\src -I..\engine\vendor
REM SET linkerFlags=-L../bin/ -.lengine.lib
SET defines=-DDEBUG_VERSION -DREN_IMPORT -DENABLE_RELATIVE_PATHS -DSRC_FOLDER=\"%workspaceFolder%/\"

ECHO %esc%[38;5;255mBuilding %assembly%...
ECHO:

gcc %cFilenames% %compilerFlags% -o ..\bin\%assembly%.exe %defines% %includeFlags% %linkerFlags% ..\bin\engine.lib

IF %ERRORLEVEL% NEQ 0 (
    ECHO %esc%[38;5;1mBuild failed!%esc%[0m
) ELSE (
    ECHO %esc%[38;5;2mBuild succeeded!%esc%[0m
)