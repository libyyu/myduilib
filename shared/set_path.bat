@echo off

@echo --------------------------------------------
@echo - config vc environment
@echo --------------------------------------------

@REM 设置环境变量,可手动修改这里
@set path=D:\Program Files\Git\usr\bin;%VS142COMNTOOLS%..\..\VC\Auxiliary\Build;%path%


@where perl 1>NUL 2>NUL
@if %ERRORLEVEL% NEQ 0 (
    @echo please install perl and add to system environment path.
    @goto ERROR
)

REM @where nasm 1>NUL 2>NUL
REM @if %ERRORLEVEL% NEQ 0 (
REM     @echo please install nasm and add to system environment path.
REM     @goto ERROR
REM )

@SET SEVEN_ZIP=%TEMP%\ShareCode\7-Zip\7z.exe
@if not exist %SEVEN_ZIP% (
    @echo 7z is not found.
    @goto ERROR
)

@if not exist "%VS142COMNTOOLS%..\..\VC\Auxiliary\Build\vcvarsall.bat" (
    @echo vcvarsall.bat not found.
    @goto ERROR
)

goto :eof


:ERROR
@echo set path error, please check it.
@exit /B 1