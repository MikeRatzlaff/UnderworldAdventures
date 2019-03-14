@echo off
REM
REM Underworld Adventures - an Ultima Underworld hacking project
REM Copyright (c) 2019 Underworld Adventures Team
REM
REM Runs SonarCloud analysis build
REM

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community

REM set this to your SonarQube tools folder
set SONARQUBE=D:\devel\tools\SonarQube

if "%SONARLOGIN%" == "" echo "Environment variable SONARLOGIN is not set! Obtain a new token and set the environment variable!"
if "%SONARLOGIN%" == "" exit 1

REM
REM Preparations
REM
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

set PATH=%PATH%;%SONARQUBE%\build-wrapper-win-x86;%SONARQUBE%\sonar-scanner-msbuild;%OPENCPPCOVERAGE%

REM
REM Build using SonarQube scanner for MSBuild
REM
rmdir .\bw-output /s /q 2> nul

SonarScanner.MSBuild.exe begin ^
    /k:"UnderworldAdventures" ^
    /v:"0.10" ^
    /d:"sonar.cfamily.build-wrapper-output=%CD%\bw-output" ^
    /d:"sonar.host.url=https://sonarcloud.io" ^
    /d:"sonar.organization=vividos-github" ^
    /d:"sonar.login=%SONARLOGIN%"

REM
REM Rebuild Release|Win32
REM
build-wrapper-win-x86-64.exe --out-dir bw-output msbuild uwadv.sln /m /property:Configuration=SonarCloud,Platform=Win32 /target:Rebuild

SonarScanner.MSBuild.exe end /d:"sonar.login=%SONARLOGIN%"

pause