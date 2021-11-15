@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

set ACTION=%1
if "%1" == "" set ACTION=make

echo -^> %ACTION% vendor-httpd

goto cmdXDefined
:cmdX
%*
if errorlevel 1 goto cmdXError
goto :eof
:cmdXError
echo "Error: %ACTION%"
exit 1
:cmdXDefined

if not "%ACTION%" == "make" goto :eof

call :cmdX xyo-cc --mode=%ACTION% --source-has-archive httpd

if not exist output\ mkdir output
if not exist temp\ mkdir temp

set INCLUDE=%XYO_PATH_REPOSITORY%\include;%INCLUDE%
set LIB=%XYO_PATH_REPOSITORY%\lib;%LIB%
set WORKSPACE_PATH=%CD%
set WORKSPACE_PATH_BUILD=%WORKSPACE_PATH%\temp

if exist %WORKSPACE_PATH_BUILD%\build.done.flag goto :eof

pushd temp

if not exist httpd\ mkdir httpd

rem --- required
set VENDOR=pcre-8.45-%XYO_PLATFORM%-dev
if not exist ..\..\vendor-pcre\release\%VENDOR%.7z goto pcre_getFromGitHub
copy /Y /B ..\..\vendor-pcre\release\%VENDOR%.7z %VENDOR%.7z
goto pcre_process
:pcre_getFromGitHub
set WEB_LINK=https://github.com/g-stefan/vendor-pcre/releases/download/v8.45/%VENDOR%.7z
curl --insecure --location %WEB_LINK% --output %VENDOR%.7z
:pcre_process
7z x -aoa -o. %VENDOR%.7z
xcopy  /Y /S /E "%VENDOR%\*" "httpd\"
rmdir /Q /S %VENDOR%


rem --- required
set VENDOR=apr-1.7.0-%XYO_PLATFORM%-dev
if not exist ..\..\vendor-apr\release\%VENDOR%.7z goto apr_getFromGitHub
copy /Y /B ..\..\vendor-apr\release\%VENDOR%.7z %VENDOR%.7z
goto apr_process
:apr_getFromGitHub
set WEB_LINK=https://github.com/g-stefan/vendor-apr/releases/download/v1.7.0/%VENDOR%.7z
curl --insecure --location %WEB_LINK% --output %VENDOR%.7z
:apr_process
7z x -aoa -o. %VENDOR%.7z
xcopy  /Y /S /E "%VENDOR%\*" "httpd\"
rmdir /Q /S %VENDOR%

rem --- required
set VENDOR=apr-util-1.6.1-%XYO_PLATFORM%-dev
if not exist ..\..\vendor-apr-util\release\%VENDOR%.7z goto apr_util_getFromGitHub
copy /Y /B ..\..\vendor-apr-util\release\%VENDOR%.7z %VENDOR%.7z
goto apr_util_process
:apr_util_getFromGitHub
set WEB_LINK=https://github.com/g-stefan/vendor-apr-util/releases/download/v1.6.1/%VENDOR%.7z
curl --insecure --location %WEB_LINK% --output %VENDOR%.7z
:apr_util_process
7z x -aoa -o. %VENDOR%.7z
xcopy  /Y /S /E "%VENDOR%\*" "httpd\"
rmdir /Q /S %VENDOR%

popd


if not exist temp\cmake mkdir temp\cmake
pushd temp\cmake

SET CC=cl.exe
SET CXX=cl.exe

SET CMD_CONFIG=cmake
SET CMD_CONFIG=%CMD_CONFIG% ../../source
SET CMD_CONFIG=%CMD_CONFIG% -G "Ninja"
SET CMD_CONFIG=%CMD_CONFIG% -DCMAKE_BUILD_TYPE=Release
SET CMD_CONFIG=%CMD_CONFIG% -DCMAKE_INSTALL_PREFIX=%WORKSPACE_PATH_BUILD%\httpd
SET CMD_CONFIG=%CMD_CONFIG% -DCMAKE_PREFIX_PATH=%XYO_PATH_REPOSITORY%

if not exist %WORKSPACE_PATH_BUILD%\build.configured.flag %CMD_CONFIG%
if errorlevel 1 goto makeError
if not exist %WORKSPACE_PATH_BUILD%\build.configured.flag echo configured > %WORKSPACE_PATH_BUILD%\build.configured.flag

ninja
if errorlevel 1 goto makeError
ninja install
if errorlevel 1 goto makeError
ninja clean
if errorlevel 1 goto makeError

goto buildDone

:makeError
popd
echo "Error: make"
exit 1

:buildDone
popd
echo done > %WORKSPACE_PATH_BUILD%\build.done.flag

rem ---

call :cmdX xyo-cc --mode=%ACTION% --exe rotatelogsw --source-path=build/source --use-project=xyo.static --use-lib=libapr-1

copy /Y /B output\bin\rotatelogsw.exe temp\httpd\bin\rotatelogsw.exe
