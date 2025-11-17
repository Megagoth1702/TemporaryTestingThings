@echo off
:: =============================================================
:: Batch File: Drive Mapper
:: Author: Megagoth1702 & Grok
:: Version: 1.0
:: Date: November 17, 2025
:: =============================================================
:: ---------------------- Documentation ------------------------
:: This script maps a folder to a virtual drive letter on Windows for easy access.
::
:: To Use:
:: 1. Edit this file in Notepad.
:: 2. Set the AddonPath and Drive variables below.
:: 3. Save and run the .bat file (right-click > Run as administrator if needed).
::
:: Setting the Folder Path (AddonPath):
:: - Use the full path, in double quotes: "C:\Your\Folder\"
:: - Use backslashes (\) for Windows paths.
:: - End with a backslash (\) to map the folder's contents.
:: - Quotes handle spaces/special characters.
::
:: Good Examples:
::   set "AddonPath=C:\Games\Mods\MyModFolder\"
::   set "AddonPath=D:\User Files\Awesome Mod Pack\"
::   set "AddonPath=\\Network\Share\Mods\"
::
:: Bad Examples (Will Fail – Fix Them!):
::   set AddonPath=C:\Games\Mods\MyModFolder
::   set "AddonPath=C:/Games/Mods/MyModFolder/"
::   set "AddonPath=C:\Games\Mods\MyModFolder"
::   set "AddonPath=C:\FakeFolder\"
::
:: Drive Letter (Drive):
:: - Single uppercase letter (A-Z), unused (e.g., V or Z).
:: - Avoid C, D, etc.
::
:: Tips:
:: - Check if the folder exists with: dir "YourPath"
:: - Mapping is temporary (until reboot).
:: - To run on startup: Press Win+E, type shell:startup in the Explorer bar, and copy this .bat file there.
:: - Test on a safe folder first.
:: =============================================================

setlocal

:: ------------------ User Settings -------------------
set "AddonPath=A:\_AS_ArmaReforgerMods-------------------\Seize-And-Secure-Phoenix\"
set "Drive=V"

:: ----------------- Begin Script ---------------------
echo.
echo Attempting to map drive %Drive%: to "%AddonPath%"
echo Clearing any previous mapping on %Drive%: ...

subst %Drive%: /d >nul 2>&1

subst %Drive%: "%AddonPath%"

if errorlevel 1 (
    echo.
    echo FAILED to map %Drive%: (errorlevel = %errorlevel%)
    echo.
    echo   Does the folder exist?  Run: dir "%AddonPath%"
    echo   Is %Drive%: already used?   Run: subst
    echo   Path issue? Check quotes, slashes, spaces (see docs above).
) else (
    echo.
    echo SUCCESS! %Drive%: now maps to "%AddonPath%"
    echo Open Explorer and type %Drive%:\ in the address bar.
)

echo.
::pause

endlocal
exit /b