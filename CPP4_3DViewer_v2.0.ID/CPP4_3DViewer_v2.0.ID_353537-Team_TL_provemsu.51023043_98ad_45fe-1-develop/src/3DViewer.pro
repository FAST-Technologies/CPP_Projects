QT       += core gui concurrent widgets opengl openglwidgets

CONFIG += c++20

TARGET    = 3DViewer
TEMPLATE  = app

SOURCES  += main.cpp \
            glwidget/glwidget.cpp \
            objloader/objloader.cpp \
            affine_transformer/affine_transformer.cpp \
            controller/controller.cpp \
            model/model.cpp \
            parser/parser.cpp \
            command/command.cpp \
            view/view.cpp \

HEADERS  += glwidget/glwidget.h \
            objloader/objloader.h \
            affine_transformer/affine_transformer.h \
            controller/controller.h \
            model/model.h \
            parser/parser.h \
            geometry_types.h \
            user_action.h \
            view/view.h \
            integration_adapter.h \
            command/command.h \
            facade.h \

INCLUDEPATH += . \
               affine_transformer \
               command \
               controller \
               glwidget \
               model \
               objloader \
               parser \
               view \

include(3rdparty/qgifimage/qtgifimage.pri)

macx {
    GIFLIB_PREFIX = $$system(brew --prefix giflib)
    INCLUDEPATH += $$GIFLIB_PREFIX/include
    LIBS += -L$$GIFLIB_PREFIX/lib -lgif
}

unix:!macx {
    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib -lgif
}

QMAKE_CXXFLAGS += -g -Wall -Wextra -Werror

CONFIG += debug
