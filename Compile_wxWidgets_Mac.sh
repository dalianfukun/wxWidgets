#!/bin/sh
echo
echo "============================================================================"
echo
echo "       This is a script of building wxWidgets-3.1.0 for LS-PrePost    on Mac      "
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

if [ "$Release_Build" = "d" ]; then
if [ -d debugbuild ]; then
rm -rf debugbuild
fi
echo
echo "============================================================================"
echo " You are ready to build wxWidgets-3.1.0 for Mac                             "
echo " Debug version                                                              "
echo " Install path: $INSTALL_PATH                                                "
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
../configure --prefix=${INSTALL_PATH} CXX="clang++" CFLAGS="-arch x86_64" CXXFLAGS="-arch x86_64 -std=c++11 -stdlib=libc++" CPPFLAGS="-arch x86_64" LDFLAGS="-arch x86_64" OBJCFLAGS="-arch x86_64" OBJCXXFLAGS="-arch x86_64" --enable-debug --disable-shared --with-osx_cocoa --with-opengl --disable-unicode --with-macosx-version-min=10.9 --with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk --disable-mediactrl CPPFLAGS=-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=1
else
if [ -d releasebuild ]; then
rm -rf releasebuild
fi

echo
echo "============================================================================"
echo " You are ready to build wxWidgets-3.1.0 for Mac                             "
echo " Release version                                                            "
echo " Install path: $INSTALL_PATH                                                "
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
../configure --prefix=${INSTALL_PATH} CXX="clang++" CFLAGS="-arch x86_64" CXXFLAGS="-arch x86_64 -std=c++11 -stdlib=libc++" CPPFLAGS="-arch x86_64" LDFLAGS="-arch x86_64" OBJCFLAGS="-arch x86_64" OBJCXXFLAGS="-arch x86_64" --disable-debug --enable-optimize --disable-shared --with-osx_cocoa --with-opengl --disable-unicode --with-macosx-version-min=10.9 --with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk --disable-mediactrl CPPFLAGS=-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=1
fi

make -j8 && make install
echo "============================================================================"
echo
echo "                        Building is finished.                               "
echo
echo "============================================================================"


exit 0
