# Automated releases

There is no manual release checklist. Every push to `main` builds and verifies x64 AVX2, x64 AVX-512, x64 AVX10.1, and ARM64, inspects each installer payload, generates SHA-256 checksums, and publishes an immutable GitHub prerelease for that commit.

A failed build, runtime integration check, installer-content check, checksum step, or upload prevents publication. Pull requests run the build and verification jobs but do not publish a release.
