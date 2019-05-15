#!/bin/sh
#wget https://github.com/nlohmann/json/archive/v3.6.1.tar.gz
#wget https://www.openssl.org/source/openssl-1.1.0j.tar.gz
#wget https://curl.haxx.se/download/curl-7.64.1.tar.gz 

tar zxf v3.6.1.tar.gz
tar zxf openssl-1.1.0j.tar.gz
tar zxf curl-7.64.1.tar.gz

rootpath=$(pwd)
opensslpath=${rootpath}/openssl-1.1.0j
curlpath=${rootpath}/curl-7.64.1

cd ${opensslpath}
mkdir -p ${opensslpath}/build
./config --prefix=${opensslpath}/build --openssldir=${opensslpath}/build
make
make install

cd ${curlpath}
mkdir -p ${curlpath}/build
export PKG_CONFIG_PATH=${opensslpath}/build/lib/pkgconfig
./configure --prefix=${curlpath}/build --with-ssl
make
make install

mkdir -p  ${rootpath}/pkg/
mv ${rootpath}/*.tar.gz ${rootpath}/pkg/

