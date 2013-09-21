CONFIG      += designer plugin
QT          += xml sql qt3support
TEMPLATE    = lib
DESTDIR     = $$[QT_INSTALL_PLUGINS]/designer
INCLUDEPATH += /usr/include/Qt
HEADERS     = \
              QcjDataTable.h \
              QcjDataTablePlugin.h

SOURCES     = \
              QcjDataTable.cpp \
              QcjDataTablePlugin.cpp

#DataForm.h \
#              QcjDataXML.h \
#DataForm.cpp \
#              QcjDataXML.cpp \

DEFINES     += QT3_SUPPORT QT4_DESIGNER_PLUGIN

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = /usr/home/croftj/p4/Engineering/ltsmaint/QcjData
INSTALLS += target sources


