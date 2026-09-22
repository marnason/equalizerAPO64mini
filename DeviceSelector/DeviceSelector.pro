#-------------------------------------------------
#
# EqualizerAPO control application qmake project for CI builds.
# The .vcxproj remains available for local Visual Studio development.
#
#-------------------------------------------------

QT += core gui widgets

TARGET = EqualizerAPO
TEMPLATE = app

DEFINES += WIN32
DEFINES += _UNICODE
QMAKE_CXXFLAGS_RELEASE += /O2

PRECOMPILED_HEADER = stdafx.h

SOURCES += \
	main.cpp \
	DeviceSelector.cpp \
	../helpers/ServiceHelper.cpp \
	stdafx.cpp

HEADERS += \
	DeviceSelector.h \
	../helpers/ServiceHelper.h \
	../helpers/ScopeGuard.h \
	resource.h \
	stdafx.h

RESOURCES += \
	DeviceSelector.qrc

# Include parent directory for shared headers
INCLUDEPATH += $$PWD/..

# Link against Common library and Windows libraries
LIBS += Kernel32.lib version.lib Shlwapi.lib user32.lib advapi32.lib

# Include Common.lib
LIBS += Common.lib
contains(QT_ARCH, arm64) {
	build_pass:CONFIG(debug, debug|release) {
		QMAKE_LIBDIR += "../ARM64/Debug"

	} else {
		QMAKE_LIBDIR += "../ARM64/Release"
	}
} else:contains(QT_ARCH, x86_64) {
	QMAKE_CXXFLAGS += /arch:AVX2
	build_pass:CONFIG(debug, debug|release) {
		QMAKE_LIBDIR += "../x64/Debug"

	} else {
		QMAKE_LIBDIR += "../x64/Release"
	}
}

# UAC: Require Administrator
QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

RC_FILE = DeviceSelector.rc
