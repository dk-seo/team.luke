@echo off

echo Hold on while I just build this for Visual Studios 2013
echo.
"../tools/premake5" --file=premake/premake5-prj.lua vs2013
goto quit

:quit
pause
:end
