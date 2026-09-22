$ErrorActionPreference = "Stop"

$projectFiles = @("Common.vcxproj", "EqualizerAPO/EqualizerAPO.vcxproj", "DeviceSelector/DeviceSelector.vcxproj", "DeviceSelector/DeviceSelector.pro")
$projectText = ($projectFiles | ForEach-Object { Get-Content $_ -Raw }) -join "`n"

if ($projectText -match "\|Win32|v145|6\.7\.2|DeviceTest|ReceiveThread|OpacityIconEngine|AbstractAPOInfo|authz\.lib|crypt32\.lib|dbghelp\.lib") {
    throw "Unsupported platform, toolset, Qt, or deleted source references remain in project metadata."
}

$forbiddenFiles = @(
    "DeviceSelector/DeviceTestDialog.cpp",
    "DeviceSelector/DeviceTestThread.cpp",
    "DeviceSelector/ReceiveThread.cpp",
    "DeviceSelector/DeviceSelector.ui",
    "AbstractAPOInfo.cpp",
    "AbstractAPOInfo.h"
)
foreach ($file in $forbiddenFiles) {
    if (Test-Path $file) { throw "Obsolete file remains: $file" }
}

$sourceFiles = & git ls-files -- "*.cpp" "*.h" | Where-Object { Test-Path $_ }
$sourceText = ($sourceFiles | ForEach-Object { Get-Content $_ -Raw }) -join "`n"
$removedSymbols = @(
    "selectedInstallState",
    "hasChanges",
    "isExperimental",
    "getDeviceString",
    "getChannelCount",
    "getSampleRate",
    "getChannelMask",
    "readMultiValue",
    "readBinaryValue",
    "getFileAccessForUser",
    "splitQuoted"
)
foreach ($symbol in $removedSymbols) {
    if ($sourceText -match "\b$symbol\b") { throw "Removed source API remains: $symbol" }
}

$solution = Get-Content "EqualizerAPO.sln" -Raw
if ([regex]::Matches($solution, '(?m)^Project\(').Count -ne 3) {
    throw "The solution must contain exactly three projects."
}

Write-Host "Source and project metadata verification passed."
