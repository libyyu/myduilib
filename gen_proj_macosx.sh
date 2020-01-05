self_path=$(cd `dirname $0`; pwd)
cd $self_path

platform=macosx
arch=x86_64
xmake f -c -p $platform -a $arch --genproj=y --boost_dir=/Volumes/SHARED/WorkSpace/boost_1_70_0
xmake project -k xcode -a $arch -m "release,debug" macosx