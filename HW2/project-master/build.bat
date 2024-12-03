@echo off
echo --- Starting build process ---

:: Компиляция программы
g++ -Iinclude -o YourProgram.exe test/main.cpp src/background_runner.cpp -mconsole

:: Проверка успешности компиляции
if %errorlevel% neq 0 (
    echo --- Compilation failed ---
    exit /b 1
)

echo --- Compilation successful ---
pause
