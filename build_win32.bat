@echo off
echo 正在构建ZhKeyesIM项目...

if not exist build mkdir build
cd build

REM 配置CMake项目
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo CMake配置失败！
    pause
    exit /b 1
)

REM 构建项目
cmake --build . --config Debug
if %ERRORLEVEL% neq 0 (
    echo 构建失败！
    pause
    exit /b 1
)

echo 构建成功！
echo 可执行文件位置: build\bin\Debug\
echo 动态库位置: build\bin\Debug\
echo 导入库位置: build\lib\Debug\
pause