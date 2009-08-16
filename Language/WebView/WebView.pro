include(../martta.prf)
include(WebView.pri)
HEADERS +=  Interfaces/WebViewable.h Support/WebStylist.h Support/WebStylistRegistrar.h
unix:HEADERS +=  Interfaces/WebViewable.cpp Support/WebStylist.cpp Support/WebStylistRegistrar.cpp
SOURCES += .WebView-composed.cpp

