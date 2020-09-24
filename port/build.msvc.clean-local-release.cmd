@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

echo -^> clean-local-release vendor-httpd

if exist release\ rmdir /Q /S release
