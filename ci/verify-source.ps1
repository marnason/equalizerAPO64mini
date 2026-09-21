$ErrorActionPreference = "Stop"

$projectFiles = @("Common.vcxproj", "EqualizerAPO/EqualizerAPO.vcxproj", "DeviceSelector/DeviceSelector.vcxproj")
$projectText = ($projectFiles | ForEach-Object { Get-Content $_ -Raw }) -join "`n"

if ($projectText -match "\|Win32|v145|6\.7\.2|DeviceTest|ReceiveThread|OpacityIconEngine") {
    throw "Unsupported platform, toolset, Qt, or deleted source references remain in project metadata."
}

$forbiddenFiles = @(
    "DeviceSelector/DeviceTestDialog.cpp",
    "DeviceSelector/DeviceTestThread.cpp",
    "DeviceSelector/ReceiveThread.cpp",
    "DeviceSelector/DeviceSelector.ui"
)
foreach ($file in $forbiddenFiles) {
    if (Test-Path $file) { throw "Obsolete file remains: $file" }
}

$solution = Get-Content "EqualizerAPO.sln" -Raw
if ([regex]::Matches($solution, '(?m)^Project\(').Count -ne 3) {
    throw "The solution must contain exactly three projects."
}

Write-Host "Source and project metadata verification passed."
