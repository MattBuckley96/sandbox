@echo off
set BUILD_DIR=build
set CONFIG=Debug
set GENERATOR=Ninja
set PROJECT=sandbox

where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo CMake not found in PATH. Please add it to PATH or provide the full path.
    exit /b 1
)

if exist %PROJECT%.exe (
    del %PROJECT%.exe
)

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cd %BUILD_DIR%

cmake -G %GENERATOR% -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed.
    exit /b 1
)

cmake --build . --config %CONFIG%

if %ERRORLEVEL% neq 0 (
    echo Build failed.
    exit /b 1
)

call %PROJECT%.exe

cd ..