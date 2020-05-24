@echo off

@set workdir=%~dp0
@set workdir=%workdir:~,-1%

@if not exist %TEMP%\ShareCode (
    @git clone https://github.com/libyyu/ShareCode.git %TEMP%\ShareCode
    @if %ERRORLEVEL% NEQ 0 goto ERROR
) else (
    pushd %TEMP%\ShareCode
    @git pull
    @if %ERRORLEVEL% NEQ 0 goto ERROR
    popd
)
@SET SHARE_CODE=%TEMP%\ShareCode

goto :eof


:ERROR
@echo get shared error occured, please check it.
@exit /B 1

