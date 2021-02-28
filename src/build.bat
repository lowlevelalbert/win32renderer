@echo off

pushd ..\build
cl /Zi ..\src\*.c /Fe:renderer.exe user32.lib gdi32.lib
popd
