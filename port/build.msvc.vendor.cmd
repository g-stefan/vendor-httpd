@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

echo -^> vendor vendor-httpd

if not exist archive\ mkdir archive

pushd archive
set VENDOR=httpd-2.4.46
set WEB_LINK=https://mirror.efect.ro/apache//httpd/httpd-2.4.46.tar.gz
if not exist %VENDOR%.tar.gz curl --insecure --location %WEB_LINK% --output %VENDOR%.tar.gz
7z x %VENDOR%.tar.gz -so | 7z x -aoa -si -ttar -o.
del /F /Q %VENDOR%.tar.gz
7zr a -mx9 -mmt4 -r- -sse -w. -y -t7z %VENDOR%.7z %VENDOR%
rmdir /Q /S %VENDOR%
popd
