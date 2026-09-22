# EqualizerAPO64mini Simplification Plan

## Goal

Reduce this repository to a dependable Windows utility whose only audio-processing feature is adjusting the preamp gain of microphones and speakers.

The finished product should provide one minimal graphical interface that lists eligible input and output endpoints and allows an independent gain value to be configured for each device. Reliability and audio fidelity take priority over minimizing the final line count.

## Preserve

- Equalizer APO's proven Windows APO integration and real-time audio-processing path.
- Audio endpoint discovery, registration, installation, and removal.
- Child-APO compatibility and the safeguards that preserve existing device behavior.
- The existing preamp gain calculation and any supporting code required to apply it correctly.
- Configuration persistence, live configuration updates, and useful diagnostic logging.
- The installer and release pipeline for x64 AVX2, x64 AVX-512, x64 AVX10, and ARM64.
- Existing licensing, copyright notices, and attribution.

Prefer simplifying proven code over replacing it. Remove a component only after confirming that it is not required by the APO lifecycle, endpoint setup, preamp processing, diagnostics, or packaging.

## Remove or Replace

- Replace the current configuration editor and device selector with one small device-and-gain application.
- Remove every audio effect other than preamp gain, including parametric and graphic EQ, convolution, VST support, delays, routing, copying, channel selection, and loudness correction.
- Remove the general-purpose configuration language and features such as expressions, conditions, includes, stages, and filter parsing when the replacement gain configuration no longer needs them.
- Remove analysis tools, plots, benchmarks, the update checker, Voicemeeter-specific integration, and documentation for deleted features.
- Remove third-party libraries, build settings, projects, installer entries, and CI steps that are no longer used.
- Simplify the solution, installer, CI workflow, and README so they describe and build only the final preamp utility.

Do not add equalization, effects, routing, metering, or unrelated convenience features while carrying out this plan.

## Audio and Runtime Requirements

- A `0 dB` setting must be bit-transparent within the existing processing format.
- Negative and positive gain must follow Equalizer APO's existing conversion from decibels to linear gain.
- Apply a device's gain consistently to every channel and supported buffer layout for that endpoint.
- Keep the real-time callback free of memory allocation, blocking I/O, configuration parsing, and locks.
- Load and validate configuration outside the real-time path, then publish updates safely without interrupting audio.
- Treat missing, disabled, renamed, and disconnected endpoints as normal conditions and never destabilize the Windows audio service.
- Preserve safe installation, upgrade, rollback, and uninstall behavior throughout the simplification.

## Implementation Order

1. Characterize the current preamp output, endpoint installation behavior, configuration reload behavior, and supported build artifacts with tests.
2. Define a small per-endpoint configuration model keyed by stable endpoint identity, with a neutral `0 dB` default.
3. Reduce the audio engine to the preamp path while retaining required APO and child-APO behavior.
4. Build the minimal device-and-gain UI and move endpoint selection, installation state, and gain persistence into it.
5. Delete unused filters, parsers, applications, dependencies, resources, and documentation.
6. Simplify the Visual Studio solution, installer, CI workflow, and README.
7. Perform installation and live-audio regression testing on every retained architecture before declaring the work complete.

Keep the repository buildable and the installer testable at the end of each stage. Prefer small, reviewable changes over one deletion-heavy rewrite.

## Verification

- Test exact sample output at negative, zero, and positive dB values.
- Cover mono, stereo, and multichannel endpoints; interleaved and planar processing; in-place and separate buffers; silent buffers; and varying frame counts.
- Confirm that separate microphones and speakers retain and apply independent gain settings across application and system restarts.
- Confirm that changing gain takes effect without crashes, glitches, stale reads, partial updates, or real-time resource allocation.
- Test endpoint discovery and recovery when devices are added, removed, disabled, renamed, or unavailable.
- Test clean installation, endpoint enable/disable, upgrade, rollback, and uninstall paths without damaging unrelated audio configuration.
- Build and package x64 AVX2, x64 AVX-512, x64 AVX10, and ARM64 in CI.

## Completion Criteria

The simplification is complete only when:

- The installed product performs per-device microphone and speaker preamp adjustment and no other audio effect.
- The minimal UI is the only user-facing configuration application.
- Removed features have no remaining source files, projects, dependencies, installer entries, CI steps, or user documentation.
- All verification above passes on the supported architectures.
- Licensing and attribution remain correct.

