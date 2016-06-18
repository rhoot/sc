@echo on

if "%TOOLSET%" == "vs2010" if "%2" == "x64" call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" "/%1" "/%2"
msbuild /m "/p:Configuration=%1" "/p:Platform=%2" "build\%TOOLSET%\sc.sln" /logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"
bin\sc_tests.exe --reporter console