TARGET = bin/qtesseract
CONFIG += release \
    static
TEMPLATE = app
QT += opengl
# QMAKE_CXXFLAGS += -Wall

LIBS += -L/usr/local/lib \
    -ltesseract_api \
    -llept \
    -ltiff \
    -lpthread
DEPENDPATH = .
INCLUDEPATH = . \
    /usr/local/include/tesseract \
    /usr/local/include/liblept
MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rcc
PRECOMPILED_HEADER = baseapi.h \
    unicharset.h \
    rect.h \
    pageres.h
RESOURCES += qtesseract.qrc
SOURCES += source/myqmainwindow.cpp \
    source/main.cpp \
    source/myqgraphicsview.cpp \
    source/zoomarea.cpp \
    source/myqtextedit.cpp \
    source/myqtreewidget.cpp \
    source/tesstext.cpp \
    source/readerthread.cpp
HEADERS = include/myqmainwindow.h \
    include/myqgraphicsview.h \
    include/zoomarea.h \
    include/myqtextedit.h \
    include/myqtreewidget.h \
    include/tesstext.h \
    include/readerthread.h
OTHER_FILES += notas.txt
TRANSLATIONS = translations/qtesseract_es.ts
