@echo off
setlocal EnableExtensions

for /f "tokens=*" %%g in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -latest -products * -property installationPath') do set "vs_path=%%g"
if not defined vs_path exit /b 1

if not defined QTX64_BIN set "QTX64_BIN=C:\Qt\6.10.1\msvc2022_64\bin"
if not defined QTARM64_BIN set "QTARM64_BIN=C:\Qt\6.10.1\msvc2022_arm64\bin"
if defined ProgramFiles(x86) (set "nsis=%ProgramFiles(x86)%\NSIS\makensis.exe") else (set "nsis=%ProgramFiles%\NSIS\makensis.exe")

call "%vs_path%\Common7\Tools\VsDevCmd.bat" -arch=x64
if errorlevel 1 exit /b %errorlevel%

for %%v in (avx2 avx512 avx10_1) do if not exist "artifacts\%%v\bin" mkdir "artifacts\%%v\bin"

call :build_x64 avx2 AdvancedVectorExtensions2
if errorlevel 1 exit /b %errorlevel%
call :build_x64 avx512 AdvancedVectorExtensions512
if errorlevel 1 exit /b %errorlevel%
call :build_x64 avx10_1 AdvancedVectorExtensions101
if errorlevel 1 exit /b %errorlevel%

rem The shared control application targets the baseline x64 instruction set.
msbuild Common.vcxproj /m /t:Rebuild /p:Configuration=Release /p:Platform=x64 /p:EnableEnhancedInstructionSet=AdvancedVectorExtensions2
if errorlevel 1 exit /b %errorlevel%

if exist build-control-x64 rmdir /s /q build-control-x64
mkdir build-control-x64
pushd build-control-x64
"%QTX64_BIN%\qmake.exe" ..\DeviceSelector\DeviceSelector.pro CONFIG+=release
if errorlevel 1 (popd & exit /b %errorlevel%)
nmake
if errorlevel 1 (popd & exit /b %errorlevel%)
popd

for %%v in (avx2 avx512 avx10_1) do (
  copy /y "build-control-x64\release\EqualizerAPO.exe" "artifacts\%%v\bin\EqualizerAPO.exe" >nul
  if exist "artifacts\%%v\qt" rmdir /s /q "artifacts\%%v\qt"
  "%QTX64_BIN%\windeployqt.exe" --dir "artifacts\%%v\qt" --release --no-opengl-sw "artifacts\%%v\bin\EqualizerAPO.exe"
  if errorlevel 1 exit /b %errorlevel%
  "%nsis%" /DBINPATH="..\artifacts\%%v\bin" /DLIBPATH="..\artifacts\%%v\qt" /DINSTALLER_OUTFILE="EqualizerAPO-nightly-x64-%%v.exe" Setup\Setup64.nsi
  if errorlevel 1 exit /b %errorlevel%
)

call "%vs_path%\Common7\Tools\VsDevCmd.bat" -arch=arm64
if errorlevel 1 exit /b %errorlevel%
msbuild Common.vcxproj /m /t:Rebuild /p:Configuration=Release /p:Platform=ARM64
if errorlevel 1 exit /b %errorlevel%
msbuild EqualizerAPO\EqualizerAPO.vcxproj /m /t:Rebuild /p:Configuration=Release /p:Platform=ARM64
if errorlevel 1 exit /b %errorlevel%
if not exist "artifacts\arm64\bin" mkdir "artifacts\arm64\bin"
copy /y "EqualizerAPO\ARM64\Release\EqualizerAPO.dll" "artifacts\arm64\bin\EqualizerAPO.dll" >nul

if exist build-control-arm64 rmdir /s /q build-control-arm64
mkdir build-control-arm64
pushd build-control-arm64
"%QTARM64_BIN%\qmake.exe" ..\DeviceSelector\DeviceSelector.pro CONFIG+=release
if errorlevel 1 (popd & exit /b %errorlevel%)
nmake
if errorlevel 1 (popd & exit /b %errorlevel%)
popd
copy /y "build-control-arm64\release\EqualizerAPO.exe" "artifacts\arm64\bin\EqualizerAPO.exe" >nul
if exist "artifacts\arm64\qt" rmdir /s /q "artifacts\arm64\qt"
"%QTARM64_BIN%\windeployqt.exe" --dir "artifacts\arm64\qt" --release --no-opengl-sw "artifacts\arm64\bin\EqualizerAPO.exe"
if errorlevel 1 exit /b %errorlevel%
"%nsis%" /DBINPATH="..\artifacts\arm64\bin" /DLIBPATH="..\artifacts\arm64\qt" /DINSTALLER_OUTFILE="EqualizerAPO-nightly-arm64.exe" Setup\SetupARM64.nsi
exit /b %errorlevel%

:build_x64
msbuild Common.vcxproj /m /t:Rebuild /p:Configuration=Release /p:Platform=x64 /p:EnableEnhancedInstructionSet=%2
if errorlevel 1 exit /b %errorlevel%
msbuild EqualizerAPO\EqualizerAPO.vcxproj /m /t:Rebuild /p:Configuration=Release /p:Platform=x64 /p:EnableEnhancedInstructionSet=%2
if errorlevel 1 exit /b %errorlevel%
copy /y "EqualizerAPO\x64\Release\EqualizerAPO.dll" "artifacts\%1\bin\EqualizerAPO.dll" >nul
exit /b %errorlevel%
