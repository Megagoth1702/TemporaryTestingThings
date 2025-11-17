@echo off
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

set "GITATTRIBUTES_FILE=.gitattributes"
if not exist "%GITATTRIBUTES_FILE%" (
    echo ERROR: %GITATTRIBUTES_FILE% not found
    exit /b 1
)

rem Initialize FileTypes variable
set "FileTypes="

rem Read .gitattributes, extract globs from line beginnings, ignore comments and blanks
for /f "usebackq tokens=1" %%i in ("%GITATTRIBUTES_FILE%") do (
    rem Skip comments and blank lines
    set "ext=%%i"
    if not "!ext!"=="" (
        if not "!ext:~0,1!"=="#" (
            rem Only include globs beginning with "*"
            if "!ext:~0,1!"=="*" (
                if defined FileTypes (
                    set "FileTypes=!FileTypes!,!ext!"
                ) else (
                    set "FileTypes=!ext!"
                )
            )
        )
    )
)

if not defined FileTypes (
    echo No valid globs found in %GITATTRIBUTES_FILE%.
    exit /b 1
)

echo Running BFG Repo-Cleaner to delete specified file types: %FileTypes%
java -jar BFG_CleanLFS.jar --delete-files "%FileTypes%" .git

echo Running Git commands to expire reflogs and collect garbage...
git reflog expire --expire=now --all
git gc --prune=now --aggressive

rem If .git.bfg-report exists, remove it (.git.bfg-report is a file, not a directory)
if exist ".git.bfg-report" (
    del /f /q ".git.bfg-report"
)

echo Repository cleanup and LFS configuration complete.
PAUSE