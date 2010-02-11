QMAKE_FEATURES *= $$PWD
QMAKE_LIBDIR *= $$DESTDIR
DEPENDPATH *= $$join(OURDIRS, " $$TWD/", "$$TWD/")
INCLUDEPATH *= $$join(OURDIRS, " $$TWD/", "$$TWD/")
contains(TARGET, $$basename(TWD)): contains(TEMPLATE, lib) {
	QMAKE_POST_LINK += echo $${TARGET} $$DEPS > $${DESTDIR}/$$basename(TWD).dep
	macx: QMAKE_LFLAGS += -install_name @rpath/$(TARGET)
	INSTALLS += target dep
	dep.files = $${DESTDIR}/$$basename(TWD).dep
	dep.path = $$PREFIX/share/martta/plugins
	target.path = $$PREFIX/share/martta/plugins
}
!contains(TARGET, $$basename(TWD)): !contains(NO_SOURCES, 1): LIBS *= -l$$basename(TWD)
for(a, DEPS): !contains(DONE, $${a}) {
	DONE += $${a}
	NO_SOURCES = 0
	include($${BASE}/$${a}/$${a}.pri)
}
win32:QMAKE_CXXFLAGS *= /wd4100 /wd4250 /wd4512 /wd4251 /wd4275 /wd4675 /wd4800 /wd4355 /wd4305

