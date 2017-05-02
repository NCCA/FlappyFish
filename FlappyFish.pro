



TARGET = FlappyFish

OBJECTS_DIR = obj

QT+=gui opengl core



isEqual(QT_MAJOR_VERSION, 5) {
	cache()
	DEFINES +=QT5BUILD
}

linux*:INCLUDEPATH+=/public/devel/Box2D/include
linux*:LIBS+=-L/public/devel/Box2D/lib
LIBS+=-lBox2D


MOC_DIR=moc

CONFIG-=app_bundle

SOURCES+= $$PWD/src/*.cpp

HEADERS+= $$PWD/include/*.h

OTHER_FILES+= $$PWD/shaders/*.glsl

INCLUDEPATH +=./include

DESTDIR=./

CONFIG += console

!equals(PWD, $${OUT_PWD}){
        copydata.commands = echo "creating destination dirs" ;
        copydata.commands += mkdir -p $$OUT_PWD/shaders ;
	first.depends = $(first) copydata
	export(first.depends)
        export(copydata.commands)
	QMAKE_EXTRA_TARGETS += first copydata
}

QMAKE_CXXFLAGS_WARN_ON += "-Wno-unused-parameter"

QMAKE_CXXFLAGS+= -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64
macx:INCLUDEPATH+=/usr/local/include/
linux-g++:QMAKE_CXXFLAGS +=  -march=native
linux-g++-64:QMAKE_CXXFLAGS +=  -march=native

DEFINES +=NGL_DEBUG

unix:LIBS += -L/usr/local/lib -lboost_system
unix:LIBS +=  -L/$(HOME)/NGL/lib -l NGL

linux-*{
		linux-*:QMAKE_CXXFLAGS +=  -march=native
		DEFINES += LINUX
}
DEPENDPATH+=include
macx:DEFINES += DARWIN
INCLUDEPATH += $$(HOME)/NGL/include/










