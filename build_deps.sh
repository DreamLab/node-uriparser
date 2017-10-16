#!/bin/bash -x

BUILDDIR=$(dirname $0)

# bash readlink
# start 
# source http://stackoverflow.com/questions/1055671/how-can-i-get-the-behavior-of-gnus-readlink-f-on-a-mac
TARGET_FILE=$BUILDDIR

cd `dirname $TARGET_FILE`
TARGET_FILE=`basename $TARGET_FILE`

# Iterate down a (possible) chain of symlinks
while [ -L "$TARGET_FILE" ]
do
TARGET_FILE=`readlink $TARGET_FILE`
cd `dirname $TARGET_FILE`
    TARGET_FILE=`basename $TARGET_FILE`
done

# Compute the canonicalized name by finding the physical path 
# for the directory we're in and appending the target file.
PHYS_DIR=`pwd -P`
BUILDDIR="$PHYS_DIR/$TARGET_FILE/build"
# end

NGXDIR="$BUILDDIR/ngx_url_parser"

if [ ! -f $NGXDIR/lib/libngx_url_parser.a ]; then
    mkdir -p $NGXDIR  && cd deps/ngx_url_parser && ./autogen.sh && ./configure --with-pic  --disable-shared --prefix=$NGXDIR && make clean install
fi
