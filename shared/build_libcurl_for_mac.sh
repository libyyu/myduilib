#!/bin/bash
cd $(dirname "$0")
work_dir=$(pwd)
cd -

echo 编译libcurl
bash $work_dir/libcurl/build_for_mac.sh

echo 同步libcurl头文件
cp $work_dir/libcurl/out/$target_cpu/include/curl/*.h $work_dir/../