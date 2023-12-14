QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        #ica.cpp \
        #butterworthfilter.cpp \
        signaltoolbox.cpp \
        feedbackwin.cpp \
        firwin.cpp \
        main.cpp\
        mainwindow.cpp\
        simplebarfbwin.cpp \
        whitekf.cpp\
        cfir.cpp\
        utils.cpp\
        kalmanparamchoice.cpp\
        qcustomplot.cpp\
        signalplotwin.cpp\
        savedata.cpp\
        datareceiver.cpp\
        idataprocessor.cpp\
        savedata/fiff.cpp\
        savedata/fiff_id.cpp\
        savedata/fiff_info.cpp\
        savedata/fiff_info_base.cpp\
        savedata/fiff_ch_info.cpp\
        savedata/fiff_ch_pos.cpp\
        code_iir/IIR.cpp \
        code_iir/stdafx.cpp

HEADERS += \
    #ica.h \
    #butterworthfilter.h \
    constants.h \
    signaltoolbox.h \
    feedbackwin.h \
    firwin.h \
        mainwindow.h\
    simplebarfbwin.h \
    utils.h \
        whitekf.h\
        cfir.h\
        kalmanparamchoice.h\
        qcustomplot.h\
        signalplotwin.h\
        savedata.h\
        datareceiver.h\
        idataprocessor.h\
        savedata/fiff.h\
        savedata/fiff_id.h\
        savedata/fiff_types.h\
        savedata/fiff_constants.h\
        savedata/fiff_info.h\
        savedata/fiff_info_base.h\
        savedata/fiff_ch_info.h\
        savedata/fiff_ch_pos.h\
        code_iir/IIR.h \
        code_iir/stdafx.h


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/build-liblsl-master-Desktop_Qt_6_4_2_MinGW_64_bit-Debug/ -lliblsl.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/build-liblsl-master-Desktop_Qt_6_4_2_MinGW_64_bit-Debug/ -lliblsl.dll
else:unix: LIBS += -L$$PWD/build-liblsl-master-Desktop_Qt_6_4_2_MinGW_64_bit-Debug/ -lliblsl.dll

INCLUDEPATH += $$PWD/liblsl-master/include
DEPENDPATH += $$PWD/liblsl-master/include
