@echo off
set masterfolder=%cd%

echo Starting CircleGen...
echo. & echo. & echo. & echo.
echo Configuring ZLib...
cd zlib
mkdir build
cd build
call cmake -G "Visual Studio 17 2022" -T host=x64 -A x64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=%cd%/install/ .. 

echo. & echo. & echo. & echo.
echo Building ZLib...
call cmake --build . --target install --

cd %masterfolder%



echo. & echo. & echo. & echo.
echo Configuring LibPng...
cd libpng
mkdir build
cd build
call cmake -G "Visual Studio 17 2022" -T host=x64 -A x64 -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_INSTALL_PREFIX=%cd%/install/ -DZLIB_LIBRARY=%masterfolder%/zlib/build/install/lib -DZLIB_INCLUDE_DIR=%masterfolder%/zlib/build/install/include .. 

echo. & echo. & echo. & echo.
echo Building LibPng...
call cmake --build . --config Debug --target png_static --

copy %masterfolder%\libpng\png.h %masterfolder%\libpng\build\png.h
copy %masterfolder%\libpng\pngconf.h %masterfolder%\libpng\build\pngconf.h

cd %masterfolder%




echo. & echo. & echo. & echo.
echo Configuring CircleGen...
mkdir build
cd build
call cmake -G "Visual Studio 17 2022" -T host=x64 -A x64 -DLIBPNG_INCLUDE_DIRS=%masterfolder%\libpng\build -DZLIB_LIBRARY_DIRS=%masterfolder%/zlib/build/install/lib -DZLIB_LIBRARY=zlibstaticd.lib -DLIBPNG_LIBRARY_DIRS=%masterfolder%/libpng/build -DLIBPNG_LIBRARIES=libpng16_staticd.lib ..

cd %masterfolder%
