#!/bin/sh
echo
echo "============================================================================"
echo
echo "       This is a script of building wxWidgets-3.1.0 for LS-PrePost                "
echo
echo "============================================================================"
echo

echo -n "1. Do you want to build a release libs or debug libs(r/d)?"
read Release_Build
if [ "$Release_Build" = "d" ]; then
INSTALL_PATH=`cd ~ && pwd`
DES_PATH=$INSTALL_PATH/wx-3.1-debug
else
INSTALL_PATH=`cd ~ && pwd`
DES_PATH=$INSTALL_PATH/wx-3.1-release
fi
echo
echo "2. Please Enter wxWidgets install Path and Name"
echo "Default path and name is $DES_PATH. If you want this, just press Enter"
echo -n ":"
read INPUT_PATH
if [ -n "$INPUT_PATH" ]; then
INSTALL_PATH=$INPUT_PATH
fi

if [ -d $INSTALL_PATH ]; then
if [ "$Release_Build" = "d" ]; then
INSTALL_PATH=$INSTALL_PATH/wx-3.1-debug
else
INSTALL_PATH=$INSTALL_PATH/wx-3.1-release
fi
fi

GTK_VERSION=2
echo
echo "4. Do you want to build with gtk+-2.0 or gtk+-3.0?"
echo "If the system support gtk+-3.0, it is STRONGLY recommend to build with gtk+-3.0"
echo "Currently, OpenSuse12.1 or higher, Ubuntu 12.04 or higher support gtk+-3.0"
echo -n "GTK_VERSION=?(2/3):"
read INPUT_GTK_VERSION
if [ -n "$INPUT_GTK_VERSION" ]; then
GTK_VERSION=$INPUT_GTK_VERSION
fi

if [ "$Release_Build" = "d" ]; then
if [ -d debugbuild ]; then
rm -rf debugbuild
fi
echo
echo "============================================================================"
echo " You are ready to build wxWidgets-3.1.0                                     "
echo " Debug version                                                              "
echo " Install path: $INSTALL_PATH                                                "
echo " GTK Version is: ${GTK_VERSION}                                             "
echo " Press Enter to start or 'n' to stop                                        "
read START_BUILD
if [ "$START_BUILD" = "n" ]; then
exit 0
fi

echo "============================================================================"
echo
echo "                        Building is starting......                          "
echo
echo "============================================================================"
mkdir debugbuild
cd debugbuild
../configure --prefix=${INSTALL_PATH} --with-gtk=${GTK_VERSION} --enable-debug --disable-static --enable-shared --with-opengl --without-gnomeprint --disable-unicode --without-libnotify --disable-notifmsg --disable-mediactrl
else
if [ -d releasebuild ]; then
rm -rf releasebuild
fi

echo
echo "============================================================================"
echo " You are ready to build wxWidgets-3.1.0                                     "
echo " Release version                                                            "
echo " Install path: $INSTALL_PATH                                                "
echo " GTK Version is: ${GTK_VERSION}                                             "
echo " Press Enter to start or 'n' to stop                                        "
read START_BUILD
if [ "$START_BUILD" = "n" ]; then
exit 0
fi

echo "============================================================================"
echo
echo "                        Building is starting......                          "
echo
echo "============================================================================"
mkdir releasebuild
cd releasebuild
../configure --prefix=${INSTALL_PATH} --with-gtk=${GTK_VERSION} --disable-debug --enable-shared --disable-static --enable-optimize --with-opengl --without-gnomeprint --disable-unicode --without-libnotify --disable-notifmsg --disable-mediactrl
fi

make -j4 && make install
if [ "$Release_Build" = "d" ]; then
cp ../src/lstc/Makefile.debug ./Makefile.debug && make -f Makefile.debug
else
cp ../src/lstc/Makefile.release ./Makefile.release && make -f Makefile.release
fi
mkdir -p ${INSTALL_PATH}/include/wx-3.1/wx/lstc
cp ../include/wx/lstc/* ${INSTALL_PATH}/include/wx-3.1/wx/lstc
cp -d libwx_gtk2_lstc* ${INSTALL_PATH}/lib
cd ${INSTALL_PATH}/lib && ln -s libwx_gtk2_lstc-3.1.so.0.0.0 libwx_gtk2_lstc-3.1.so && ln -s libwx_gtk2_lstc-3.1.so.0.0.0 libwx_gtk2_lstc-3.1.so.0
echo "${INSTALL_PATH}/bin">~/.newwxpath
echo "============================================================================"
echo
echo "                        Building is finished.                               "
echo
echo "============================================================================"


exit 0
