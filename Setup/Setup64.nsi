
!ifndef BINPATH
!define BINPATH "..\x64\Release"
!endif
!ifndef LIBPATH
!define LIBPATH "..\lib64"
!endif
!define VCREDIST_URL "https://aka.ms/vs/17/release/vc_redist.x64.exe"
!define TARGET_ARCH "x64"

!include "Setup.nsi"

!ifndef INSTALLER_OUTFILE
!define INSTALLER_OUTFILE "EqualizerAPO-x64-${VERSION}.exe"
!endif
OutFile "${INSTALLER_OUTFILE}"
