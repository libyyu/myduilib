@echo off
::�����ڵ�ǰĿ¼ִ��
@set self_path=%~dp0
@set self_path=%self_path:~,-1%
echo current dir: %self_path%


xmake f -c -p windows -a x86 --vs=vs2019 --genproj=y --boost_dir=D:\boost_1_70_0 --stack_dump=y
@if errorlevel 1 goto :BAD

xmake project -k vs2019 -m "release,debug" .
@if errorlevel 1 goto :BAD
goto :SUCCESS

:BAD
echo error
@pause
exit 1

:SUCCESS
@pause
exit 0
