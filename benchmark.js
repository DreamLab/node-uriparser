#!/usr/bin/env node

/*
 * Copyright (C) DreamLab Onet.pl Sp. z o. o.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

var uriparser = require('./bin/uriparser');
var urlParse = require('url').parse;

var repeats = Number(process.argv[2]) || 2000000;
var urls = {
    'simple url': 'http://domain.eu/exmaple/path',
    'complex url': 'scheme://user:password@127.0.0.1:1337//p/../a/t/h/?egg=spam&foo=bar#fragment',
    'url with short query string': 'http://domain.eu?a=1&b=2&c=3',
    'url with long query string': 'http://domain.eu?a=1&b=2&c=3&d=4&e=5&f=6&g=7&h=8&1=a&2=b&3=c&4=d&5=e&6=f&7=g&h=8'
};

for (var name in urls) {
    var url = urls[name];

    console.log('%s (%s):', name, url);

    console.time('  url.parse');
    for (var i = 0; i < repeats; i++) {
        urlParse(url, true, false);
    }
    console.timeEnd('  url.parse');


    console.time('  uriparser (nginx)');
    for (var i = 0; i < repeats; i++) {
        uriparser.parse(url);
    }
    console.timeEnd('  uriparser (nginx)');

    console.log();
}
