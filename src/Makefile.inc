libcurl_dir=$(SRC_ROOT)/third/curl-7.64.1/build/
openssl_dir=$(SRC_ROOT)/third/openssl-1.1.0j/build/
jsoncpp_dir=$(SRC_ROOT)/third/json-3.6.1/

#please include the root of lib path
LIB_PATHS += ${openssl_dir}/lib \
		${libcurl_dir}/lib 

#please include the root of include path
INCLUDES += ${openssl_dir}/include \
		${libcurl_dir}/include \
		${jsoncpp_dir}/include


#include all cpp files,must inlcude prefix $(SRC_ROOT)
SOURCES_DIR += 
