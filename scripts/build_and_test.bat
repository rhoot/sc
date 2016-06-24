@echo on

msbuild /m "/p:Configuration=%1" "/p:Platform=%2" "build\%TOOLSET%\sc.sln" /logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"
if %errorlevel% neq 0 exit /b %errorlevel%

bin\sc_tests.exe --reporter console
if %errorlevel% neq 0 exit /b %errorlevel%
