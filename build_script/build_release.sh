cd ~/src/notepad--/build-RealCompare-Desktop_x86_darwin_generic_mach_o_64bit-Release/
/usr/local/opt/qt@5/bin/qmake ~/src/notepad--/src/RealCompare.pro -spec macx-clang CONFIG+=release
/usr/bin/make -f ~/src/notepad--/build-RealCompare-Desktop_x86_darwin_generic_mach_o_64bit-Release/Makefile qmake_all
/usr/bin/make -j4
cp -r ~/src/notepad--/build-RealCompare-Desktop_x86_darwin_generic_mach_o_64bit-Release/x64/Release/Notepad--.app ~/Applications/