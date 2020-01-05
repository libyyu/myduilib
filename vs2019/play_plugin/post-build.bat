@echo off
@echo *******************PostBuild play_plugin*******************
@setlocal enabledelayedexpansion
@set configuration=%1
@set platform=%2
@set outfile="%3"
@set outdir="%4"
@set projectdir="%5"
@rem @xcopy "%projectdir%..\..\thirdpart\FLib\src\*.h" "%projectdir%..\..\thirdpart\FLib\include\" /i /e /y
@rem @xcopy "%projectdir%%outdir%*FLib.lib" "%projectdir%..\..\thirdpart\FLib\lib\shared\%configuration%\%platform%\" /i /e /y
@endlocal enabledelayedexpansion
@echo on
