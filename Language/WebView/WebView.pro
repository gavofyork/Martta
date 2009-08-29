include(../martta.prf)
include(WebView.pri)
HEADERS +=  Interfaces/WebViewable.h Interfaces/WebInformer.h Support/WebStylist.h Support/WebStylistRegistrar.h
unix:HEADERS +=  Interfaces/WebViewable.cpp Interfaces/WebInformer.cpp Support/WebStylist.cpp Support/WebStylistRegistrar.cpp
SOURCES += .WebView-composed.cpp
DEFINES += M_API_WebView=M_INAPI

