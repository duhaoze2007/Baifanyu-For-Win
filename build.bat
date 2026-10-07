@echo off
rem BaifanYu (白饭鱼) for Windows — build wrapper.
rem
rem build.sh does the real work; it needs a POSIX shell, which ships with
rem Git for Windows and with MSYS2.  This .bat just finds one and calls it,
rem so you can double-click it or run it from cmd.exe.
setlocal

set "HERE=%~dp0"
set "SH="

where bash >nul 2>nul && set "SH=bash"
if not defined SH if exist "%ProgramFiles%\Git\bin\bash.exe" set "SH=%ProgramFiles%\Git\bin\bash.exe"
if not defined SH if exist "%ProgramFiles(x86)%\Git\bin\bash.exe" set "SH=%ProgramFiles(x86)%\Git\bin\bash.exe"
if not defined SH if exist "C:\msys64\usr\bin\bash.exe" set "SH=C:\msys64\usr\bin\bash.exe"

if not defined SH (
    echo.
    echo   Could not find bash.exe.
    echo   Install "Git for Windows" ^(which includes Git Bash^) or MSYS2, then
    echo   run:  bash build.sh
    echo.
    exit /b 1
)

"%SH%" "%HERE%build.sh" %*
set "RC=%ERRORLEVEL%"
if not "%RC%"=="0" (
    echo.
    echo   Build failed with exit code %RC%.
)
endlocal & exit /b %RC%
