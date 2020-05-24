@echo off

@set workdir=%~dp0
@set workdir=%workdir:~,-1%

@REM get 7z and openssl
@call %workdir%\get_shared.bat
@if %ERRORLEVEL% NEQ 0 goto ERROR

@REM config environment
@call %workdir%\set_path.bat
@if %ERRORLEVEL% NEQ 0 goto ERROR

@set sevenzip=%SEVEN_ZIP%
@echo workdir is %workdir%
@echo sevenzip is %sevenzip%

@echo enter directory %workdir%
@pushd %workdir%

:: build libcurl
@echo enter directory %workdir%\libcurl
@pushd %workdir%\libcurl
@call build_for_win.bat %sevenzip% noenable_xp md static
@if %ERRORLEVEL% NEQ 0 goto ERROR
@echo leave directory %workdir%\libcurl
@popd


@echo leave directory %workdir%
@popd

@echo --------------------------------------------
@echo - build finished
@echo --------------------------------------------
@echo on
@pause
@exit /B 0

:ERROR
@echo Error occured, please check it.
@pause
@exit /B 1

