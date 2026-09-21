
!ifndef BINPATH
!define BINPATH "..\ARM64\Release"
!endif
!ifndef LIBPATH
!define LIBPATH "..\lib64"
!endif
!define VCREDIST_URL "https://aka.ms/vs/17/release/vc_redist.arm64.exe"
!define TARGET_ARCH "ARM64"

!include "Setup.nsi"

!ifndef INSTALLER_OUTFILE
!define INSTALLER_OUTFILE "EqualizerAPO-ARM64-${VERSION}.exe"
!endif
OutFile "${INSTALLER_OUTFILE}"
