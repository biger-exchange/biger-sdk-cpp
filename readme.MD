
# 1. Here is the biger cpp sdk projects

## 1.1. 在centos7中搭建环境

## 1.2. 安装下载第三方库

* cd third  
* sh ./build.sh

## 1.3. 安装clang-format

* sudo yum install llvm-toolset-7-clang -y

## 1.4. 安装C++编译器

* sudo yum install centos-release-scl* -y
* sudo yum install devtoolset-7-gcc-c++ -y
* sudo yum install devtoolset-7-gcc devtoolset-7-gdb -y

## 1.5. 安装boost 1.69.0

* sudo yum install gcc-c++ -y
* sudo yum install libicu-devel -y
* cd ~ && tar zxf boost_1_69_0.tar.gz && cd boost_1_69_0
* sudo sh ./bootstrap.sh --prefix=/usr/local/
* sudo ./b2 install

## 1.6. 编译依赖

* sudo yum install gperftools-devel -y
* sudo yum install gperftools-libs -y
* sudo yum install openssl-devel -y
* sudo yum install cryptopp-devel -y
