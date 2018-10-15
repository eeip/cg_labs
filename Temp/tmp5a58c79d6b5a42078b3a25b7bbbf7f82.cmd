setlocal
set errorlevel=dummy
set errorlevel=
echo Checking Build System
setlocal
"C:\Program Files\CMake\bin\cmake.exe" -HC:/Users/lan13ep1/Program/CG_Labs/code -BC:/Users/lan13ep1/Program/CG_Labs/build --check-stamp-list CMakeFiles/generate.stamp.list --vs-solution-file "C:\Users\lan13ep1\Program\CG_Labs\build\CG_Labs.sln"
if %errorlevel% neq 0 goto :cmEnd
:cmEnd
endlocal & call :cmErrorLevel %errorlevel% & goto :cmDone
:cmErrorLevel
exit /b %1
:cmDone
if %errorlevel% neq 0 goto :VCEnd

:VCEnd
exit %errorlevel%
