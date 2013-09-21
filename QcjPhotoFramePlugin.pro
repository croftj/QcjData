CONFIG      += designer plugin
QT          += xml sql qt3support
TEMPLATE    = lib
DESTDIR     = $$[QT_INSTALL_PLUGINS]/designer
INCLUDEPATH += /usr/include/Qt
HEADERS     = QcjPhotoFrame.h \
              QcjPhotoFramePlugin.h

SOURCES     = QcjPhotoFrame.cpp \
              QcjPhotoFramePlugin.cpp

DEFINES     += QT3_SUPPORT QT4_DESIGNER_PLUGIN

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = /u1/src/k9kinship/QcjData
INSTALLS += target sources


