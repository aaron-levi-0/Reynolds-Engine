@ECHO OFF
Title Reynolds Engine Build 
SetLocal EnableDelayedExpansion
ECHO %esc%[38;5;3mBuild script for engine

REM Get a list of all the .c files.
CD ..\engine
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

REM Configuration
SET assembly=engine
SET compilerFlags=-g -Wvarargs -Wall
SET includeFlags=-Isrc -Ivendor
SET linkerFlags=-shared -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lglew32
SET defines=-DDEBUG_VERSION -DREN_EXPORT -D_CRT_SECURE_NO_WARNINGS 

ECHO %esc%[38;5;255mBuilding %assembly%%...
ECHO:

REM Link flags: make a DLL + link dependencies
SET linkFlags=-shared -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lglew32 ^
 -Wl,--out-implib,../bin/lib%assembly%.dll.a

ECHO %esc%[38;5;255mBuilding %assembly%...
ECHO:

REM Build engine.dll and generate libengine.dll.a
gcc %cFilenames% %compilerFlags% %defines% %includeFlags% %linkFlags% -o ../bin/%assembly%.dll

CD ..\build_files

REM Report build result
IF %ERRORLEVEL% NEQ 0 (
    ECHO %esc%[38;5;1mBuild failed.%esc%[0m
) ELSE (
    ECHO %esc%[38;5;2mBuild succeeded.%esc%[0m
)