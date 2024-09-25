@echo off
cls

if not exist "build\" (mkdir build)

for %%a in (%*) do set "%%a=1"

set cl_flags=/Zi /W0 /MD /nologo /Feapp.exe /link user32.lib gdi32.lib
set src=../src/main.cpp

pushd build

if "%release%"=="1" (
	echo building release build...
	cl %src% /DBUILD_RELEASE %cl_flags% /SUBSYSTEM:WINDOWS
) else (
	echo building debug build...
	cl %src% /DBUILD_DEBUG %cl_flags% /SUBSYSTEM:CONSOLE
)

popd

if errorlevel 1 (
	echo build failed.
) else (
	copy build\app.exe app.exe >NUL
)

for %%a in (%*) do set "%%a=0"
set cl_flags=
set src=



