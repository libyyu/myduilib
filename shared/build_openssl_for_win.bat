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

:: build openssl
@echo enter directory %workdir%\openssl
@pushd %workdir%\openssl
@call build_for_win.bat %sevenzip% md static
@if %ERRORLEVEL% NEQ 0 goto ERROR
@echo leave directory %workdir%\openssl
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

