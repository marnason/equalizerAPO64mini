$ErrorActionPreference = "Stop"
$temporary = Join-Path $env:RUNNER_TEMP "equalizerapo-runtime-check"
New-Item -ItemType Directory -Force $temporary | Out-Null
$source = Join-Path $temporary "runtime-check.cpp"

@'
#include "PreampProcessor.h"
#include "EndpointGainStore.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <windows.h>

int main()
{
    PreampProcessor processor;
    float input[] = { 0.25f, -0.5f, 1.0f, -1.0f };
    float output[4] = {};
    processor.initialize(2, 2, 48000, 0.0);
    processor.process(output, input, 2);
    if (std::memcmp(input, output, sizeof(input)) != 0) return 1;
    processor.process(input, input, 2);
    if (input[0] != 0.25f || input[1] != -0.5f) return 2;

    PreampProcessor mono;
    float monoInput[] = { 0.5f, -0.5f };
    float stereoOutput[4] = {};
    mono.initialize(1, 2, 48000, -6.0);
    mono.process(stereoOutput, monoInput, 2);
    if (stereoOutput[0] != stereoOutput[1] || stereoOutput[2] != stereoOutput[3]) return 3;

    PreampProcessor multichannel;
    float surroundInput[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    float surroundOutput[6] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    multichannel.initialize(6, 6, 48000, 12.0);
    multichannel.process(surroundOutput, surroundInput, 1);
    for (float sample : surroundOutput) if (sample != 0.0f) return 4;

    processor.setGainDb(6.0);
    std::vector<float> rampInput(480 * 2, 1.0f), rampOutput(480 * 2);
    processor.process(rampOutput.data(), rampInput.data(), 480);
    const float expected = static_cast<float>(std::pow(10.0, 6.0 / 20.0));
    if (std::abs(rampOutput.back() - expected) > 0.00001f) return 5;

    if (!EndpointGainStore::isValid(-60000) || !EndpointGainStore::isValid(30000)
        || EndpointGainStore::isValid(-60001) || EndpointGainStore::isValid(30001)) return 6;
    const std::wstring key = L"{00000000-0000-0000-0000-000000000001}";
    EndpointGainStore::writeGainMilliDb(key, -12300);
    if (EndpointGainStore::readGainMilliDb(key) != -12300) return 7;
    RegDeleteTreeW(HKEY_LOCAL_MACHINE, (std::wstring(EndpointGainStore::REGISTRY_PATH) + L"\\" + key).c_str());
    return 0;
}
'@ | Set-Content $source

& cl.exe /nologo /EHsc /std:c++17 /I. PreampProcessor.cpp EndpointGainStore.cpp stdafx.cpp $source /Fe:"$temporary\runtime-check.exe" /link advapi32.lib
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& "$temporary\runtime-check.exe"
if ($LASTEXITCODE -ne 0) { throw "Runtime integration check failed with exit code $LASTEXITCODE" }
Write-Host "Runtime integration verification passed."
