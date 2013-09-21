CONFIG      += designer plugin
QT          += xml sql qt3support
TEMPLATE    = lib
DESTDIR     = $$[QT_INSTALL_PLUGINS]/designer
INCLUDEPATH += /usr/include/Qt
HEADERS     = QcjDataForm.h \
              QcjDataFormPlugin.h

SOURCES     = QcjDataForm.cpp \
              QcjDataFormPlugin.cpp

DEFINES     += QT3_SUPPORT QT4_DESIGNER_PLUGIN

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = /usr/home/croftj/p4/Engineering/ltsmaint/QcjData
INSTALLS += target sources

