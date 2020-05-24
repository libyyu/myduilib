#!/bin/bash
ndk_dir=$1
ndk_root=$2
ndk_ver=$3
target_cpu=$4
echo ndk目录：$ndk_dir
echo ndk根目录：$ndk_root
echo ndk最低支持版本：$ndk_ver
echo cpu指令集：$target_cpu

cd $(dirname "$0")
work_dir=$(pwd)
cd -

ndk_toolchain_dir="$work_dir/ndk_toolchain"

echo 抽取NDK指令集目录
rm -rf $ndk_toolchain_dir
$ndk_dir/build/tools/make_standalone_toolchain.py --arch $target_cpu --api $ndk_ver --stl gnustl --install-dir=$ndk_toolchain_dir --force

echo 编译openssl
bash $work_dir/openssl/build_for_android.sh $ndk_root $ndk_toolchain_dir $target_cpu

echo 编译libcurl
bash $work_dir/libcurl/build_for_android.sh $ndk_toolchain_dir $work_dir/openssl $target_cpu

echo 删除NDK临时目录$ndk_toolchain_dir
rm -rf $ndk_toolchain_dir

echo 同步libcurl和openssl头文件
cp $work_dir/libcurl/out/$target_cpu/include/curl/*.h $work_dir/../
cp $work_dir/openssl/out/$target_cpu/include/openssl/*.h $work_dir/../openssl

