@echo off
::必须在当前目录执行
@set self_path=%~dp0
@set self_path=%self_path:~,-1%
@echo current dir: %self_path%

@if exist %self_path%\xmake goto :SUCCESS

git clone https://github.com/libyyu/xmake.git %self_path%\xmake
@if errorlevel 1 goto :BAD

git pull %self_path%\xmake
@if errorlevel 1 goto :BAD

goto :SUCCESS

:BAD
@echo get xmake error
@pause
exit 1

:SUCCESS
exit 0
