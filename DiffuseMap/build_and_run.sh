#!/bin/bash

# 设置脚本路径
PROJECT_DIR=$(pwd)
BUILD_DIR="$PROJECT_DIR/build"

# 检查是否存在 build 目录，如果不存在则创建
if [ ! -d "$BUILD_DIR" ]; then
    echo "创建 build 目录..."
    mkdir -p "$BUILD_DIR"
fi

# 进入 build 目录
cd "$BUILD_DIR"

# 运行 CMake 和构建
echo "配置并构建项目..."
cmake ..
cmake --build .

# 返回项目根目录
cd "$PROJECT_DIR"

# 运行生成的可执行文件
EXECUTABLE="$PROJECT_DIR/MyOpenGLProject"
if [ -f "$EXECUTABLE" ]; then
    echo "运行程序..."
    ./"MyOpenGLProject"
else
    echo "错误: 可执行文件未找到!"
fi
