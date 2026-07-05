#!/bin/bash

# Скрипт для запуска 3DViewer с правильными переменными окружения WSL

echo "Setting up OpenGL environment for WSL..."

# Устанавливаем переменные окружения для программного рендеринга
export LIBGL_ALWAYS_SOFTWARE=1
export GALLIUM_DRIVER=llvmpipe

# Дополнительные переменные для стабильности
export MESA_GL_VERSION_OVERRIDE=3.3
export MESA_GLSL_VERSION_OVERRIDE=330

echo "Environment variables set:"
echo "LIBGL_ALWAYS_SOFTWARE = $LIBGL_ALWAYS_SOFTWARE"
echo "GALLIUM_DRIVER = $GALLIUM_DRIVER"
echo "MESA_GL_VERSION_OVERRIDE = $MESA_GL_VERSION_OVERRIDE"
echo "MESA_GLSL_VERSION_OVERRIDE = $MESA_GLSL_VERSION_OVERRIDE"
echo "DISPLAY = $DISPLAY"
echo ""

# Проверяем, что исполняемый файл существует
if [ ! -f "./build/3DViewer" ]; then
    echo "Error: 3DViewer executable not found. Please compile first with:"
    echo "  make clean"
    echo "  qmake6 3DViewer.pro"
    echo "  make"
    exit 1
fi

echo "Starting 3DViewer..."
echo "=========================="

# Запускаем приложение
./build/3DViewer

echo ""
echo "3DViewer finished."
