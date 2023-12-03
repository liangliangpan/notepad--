TEMPLATE = app
LANGUAGE = C++

TARGET = Notepad--

CONFIG	+= qt warn_on Release

QT += core gui widgets concurrent network macextras

HEADERS	+= *.h \
           cceditor/ccnotepad.h \
           cceditor/filemanager.h
		
SOURCES	+= *.cpp  \
           cceditor/ccnotepad.cpp \
           cceditor/filemanager.cpp
		
FORMS += *.ui \
         cceditor/ccnotepad.ui

RESOURCES += RealCompare.qrc

INCLUDEPATH	+= qscint/src
INCLUDEPATH	+= qscint/src/Qsci
INCLUDEPATH	+= qscint/scintilla/include
INCLUDEPATH += cceditor

DEFINES +=  QSCINTILLA_DLL

TRANSLATIONS += realcompare_zh.ts
	
win32 {
   if(contains(QMAKE_HOST.arch, x86_64)){
    CONFIG(Debug, Debug|Release){
        DESTDIR = x64/Debug
		LIBS	+= -Lx64/Debug
		LIBS += -lqmyedit_qt5d
    }else{
        DESTDIR = x64/Release
		LIBS	+= -Lx64/Release
		LIBS += -lqmyedit_qt5
    }
   }
}


win32{
	if(contains(QMAKE_HOST.arch, x86_64)){
		if(CONFIG(Debug, Debug|Release)){
			LIBS += -Llib64/Debug -llibprotobufd
		}else{
			LIBS += -Llib64/Release -llibprotobuf
		}
   }else{
		if(CONFIG(Debug, Debug|Release)){
			LIBS += -Llib32/Debug -llibprotobufd
		}else{
			LIBS += -Llib32/Release -llibprotobuf
		}
	}
}

unix{

    if(CONFIG(Debug, Debug|Release)){
              LIBS += -L/usr/local/Cellar/protobuf/25.1/lib/  -lprotobuf
              #LIBS += -L/Users/yinzuowei/work/CCNotePad/x64/Debug -lqmyedit_qt5_debug
              LIBS += -L/Users/mpan/src/notepad--/src/x64/Release -lqmyedit_qt5
    }else{
              LIBS += -L/usr/local/Cellar/protobuf/25.1/lib/ -lprotobuf
              LIBS += -L/Users/mpan/src/notepad--/src/x64/Release -lqmyedit_qt5
              DESTDIR = x64/Release
              #QMAKE_CXXFLAGS += -fopenmp -O2
              #LIBS += -lgomp -lpthread
    }
}


win32
{
INCLUDEPATH += f://protobuf-3.11.4/src
}

unix
{
INCLUDEPATH += /usr/local/Cellar/protobuf/25.1/include
}


RC_FILE += RealCompare.rc

ICON = mac.icns

DISTFILES += \
    RealCompare.rc

CONFIG+=sdk_no_version_check
