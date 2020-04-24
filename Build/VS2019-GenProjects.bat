@echo off
pushd %~dp0\..\
call Hazel\vendor\bin\premake\premake5.exe vs2019
popd
PAUSE