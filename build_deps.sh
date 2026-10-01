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

if [ ! -f deps/ngx_url_parser/autogen.sh ]; then
    if git rev-parse --git-dir > /dev/null 2>&1; then
        git submodule update --init --recursive deps/ngx_url_parser || exit 1
    else
        echo "deps/ngx_url_parser is missing. Run: git submodule update --init --recursive" >&2
        exit 1
    fi
fi

check_build_tools() {
    missing=""
    for tool in autoconf automake pkg-config make; do
        command -v $tool > /dev/null 2>&1 || missing="$missing $tool"
    done
    # libtool installs libtoolize (glibtoolize on macOS with Homebrew)
    if ! command -v libtoolize > /dev/null 2>&1 && ! command -v glibtoolize > /dev/null 2>&1; then
        missing="$missing libtool"
    fi

    if [ -n "$missing" ]; then
        echo "Missing build tools required for deps/ngx_url_parser:$missing" >&2
        echo "  Debian/Ubuntu: sudo apt-get install autoconf automake libtool pkg-config build-essential" >&2
        echo "  macOS:         brew install autoconf automake libtool pkg-config" >&2
        exit 1
    fi
}

if [ ! -f $NGXDIR/lib/libngx_url_parser.a ]; then
    check_build_tools
    mkdir -p $NGXDIR  && cd deps/ngx_url_parser && ./autogen.sh && ./configure --with-pic  --disable-shared --prefix=$NGXDIR && make clean install
fi
