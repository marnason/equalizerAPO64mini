# EqualizerAPO64mini

EqualizerAPO64mini is a focused Windows Audio Processing Object derived from Equalizer APO. Its only audio effect is an independent preamp gain for each playback and capture endpoint.

The control application lists available endpoints, installs or removes the APO, and stores a gain from `-60.0 dB` to `+30.0 dB` in `0.1 dB` steps. Gain changes are published to the running APO without restarting the Windows audio service. Installation changes are applied explicitly and restart the service when possible.

## Audio behavior

- Uses Equalizer APO's established endpoint registration, child-APO integration, and real-time callback path.
- Applies the configured gain to every interleaved channel using `10^(dB/20)`.
- Ramps live changes over 10 ms to avoid discontinuities.
- Bypasses multiplication at a stable `0 dB` and copies only when input and output buffers differ.
- Performs no allocation, locking, registry access, parsing, or I/O in `APOProcess`.
- Treats missing or invalid settings as `0 dB`.
- Does not read, migrate, overwrite, or delete Equalizer APO's legacy `config.txt` or configuration directory.

The product contains no equalization, convolution, VST hosting, routing, delay, channel selection, loudness correction, expression language, analysis tool, benchmark, or update checker.

## Supported builds

GitHub Actions builds and packages four Windows variants:

- x64 AVX2
- x64 AVX-512
- x64 AVX10.1
- ARM64

The solution contains Common, EqualizerAPO, and EqualizerAPO Control. Local builds require Visual Studio 2022 with the `v143` toolset, a Windows SDK, Qt 6.10.1, and NSIS. `build.bat` creates all four installers; `QTX64_BIN` and `QTARM64_BIN` can override the corresponding Qt `bin` directories.

Every successful push to `main` publishes an immutable dated prerelease with all four installers and SHA-256 checksums. Pull requests build and verify the code without publishing a release.

## License

This project is derived from [Equalizer APO](https://sourceforge.net/projects/equalizerapo/) by Jonas Thedering. Its earlier double-precision work was inspired by [equalizer-apo-64](https://github.com/chebum/equalizer-apo-64). Existing copyright notices and the GPL license are retained; see [License.txt](License.txt).
