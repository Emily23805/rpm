#!/bin/sh

export CPPFLAGS
export CFLAGS
export LDFLAGS

echo "rpm 패키지 메니저를 설치하실 디렉터리를 골라주세요 ex) /root/rpm/bin"
read install

autoreconf -i

case "$1" in
  "--noconfigure")
    exit 0;
    ;;
  "--rpmconfigure")
    shift
    eval "`rpm --eval %configure`" "$@"
    ;;
  *)
    ./configure --prefix=$install "$@"
    ;;
esac
