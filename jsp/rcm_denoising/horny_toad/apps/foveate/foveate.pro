# Foveate Project File
#
# Copyright (C) 2008
# Jeffrey S Perry
#
# jsp Thu Feb 28 15:59:32 CST 2008

TEMPLATE = app
TARGET = foveate
CONFIG += warn_on
DEPENDPATH += .
INCLUDEPATH += . \
    .. \
    ../..\
    ../../../jack_rabbit
HEADERS += \
    foveate_main_window.h \
    foveate_scene.h \
    foveate_view.h \
    foveate_exception_enabled_app.h
FORMS += foveate_main_window.ui
FORMS += foveate_export_dialog.ui
SOURCES += foveate.cc
