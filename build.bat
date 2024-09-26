@echo off

:: get arguments
set "base_file=%~1"
set "build_mode=%~2"
set "src_dir=src"
set "extensions=c cpp"

:: defaults
if "%base_file%"=="" set "base_file=main"
if "%build_mode%"=="" set "build_mode=debug"

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

set "common_flags=/nologo /FC /W0"
set "common_link_flags=/link /INCREMENTAL:NO"

if "%build_mode%"=="debug" (
	echo [info] build mode: debug.
	set "build_flags=/DBUILD_DEBUG /Zi /Od"
	set "link_flags=/SUBSYSTEM:CONSOLE"
	goto compile
) 

if "%build_mode%"=="release" (
	echo [info] build mode: debug.
	set "build_flags=/DBUILD_RELEASE /O2 /Ob1"
	set "link_flags=/SUBSYSTEM:WINDOWS"
	goto compile
) 

echo [error] invalid build mode: "%build_mode%".
exit /b 1

:compile

:: create build directory if needed
if not exist "build\" (mkdir build)

:: compile 
pushd build
cl %common_flags% %build_flags% ..\%source_file% %common_link_flags% %link_flags%
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