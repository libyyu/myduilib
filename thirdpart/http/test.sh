
CURL_PATH=../../3rd/curl
#./configure  --disable-shared --enable-static --without-zlib --without-libidn --without-ssl --without-librtmp --without-gnutls --without-nss --without-libssh2 --without-winidn --disable-rtsp --disable-ldap --disable-ldaps --disable-ipv6
MODULE_NAME=HTTP
g++ -c \
	-I./ \
	-std=c++11 -static \
	HTTPHelper.cpp \
	-DCURL_STATICLIB
ar cr lib${MODULE_NAME}.a HTTPHelper.o #/usr/local/lib/libcurl.a

MODULE_NAME2=test
g++ \
	-I./ \
	-L./ \
	Test.cpp \
	-o ${MODULE_NAME2} -std=c++11 \
	-DCURL_STATICLIB \
	-l${MODULE_NAME} -lldap -lcurl #-lcrypto