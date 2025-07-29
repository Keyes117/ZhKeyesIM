#!/bin/bash
echo "正在构建ZhKeyesIM项目（动态库版本）..."

if [ ! -d "build" ]; then
    mkdir build
fi
cd build

# 配置CMake项目
cmake .. -DCMAKE_BUILD_TYPE=Debug
if [ $? -ne 0 ]; then
    echo "CMake配置失败！"
    exit 1
fi

# 构建项目
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "构建失败！"
    exit 1
fi

echo "构建成功！"
echo "可执行文件位置: build/bin/"
echo "动态库位置: build/bin/"