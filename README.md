[![Build Status](https://travis-ci.org/DreamLab/node-uriparser.svg?branch=master)](https://travis-ci.org/DreamLab/node-uriparser) [![npm version](https://badge.fury.io/js/uriparser.svg)](https://badge.fury.io/js/uriparser) [![downloads badge](http://img.shields.io/npm/dm/uriparser.svg)](https://www.npmjs.org/package/uriparser)


# node-uriparser

## About:
Bindings for non RFC strict NGINX based parser (https://github.com/DreamLab/ngx_url_parser).

## How to build/install:
```
git submodule update --init --recursive
node-gyp configure build
```
or
```
npm install uriparser
```

## Requirements
Library (uriparser) requires following packages to be installed:
- automake
- autoconf
- libtool
- pkg-config

## API:
There is only one simple function: ````parse(url[, options);````.

Optional argument ````options```` lets you tell the library what your interests are, for example you can tell it to only give you the host.

Available options: ````PROTOCOL````, ````AUTH````, ````HOST````, ````PORT````, ````QUERY````, ````FRAGMENT````, ````PATH````, ````ALL````


## Example:
```js

const uriparser = require('uriparser');

const u = uriparser.parse('http://github.com');
console.log(u);
{ protocol: 'http:', host: 'github.com', path: '/' }

const q = uriparser.parse('https://github.com', uriparser.Uri.PROTOCOL);
console.log(q);
{ protocol: 'https:' }
```

## Benchmark:
Just run ``./benchmark.js``

## License:
```
Copyright (C) DreamLab Onet.pl Sp. z o. o.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
