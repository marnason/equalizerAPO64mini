# Release checklist

1. Update `version.h`.
2. Confirm the GitHub Actions build succeeds for x64 AVX2, x64 AVX-512, x64 AVX10.1, and ARM64.
3. Smoke-test endpoint installation, audio playback/capture, and uninstallation on Windows.
4. Confirm each installer contains only the APO, DeviceSelector, the required Qt runtime, licensing, and the uninstaller.
5. Publish the four architecture-specific installers and corresponding source archive.
