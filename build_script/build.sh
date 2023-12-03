cd ~/src/notepad--/build-RealCompare-Desktop_x86_darwin_generic_mach_o_64bit-Debug/
/usr/local/opt/qt@5/bin/qmake ~/src/notepad--/src/RealCompare.pro -spec macx-clang  CONFIG+=debug CONFIG+=qml_debug
/usr/bin/make -f ~/src/notepad--/build-RealCompare-Desktop_x86_darwin_generic_mach_o_64bit-Debug/Makefile qmake_all
/usr/bin/make -j4