#!/bin/bash

{
cd $(dirname "$0")
current_path=$(pwd)
cd -
} &> /dev/null # disable output

echo ---------------------------------------
echo 解压源代码目录...
echo ---------------------------------------
source_dir=curl-7.63.0

if [ -d $source_dir ]; then
  rm -rf $source_dir
fi

tar xzf $current_path/curl-7.63.0.tar.gz
if [ $? != 0 ]; then
   exit 1
fi


echo ---------------------------------------
echo 编译libcurl...
echo ---------------------------------------
cd $source_dir

export MACOSX_DEPLOYMENT_TARGET="10.6"

# buid configure
./buildconf
./configure --prefix=$current_path/out \
            --disable-shared \
            --enable-static \
            --with-darwinssl \
            --enable-threaded-resolver \
            --disable-ldap \
            --disable-ldaps

# workaround still works though: make CFLAGS=-Wno-error for buid bug before v7.55.1
# the build error is: connectx' is only available on macOS 10.11 or newer           
#make CFLAGS=-Wno-error
make

# install
make install

echo "结果："
echo $current_path/out
