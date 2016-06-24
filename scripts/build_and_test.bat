@echo on

if "%TOOLSET%" == "vs2010" (
    if "%2" == "x64" (
        call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" "/%1" "/%2"
        if %errorlevel% neq 0 exit /b %errorlevel%
    )
)

msbuild /m "/p:Configuration=%1" "/p:Platform=%2" "build\%TOOLSET%\sc.sln" /logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"
if %errorlevel% neq 0 exit /b %errorlevel%

bin\sc_tests.exe --reporter console
if %errorlevel% neq 0 exit /b %errorlevel%
