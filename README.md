# EqualizerAPO64mini

EqualizerAPO64mini is a Windows Audio Processing Object derived from Equalizer APO. The repository is being reduced to a small utility that applies one independent preamp gain to each playback or capture endpoint.

This is an intermediate simplification milestone. The APO currently runs at a neutral `0 dB` gain and intentionally has no gain configuration application or configuration file. `DeviceSelector` is retained temporarily to install and remove the APO on Windows audio endpoints.

## Current behavior

- Preserves Equalizer APO's endpoint registration, installation, removal, child-APO compatibility, diagnostics, and real-time integration.
- Uses a bit-transparent bypass when the gain is `0 dB`.
- Contains the preamp processor and the existing `10^(dB/20)` gain conversion needed by the future configuration application.
- Does not provide EQ, convolution, VST hosting, routing, delays, channel selection, loudness correction, expressions, includes, stages, analysis, benchmarking, or update checking.
- Does not read or modify legacy Equalizer APO configuration files.

## Supported builds

The GitHub Actions workflow builds and packages:

- x64 AVX2
- x64 AVX-512
- x64 AVX10.1
- ARM64

The supported projects are `Common`, `EqualizerAPO`, and `DeviceSelector`. Visual Studio 2022, the Windows SDK, Qt 6.10.1, and NSIS are used by the build and packaging pipeline. For local installer builds, `build.bat` accepts `QTX64_BIN` and `QTARM64_BIN` pointing to the matching Qt `bin` directories.

## License

This project is derived from [Equalizer APO](https://sourceforge.net/projects/equalizerapo/) by Jonas Thedering. Its earlier double-precision work was inspired by [equalizer-apo-64](https://github.com/chebum/equalizer-apo-64). Existing copyright notices and the GPL license are retained; see [License.txt](License.txt).
