#!/bin/bash -x


if [ -n "deps/ngx_url_parser/libngx_url_parser.a" ]; then
    cd deps/ngx_url_parser && make static-lib
    # cd deps/ngx_url_parser && cmake CMakeLists.txt -DBUILD_TEST=OFF && make
fi
