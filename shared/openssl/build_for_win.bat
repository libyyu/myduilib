@echo off
@set currentPath=%~dp0
@set currentPath=%currentPath:~,-1%
@set sevenZip=%1
@set arch=%2
@set static=%3
@set opensslname=openssl-1.0.2l

@echo --------------------------------------------
@echo - currentPath is %currentPath%
@echo --------------------------------------------

:: 1)unzip openssl
@call :unzip_openssl_source
@echo --------------------------------------------

:: 2)cd build dir, and build it
@pushd %currentPath%\%opensslname%
@echo --------------------------------------------
@echo - Switch X86 Env
@call vcvarsall.bat x86

:: 3)build x86 release
@echo - configure VC-WIN32
@set outputlib=%currentPath%\output_lib\release\x86
@call :make_folder %outputlib%
@perl configure VC-WIN32 no-asm --prefix=%outputlib%
@if %ERRORLEVEL% NEQ 0 goto ERROR
@echo --------------------------------------------
@call :build_openssl x86
@echo --------------------------------------------
@echo - build openssl x86-release finished
@echo --------------------------------------------
@call :sync_targets release x86
@echo --------------------------------------------

:: 4)build x86 debug
@echo - configure debug-VC-WIN32
@set outputlib=%currentPath%\output_lib\debug\x86
@call :make_folder %outputlib%
@perl configure debug-VC-WIN32 no-asm --prefix=%outputlib%
@if %ERRORLEVEL% NEQ 0 goto ERROR
@echo --------------------------------------------
@call :build_openssl x86 debug
@echo --------------------------------------------
@echo - build openssl x86-debug finished
@echo --------------------------------------------
@call :sync_targets debug x86
@echo --------------------------------------------

@echo --------------------------------------------
@echo - Switch X64 Env
@call vcvarsall.bat x64

:: 5)build x64 release
@echo - configure VC-WIN64A
@set outputlib=%currentPath%\output_lib\release\x64
@call :make_folder %outputlib%
@perl configure VC-WIN64A no-asm --prefix=%outputlib%
@if %ERRORLEVEL% NEQ 0 goto ERROR
@echo --------------------------------------------
@call :build_openssl x64
@echo --------------------------------------------
@echo - build openssl x64-release finished
@echo --------------------------------------------
@call :sync_targets release x64
@echo --------------------------------------------

:: 6)build x64 debug
@echo - configure debug-VC-WIN64A
@set outputlib=%currentPath%\output_lib\debug\x64
@call :make_folder %outputlib%
@perl configure debug-VC-WIN64A no-asm --prefix=%outputlib%
@if %ERRORLEVEL% NEQ 0 goto ERROR
@echo --------------------------------------------
@call :build_openssl x64 debug
@echo --------------------------------------------
@echo - build openssl x64-debug finished
@echo --------------------------------------------
@call :sync_targets debug x64
@echo --------------------------------------------

:: 2)leave build dir
@popd
@echo on
@goto :eof


::create directory
:make_folder
@if not exist %1 (
    @echo --------------------------------------------
    @echo - make directory %1
    @echo --------------------------------------------
    @md %1
    @if %ERRORLEVEL% NEQ 0 goto ERROR
)
goto :eof

::unzip file
:seven_zip_op
@echo 7z %*
@%sevenZip% %*
@if %ERRORLEVEL% NEQ 0 goto ERROR
goto :eof

::unzip openssl
:unzip_openssl_source
@if not exist %currentPath%\%opensslname% (
    @echo --------------------------------------------
    @echo - unzip openssl source code
    @echo --------------------------------------------
	@call :seven_zip_op e %SHARE_CODE%\%opensslname%.tar.gz -o%currentPath% -aoa
	@call :seven_zip_op x %currentPath%\%opensslname%.tar -o%currentPath%
    @if exist %currentPath%\%opensslname%.tar (del "%currentPath%\%opensslname%.tar")
)
goto :eof


:fix_mak_file
@setlocal enabledelayedexpansion
@set ntdll_mak_file=%currentPath%\%opensslname%\ms\ntdll.mak
@set ntdll_mak_file_temp=%currentPath%\%opensslname%\ms\ntdll_temp.mak
if exist %ntdll_mak_file_temp% (del %ntdll_mak_file_temp%) else echo - create temp file ntdll_temp.mak

for /f "delims=" %%i in (%ntdll_mak_file%) do (
    set str=%%i
    set str=!str:/MT=/MD!
    echo !str!>>%ntdll_mak_file_temp%
)
@move /y "%ntdll_mak_file_temp%" "%ntdll_mak_file%"
@if %ERRORLEVEL% NEQ 0 goto ERROR

@set nt_mak_file=%currentPath%\%opensslname%\ms\nt.mak
@set nt_mak_file_temp=%currentPath%\%opensslname%\ms\nt_temp.mak
if exist %nt_mak_file_temp% (del %nt_mak_file_temp%) else echo - create temp file nt_temp.mak

for /f "delims=" %%i in (%nt_mak_file%) do (
    set str=%%i
    set str=!str:/MT=/MD!
    echo !str!>>%nt_mak_file_temp%
)
@move /y "%nt_mak_file_temp%" "%nt_mak_file%"
@if %ERRORLEVEL% NEQ 0 goto ERROR

@endlocal enabledelayedexpansion
goto :eof

:build_openssl
echo --------------------------------------------
echo - build %static% %arch% %*
echo --------------------------------------------

@if /i "%1" == "x64" (
    @echo - do_win64a.bat
    @call ms\do_win64a.bat
    @if %ERRORLEVEL% NEQ 0 goto ERROR
) else (
    @echo - do_ms.bat
    @call ms\do_ms.bat
    @if %ERRORLEVEL% NEQ 0 goto ERROR
)
::replace "/MT" to "/MD" in ms/ntdll.mak,ms\nt.mak
@if /i "%arch%" == "md" (
    @echo - fix mak_file with /MD
    @call :fix_mak_file
)

@set mak_file=ms\ntdll.mak
@if /i "%static%" == "static" (
	@set mak_file=ms\nt.mak
)

@call :safe_nmake -f %mak_file% clean
@call :safe_nmake -f %mak_file%
@call :safe_nmake -f %mak_file% install
goto :eof

:safe_nmake
@echo nmake %*
@nmake %*
@if %ERRORLEVEL% NEQ 0 goto ERROR
goto :eof

:sync_targets
@echo --------------------------------------------
@echo - sync openssl build result to workspace %* ...
@echo --------------------------------------------
@echo - %outputlib%
@call :make_folder %currentPath%\..\..\libs\windows\%1\%2
@call :make_folder %currentPath%\..\..\%1\windows\%2

@copy /y %outputlib%\lib\libeay32.lib %currentPath%\..\..\libs\windows\%1\%2\libeay32.lib
@if %ERRORLEVEL% NEQ 0 goto ERROR
@copy /y %outputlib%\lib\ssleay32.lib %currentPath%\..\..\libs\windows\%1\%2\ssleay32.lib
@if %ERRORLEVEL% NEQ 0 goto ERROR
@copy /y %outputlib%\bin\openssl.exe %currentPath%\..\..\%1\windows\%2\openssl.exe
@if %ERRORLEVEL% NEQ 0 goto ERROR
@xcopy /Q /Y /E %outputlib%\include %currentPath%\..\..\thirdpart
@if %ERRORLEVEL% NEQ 0 goto ERROR
goto :eof

:ERROR
@pause
@exit /B 1