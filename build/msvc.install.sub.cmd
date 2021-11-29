@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

rem --- make

cmd.exe /C build\msvc.make.cmd
if errorlevel 1 exit 1

rem ---

if not exist "%INSTALL_PATH_BIN%\" mkdir "%INSTALL_PATH_BIN%"
xcopy /Y /S /E "output\*" "%INSTALL_PATH_BIN%\"
rmdir /Q /S "%INSTALL_PATH_BIN%\include"
rmdir /Q /S "%INSTALL_PATH_BIN%\lib"
rmdir /Q /S "%INSTALL_PATH_BIN%\manual"

rem --- dev

if not exist "%INSTALL_PATH_DEV%\" mkdir "%INSTALL_PATH_DEV%"
xcopy /Y /S /E "output\*" "%INSTALL_PATH_DEV%\"
