@echo off
setlocal enabledelayedexpansion


:: default values
set "build_mode=debug"
set "compiler=msvc"
set "source_file=main.cpp"

:: get arguments
for %%a in (%*) do (
    if /I "%%a"=="debug" set "build_mode=debug"
    if /I "%%a"=="release" set "build_mode=release"
    if /I "%%a"=="msvc" set "compiler=msvc"
    if /I "%%a"=="clang" set "compiler=clang"
    set "base_file=%%a"
)

:: ensure the source file is not set to "debug", "release", "msvc", or "clang"
if /I "%base_file%"=="debug" set "base_file=main"
if /I "%base_file%"=="release" set "base_file=main"
if /I "%base_file%"=="msvc" set "base_file=main"
if /I "%base_file%"=="clang" set "base_file=main"

set "src_dir=src"
set "extensions=c cpp"

:: echo build mode/compiler
echo [build mode: %build_mode%]
echo [compiler: %compiler%]

:: find source file
set "source_file="
for %%e in (%extensions%) do (
     if exist "%src_dir%\%base_file%.%%e" (
        set "source_file=%src_dir%\%base_file%.%%e"
        goto :found
    )
)
echo [error] source file: "%base_file%" not found.
exit /b 1
:found

:: set compile flags

set "cl_common_flags=/nologo /FC /W0"
set "cl_common_link_flags=/link /INCREMENTAL:NO"

set "clang_common_flags=-Wno-everything"
set "clang_common_link_flags="

if "%build_mode%"=="debug" (
	set "cl_build_flags=/DBUILD_DEBUG=1 /Zi /Od /fsanitize=address"
	set "cl_link_flags=/SUBSYSTEM:CONSOLE"

	set "clang_build_flags=-D BUILD_DEBUG=1 -g -O0 -fsanitize=address"
	set "clang_link_flags=-lclang_rt.asan -fuse-ld=lld"

	goto compile
) 

if "%build_mode%"=="release" (
	set "cl_build_flags=/DBUILD_RELEASE /O2 /Zi /fsanitize=address"
	set "cl_link_flags=/SUBSYSTEM:WINDOWS"

	goto compile
) 

echo [error] invalid build mode: "%build_mode%".
exit /b 1

:compile

:: create build directory if needed
if not exist "build\" (mkdir build)

:: compile 
pushd build
if "%compiler%"=="msvc" (
cl %cl_common_flags% %cl_build_flags% ..\%source_file% %cl_common_link_flags% %cl_link_flags%
)
if "%compiler%"=="clang" (
clang-cl %clang_common_flags% %clang_build_flags% ..\%source_file% %clang_common_link_flags% %clang_link_flags%
)
popd

:: check for errors
if %ERRORLEVEL% neq 0 (
	echo [error] build failed.
	exit /b %ERRORLEVEL%
)

:: if no error copy file 
copy build\%base_file%.exe %base_file%.exe >NUL
echo [info] build successful.

:: run program
call %base_file%.exe

exit /b 0