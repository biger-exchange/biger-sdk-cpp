#安装下载第三方库
cd third
sh ./build.sh

#for centos7
#安装clang-format
sudo yum install llvm-toolset-7-clang -y
#安装C++编译器
sudo yum install centos-release-scl* -y
sudo yum install devtoolset-7-gcc-c++ -y
sudo yum install devtoolset-7-gcc devtoolset-7-gdb -y
#安装boost 1.69.0
sudo yum install gcc-c++ -y
sudo yum install libicu-devel -y
cd ~ && tar zxf boost_1_69_0.tar.gz && cd boost_1_69_0
sudo sh ./bootstrap.sh --prefix=/usr/local/
sudo ./b2 install
#编译依赖
sudo yum install gperftools-devel -y
sudo yum install gperftools-libs -y
sudo yum install openssl-devel -y
sudo yum install cryptopp-devel -y

