@echo off
rem 컴파일
g++ -Iheader main.cpp src/header.cpp src/map.cpp src/character.cpp -o main.exe
if errorlevel 1 (
    echo 빌드 실패!
    pause
    exit /b 1
)

rem 실행
echo 실행 중...
main.exe

pause