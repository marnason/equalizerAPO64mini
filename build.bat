@echo off
setlocal

for /f "tokens=*" %%g in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -latest -products * -property installationPath') do set "vspath=%%g"
call "%vspath%\Common7\Tools\VsDevCmd.bat"

msbuild EqualizerAPO.sln /p:Configuration=Release /p:Platform=x64 /p:EnableEnhancedInstructionSet=AdvancedVectorExtensions2 /t:Rebuild /m
if errorlevel 1 goto done

msbuild EqualizerAPO.sln /p:Configuration=Release /p:Platform=ARM64 /t:Rebuild /m
if errorlevel 1 goto done

if not defined QTX64_BIN set "QTX64_BIN=C:\Qt\6.10.1\msvc2022_64\bin"
if not defined QTARM64_BIN set "QTARM64_BIN=C:\Qt\6.10.1\msvc2022_arm64\bin"

if defined ProgramFiles(x86) (
	set "nsis=%ProgramFiles(x86)%\NSIS\makensis.exe"
) else (
	set "nsis=%ProgramFiles%\NSIS\makensis.exe"
)

pushd Setup
if exist ..\lib64 rmdir /s /q ..\lib64
"%QTX64_BIN%\windeployqt.exe" --dir ..\lib64 --release --no-opengl-sw ..\x64\Release\DeviceSelector.exe
if errorlevel 1 (popd & goto done)
"%nsis%" Setup64.nsi
if errorlevel 1 (popd & goto done)
if exist ..\lib64 rmdir /s /q ..\lib64
"%QTARM64_BIN%\windeployqt.exe" --dir ..\lib64 --release --no-opengl-sw ..\ARM64\Release\DeviceSelector.exe
if errorlevel 1 (popd & goto done)
"%nsis%" SetupARM64.nsi
popd

:done
pause
