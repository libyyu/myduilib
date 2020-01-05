@echo off
::必须在当前目录执行
@set self_path=%~dp0
@set self_path=%self_path:~,-1%
echo current dir: %self_path%


xmake f -c -p windows -a x86 --vs=vs2017 --verbose --backtrace
@if errorlevel 1 goto :BAD

xmake --verbose --backtrace
@if errorlevel 1 goto :BAD
goto :SUCCESS

:BAD
echo error
@pause
exit 1

:SUCCESS
@pause
exit 0
