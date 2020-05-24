#!/bin/bash
ndk_toolchain_dir=$1
openssl_dir=$2
target_cpu=$3
echo ndk_toolchain目录：$1
echo openssl依赖目录：$2
echo cpu指令集：$3

{
cd $(dirname "$0")
current_path=$(pwd)
cd -
} &> /dev/null # disable output

source_dir=curl-7.63.0

if [ -d $source_dir ]; then
  rm -rf $source_dir
fi

tar xzf $current_path/curl-7.63.0.tar.gz
if [ $? != 0 ]; then
   exit 1
fi

# 自己的android-toolchain(NDK针对特定配置抽取出来的独立目录)
export ANDROID_HOME=$ndk_toolchain_dir

# openssl的输出目录
export CFLAGS="-isystem$openssl_dir/out/$target_cpu/include"
export LDFLAGS="-L$openssl_dir/out/$target_cpu/lib"
export TOOLCHAIN=$ANDROID_HOME/bin

# 设置目标机器指令集
arch_flags="-march=i686 -msse3 -mstackrealign -mfpmath=sse"
arch=arch-x86
tool_target=i686-linux-android
host_os=i686-linux-android
if [ $target_cpu == "x86" ]; then
    arch_flags="-march=i686 -msse3 -mstackrealign -mfpmath=sse"
    arch=arch-x86
    tool_target=i686-linux-android
    host_os=i686-linux-android
fi
if [ $target_cpu == "arm" ]; then
    arch_flags="-march=armv7-a -msse3 -mstackrealign -mfpmath=sse"
    arch=arch-arm
    tool_target=arm-linux-androideabi
    host_os=arm-androideabi-linux
fi
if [ $target_cpu == "arm64" ]; then
    arch_flags="-march=armv8 -msse3 -mstackrealign -mfpmath=sse"
    arch=arch-arm
    tool_target=arm-linux-androideabi
    host_os=arm-androideabi-linux
fi
echo 当前CPU指令集匹配arch为"$arch"，arch_flags为$arch_flags
export TOOL=$tool_target 
export ARCH_FLAGS=$arch_flags
export ARCH=$arch

export CC=$TOOLCHAIN/$TOOL-gcc
export CXX=$TOOLCHAIN/${TOOL}-g++
export LINK=${CXX}
export LD=$TOOLCHAIN/${TOOL}-ld
export AR=$TOOLCHAIN/${TOOL}-ar
export RANLIB=$TOOLCHAIN/${TOOL}-ranlib
export STRIP=$TOOLCHAIN/${TOOL}-strip


export CPPFLAGS="-DANDROID -D__ANDROID_API__=18"
export LIBS="-lssl -lcrypto"
export CROSS_SYSROOT=$TOOLCHAIN/sysroot

cd $source_dir

./configure --prefix=$current_path/out/$target_cpu \
            --exec-prefix=$current_path/out/$target_cpu \
            --bindir=$TOOLCHAIN \
            --sbindir=$TOOLCHAIN \
            --libexecdir=$TOOLCHAIN \
            --with-sysroot=$CROSS_SYSROOT \
            --host=$host_os \
            --enable-ipv6 \
            --enable-threaded-resolver \
            --disable-dict \
            --disable-gopher \
            --disable-ldap \
            --disable-ldaps \
            --disable-manual \
            --disable-pop3 \
            --disable-smtp \
            --disable-imap \
            --disable-rtsp \
            --disable-smb \
            --disable-telnet \
            --disable-verbose

make install

echo "结果："
echo $current_path/out/$target_cpu
