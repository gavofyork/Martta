QMAKE_FEATURES *= $$PWD
QMAKE_LIBDIR *= $$DESTDIR
DEPENDPATH *= $$join(OURDIRS, " $$TWD/", "$$TWD/")
INCLUDEPATH *= $$join(OURDIRS, " $$TWD/", "$$TWD/")
contains(TARGET, $$basename(TWD)): contains(TEMPLATE, lib): QMAKE_POST_LINK += echo $${TARGET} $$DEPS > $${DESTDIR}/$(TARGET).dep
!contains(TARGET, $$basename(TWD)): !contains(NO_SOURCES, 1): LIBS *= -l$$basename(TWD)
for(a, DEPS): !contains(DONE, $${a}) {
	DONE += $${a}
	NO_SOURCES = 0
	include($${BASE}/$${a}/$${a}.pri)
}
