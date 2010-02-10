CONFIG -= qt stl exceptions rtti
CONFIG *= shared plugin thread warn_on
DEFINES -= DEBUG RELEASE NODEBUG NORELEASE

ENV_BUILD_CONFIG = $$(BUILD_CONFIG)
!isEmpty(ENV_BUILD_CONFIG) {
	CONFIG -= release debug profile
	CONFIG *= $$ENV_BUILD_CONFIG 
}

profile {
	DEFINES *= PROFILE
	CONFIG *= release
}

!release:!debug: CONFIG *= release

release: CONFIG -= debug

debug:DEFINES += DEBUG NORELEASE
release:DEFINES += RELEASE NODEBUG
unix:DEFINES += M_UNIX
macx:DEFINES += M_MAC
win32:DEFINES += M_WIN
!macx:unix: CONFIG += linux
linux:DEFINES += M_LINUX

!win32:VERSION = 0.1.0

unix:QMAKE_CXXFLAGS += -fno-strict-aliasing

