param(
    [Parameter(Mandatory = $true)][string]$Installer,
    [Parameter(Mandatory = $true)][string]$Variant
)

$ErrorActionPreference = "Stop"
if (!(Test-Path $Installer)) { throw "Installer not found: $Installer" }

$listing = & 7z l $Installer | Out-String
if ($LASTEXITCODE -ne 0) { throw "Unable to inspect $Installer" }

foreach ($required in @("EqualizerAPO.dll", "EqualizerAPO.exe", "License.txt", "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "qwindows.dll")) {
    if ($listing -notmatch [regex]::Escape($required)) { throw "Installer is missing $required" }
}
foreach ($forbidden in @("DeviceSelector.exe", "Editor.exe", "Benchmark.exe", "UpdateChecker.exe", "VoicemeeterClient.exe", "libfftw", "sndfile", "config.txt")) {
    if ($listing -match [regex]::Escape($forbidden)) { throw "Installer contains obsolete payload: $forbidden" }
}

$allowedPayload = @(
    '^\$INSTDIR$', '^(\$INSTDIR[\\/])?EqualizerAPO\.dll$', '^(\$INSTDIR[\\/])?EqualizerAPO\.exe$',
    '^(\$INSTDIR[\\/])?Qt6(Core|Gui|Widgets)\.dll$', '^(\$INSTDIR[\\/])?qt([\\/](platforms)?)?$',
    '^(\$INSTDIR[\\/])?qt[\\/]platforms[\\/]qwindows\.dll$', '^(\$INSTDIR[\\/])?Uninstall\.exe$',
    '^(\$INSTDIR[\\/])?License\.txt$',
    '^\$PLUGINSDIR([\\/].*)?$', '^\[NSIS\]\.nsi$'
)
$listedPaths = [regex]::Matches($listing, '(?m)^Path = (.+)$') | ForEach-Object { $_.Groups[1].Value.Trim() }
foreach ($path in $listedPaths) {
    if ([IO.Path]::GetFileName($path) -eq (Split-Path $Installer -Leaf)) { continue }
    if (!($allowedPayload | Where-Object { $path -match $_ })) {
        throw "Installer contains a file outside the payload allowlist: $path"
    }
}

Write-Host "Installer payload verification passed for $Variant."
