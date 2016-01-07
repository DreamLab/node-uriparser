#!/bin/bash -x

BUILDDIR=$(dirname $0)
BUILDDIR="$(readlink -f $BUILDDIR)/build"
URIDIR="$BUILDDIR/uriparser"
NGXDIR="$BUILDDIR/ngx_url_parser"

if [ ! -f $NGXDIR/lib/libngx_url_parser.a ]; then
    mkdir -p $NGXDIR  && cd deps/ngx_url_parser && ./autogen.sh && ./configure --with-pic  --disable-shared --prefix=$NGXDIR && make clean install
fi

cd -

if [ ! -f $URIDIR/lib/liburiparser.a ]; then
    mkdir -p $URIDIR && cd deps/uriparser && ./autogen.sh && ./configure --with-pic --disable-test --disable-doc --disable-shared --prefix=$URIDIR && make clean install
fi
