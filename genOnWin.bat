@echo off
set masterfolder=%cd%

echo Configuring...
mkdir build
cd build
call cmake -G "Visual Studio 17 2022" -T host=x64 -A x64 ..

cd %masterfolder%
