@echo off
@echo *******************PostBuild FLib*******************
@setlocal enabledelayedexpansion
@set configuration=%1
@set platform=%2
@set outfile="%3"
@set outdir="%4"
@set projectdir="%5"
@rem @xcopy "%projectdir%..\..\thirdpart\FLib\src\*.h" "%projectdir%..\..\thirdpart\FLib\include\" /i /e /y /q
@endlocal enabledelayedexpansion
@echo on
