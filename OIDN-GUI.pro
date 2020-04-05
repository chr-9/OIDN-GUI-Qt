QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    sequenceParser/FrameRange.cpp \
    sequenceParser/Item.cpp \
    sequenceParser/ItemStat.cpp \
    sequenceParser/Sequence.cpp \
    sequenceParser/detail/FileNumbers.cpp \
    sequenceParser/detail/FileStrings.cpp \
    sequenceParser/detail/analyze.cpp \
    sequenceParser/detector.cpp \
    sequenceParser/filesystem.cpp \
    sequenceParser/utils.cpp

HEADERS += \
    imagebuffer.h \
    mainwindow.h \
    sequenceParser/FrameRange.hpp \
    sequenceParser/FrameRange.i \
    sequenceParser/Item.hpp \
    sequenceParser/Item.i \
    sequenceParser/ItemStat.hpp \
    sequenceParser/ItemStat.i \
    sequenceParser/Sequence.hpp \
    sequenceParser/Sequence.i \
    sequenceParser/Sequence.tcc \
    sequenceParser/common.hpp \
    sequenceParser/common.i \
    sequenceParser/detail/FileNumbers.hpp \
    sequenceParser/detail/FileStrings.hpp \
    sequenceParser/detail/analyze.hpp \
    sequenceParser/detector.hpp \
    sequenceParser/detector.i \
    sequenceParser/filesystem.hpp \
    sequenceParser/filesystem.i \
    sequenceParser/system.hpp \
    sequenceParser/utils.hpp


FORMS += \
    mainwindow.ui

LIBS += \
    -lboost_system\
    -lboost_regex\
    -lboost_filesystem\
    -lboost_locale\
    -lIlmImf\
    -L/usr/local/lib -lOpenImageDenoise

CONFIG += staticLib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    sequenceParser/sequenceParser.i
