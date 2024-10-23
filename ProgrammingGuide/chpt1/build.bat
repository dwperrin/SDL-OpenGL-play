@echo off

REM This will get you started building a simple one-file SDL application.
REM Place this in the same directory as your .cpp file and run it from there.

set Pincludes=/I D:\dwp\dev\libs\SDL\SDL2-2.26.3\include /I D:\dwp\dev\libs\glew\glew-2.2.0\include /I ../includes
set Plibs="D:\dwp\dev\libs\glew\glew-2.2.0\lib\Release\x64\glew32.lib" "D:\dwp\dev\libs\SDL\SDL2-2.26.3\lib\x64\SDL2.lib" OpenGL32.lib user32.lib gdi32.lib kernel32.lib

if not defined DevEnvDir (
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
)

if not exist "build" mkdir build
pushd .\build
del /Q *
copy "D:\dwp\dev\libs\SDL\SDL2-2.26.3\lib\x64\SDL2.dll" .
copy "D:\dwp\dev\libs\glew\glew-2.2.0\bin\Release\x64\glew32.dll" .
rem copy ..\triangles.vert .
REM copy ..\triangles.frag .


cl.exe /FC /Zi /nologo ..\*.cpp %Pincludes% /link /SUBSYSTEM:windows %Plibs%  /out:app.exe
popd