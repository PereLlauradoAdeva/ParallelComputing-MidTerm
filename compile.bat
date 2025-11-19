@echo off
echo Compilant projecte...
g++ -fopenmp -O2 -std=c++17 main.cpp sequential.cpp parallel.cpp -o main.exe
if %errorlevel% == 0 (
    echo Compilacio completada correctament!
    echo Executable: main.exe
) else (
    echo ERROR en la compilacio!
)
pause
