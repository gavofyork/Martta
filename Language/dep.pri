QMAKE_FEATURES *= $$PWD
QMAKE_LIBDIR *= $$DESTDIR
DEPENDPATH *= $$join(OURDIRS, " $$TWD/", "$$TWD/")
INCLUDEPATH *= $$join(OURDIRS, " $$TWD/", "$$TWD/")
contains(TARGET, $$basename(TWD)): contains(TEMPLATE, lib) {
	QMAKE_POST_LINK += echo $${TARGET} $$DEPS > $${DESTDIR}/$(TARGET).dep
	macx: QMAKE_LFLAGS += -install_name @rpath/$(TARGET)
}
!contains(TARGET, $$basename(TWD)): !contains(NO_SOURCES, 1) {
	LIBS *= -l$$basename(TWD)
	INSTALLS = target deps
	target.path = $$PREFIX/share/martta/plugins
	deps.path = $$PREFIX/share/martta/plugins
	deps.files = $${DESTDIR}/$(TARGET).dep
}
for(a, DEPS): !contains(DONE, $${a}) {
	DONE += $${a}
	NO_SOURCES = 0
	include($${BASE}/$${a}/$${a}.pri)
}
