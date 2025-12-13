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
SET compilerFlags=-g -shared -Wvarargs -Wall
SET includeFlags=-Isrc -Ivendor
SET linkerFlags=-lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lglew32
SET defines=-DDEBUG_VERSION -DREN_EXPORT -D_CRT_SECURE_NO_WARNINGS 

ECHO %esc%[38;5;255mBuilding %assembly%%...
ECHO:

REM Compile the DLL and generate the import library
gcc %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll -Wl,--out-implib,../bin/%assembly%.lib %defines% %linkerFlags% %includeFlags%

CD ..\build_files

REM Report build result
IF %ERRORLEVEL% NEQ 0 (
    ECHO %esc%[38;5;1mBuild failed!%esc%[0m
) ELSE (
    ECHO %esc%[38;5;2mBuild succeeded!%esc%[0m
)