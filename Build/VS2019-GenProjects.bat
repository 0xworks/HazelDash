@echo off
pushd %~dp0\..\
call Hazel\vendor\premake\bin\premake5.exe vs2019
popd
PAUSE