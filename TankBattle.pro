QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = TankBattle
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamescene.cpp \
    startscene.cpp \
    tank.cpp \
    playertank.cpp \
    enemytank.cpp \
    bullet.cpp \
    wall.cpp \
    base.cpp \
    gamemanager.cpp

HEADERS += \
    constants.h \
    mapdata.h \
    mainwindow.h \
    gamescene.h \
    startscene.h \
    tank.h \
    playertank.h \
    enemytank.h \
    bullet.h \
    wall.h \
    base.h \
    gamemanager.h

# Windows 下可能需要
# windows {
#     RC_FILE = app.rc
# }
