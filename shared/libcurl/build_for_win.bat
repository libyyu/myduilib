@echo off

@set currentPath=%~dp0
@set currentPath=%currentPath:~,-1%
@set sevenZip=%1
@set supportXP=%2
@set arch=%3
@set static=%4
@set curlfoldername=curl-7.63.0
@set vcplatform=14
:: 1) unzip curl
@call :unzip_curl_source

:: 2) enter curl dir
@pushd %currentPath%\%curlfoldername%
@pushd winbuild

:: 3) support Windows XP (add build command into "MakefileBuild.vc")
if "%supportXP%" == "enable_xp" (
    @echo modify "winbuild/MakefileBuild.vc" to support windows xp
    python %currentPath%\build_for_win_support_xp.py %currentPath%\%curlfoldername%\winbuild\MakefileBuild.vc
    @if %ERRORLEVEL% NEQ 0 goto ERROR
)

:: 4) build
@set RTLIBCFG=static
@set RTLIB=/MT
@set RTLIB_DEBUG=/MTd
@if "%arch%" == "md" (
    @set RTLIBCFG=dll
    @set RTLIB=/MD
    @set RTLIB_DEBUG=/MDd
)
@set mode=dll
@if "%static%" == "static" (
    @set mode=static
)
@echo --------------------------------------------
@echo - Switch X86 Env
@call vcvarsall x86

@echo build curl %mode% %RTLIBCFG% release x86
@call :safe_nmake /f Makefile.vc clean
@call :safe_nmake /f Makefile.vc WITH_DEVEL=../../../openssl/output_lib/release/x86 mode=%mode% VC=%vcplatform% RTLIBCFG=%RTLIBCFG% RTLIB=%RTLIB% RTLIB_DEBUG=%RTLIB_DEBUG% WITH_SSL=%mode% ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no GEN_PDB=yes DEBUG=no
@call :sync_targets release x86

@echo build curl %mode% %RTLIBCFG% debug x86
@call :safe_nmake /f Makefile.vc clean
@call :safe_nmake /f Makefile.vc WITH_DEVEL=../../../openssl/output_lib/debug/x86 mode=%mode% VC=%vcplatform% RTLIBCFG=%RTLIBCFG% WITH_SSL=%mode% ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no GEN_PDB=yes DEBUG=yes
@call :sync_targets debug x86

@echo --------------------------------------------
@echo - Switch X64 Env
@call vcvarsall x64

@echo build curl %mode% %RTLIBCFG% release x64
@call :safe_nmake /f Makefile.vc clean
@call :safe_nmake /f Makefile.vc WITH_DEVEL=../../../openssl/output_lib/release/x64 mode=%mode% VC=%vcplatform% RTLIBCFG=%RTLIBCFG% WITH_SSL=%mode% ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no GEN_PDB=yes DEBUG=no
@call :sync_targets release x64

@echo build curl %mode% %RTLIBCFG% debug x64
@call :safe_nmake /f Makefile.vc clean
@call :safe_nmake /f Makefile.vc WITH_DEVEL=../../../openssl/output_lib/debug/x64 mode=%mode% VC=%vcplatform% RTLIBCFG=%RTLIBCFG% WITH_SSL=%mode% ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no GEN_PDB=yes DEBUG=yes
@call :sync_targets debug x64

@popd
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

::unzip curl
:unzip_curl_source
@if not exist %currentPath%\%curlfoldername% (
    @echo --------------------------------------------
    @echo - unzip curl source code
    @echo --------------------------------------------
    @call :seven_zip_op e %SHARE_CODE%\%curlfoldername%.tar.gz -o%currentPath% -aoa
	@call :seven_zip_op x %currentPath%\%curlfoldername%.tar -o%currentPath%
    @if exist %currentPath%\%curlfoldername%.tar (del "%currentPath%\%curlfoldername%.tar")
)
goto :eof

:safe_nmake
@echo nmake %*
@nmake %*
@if %ERRORLEVEL% NEQ 0 goto ERROR
goto :eof

:sync_targets
@echo --------------------------------------------
@echo - sync libcurl build result to workspace ...
@echo --------------------------------------------
@call :make_folder %currentPath%\..\..\libs\windows\%1\%2
@call :make_folder %currentPath%\..\..\%1\windows\%2
@set output=%currentPath%\%curlfoldername%\builds\libcurl-vc%vcplatform%-%2-%1-%mode%-ssl-%mode%-ipv6
@echo - %outputlib%

@if /i "%1" == "release" (
    @copy /y "%output%\lib\libcurl_a.lib" %currentPath%\..\..\libs\windows\%1\%2\libcurl.lib
    @if %ERRORLEVEL% NEQ 0 goto ERROR
    @copy /y "%output%\lib\libcurl_a.pdb" %currentPath%\..\..\libs\windows\%1\%2\libcurl.pdb
)else (
    @copy /y "%output%\lib\libcurl_a_debug.lib" %currentPath%\..\..\libs\windows\%1\%2\libcurl.lib
    @if %ERRORLEVEL% NEQ 0 goto ERROR
    @copy /y "%output%\lib\libcurl_a_debug.pdb" %currentPath%\..\..\libs\windows\%1\%2\libcurl.pdb
)
@copy /y "%output%\bin\curl.exe" %currentPath%\..\..\%1\windows\%2\curl.exe
@if %ERRORLEVEL% NEQ 0 goto ERROR
@xcopy /Q /Y /E %output%\include %currentPath%\..\..\thirdpart
@if %ERRORLEVEL% NEQ 0 goto ERROR
goto :eof


:ERROR
@pause
@exit /B 1