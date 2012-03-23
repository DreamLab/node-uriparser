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

var uriparser = require("../build/default/uriparser");
var assert = require('assert');
var undef;

var url = "http://eloszka.pl";
url = uriparser.parse(url);
console.log(url);
assert.equal(url.protocol, "http:");
assert.equal(url.auth, undef);
assert.equal(url.host, "eloszka.pl");
assert.equal(url.port, undef);
assert.equal(url.query, undef);
assert.equal(url.fragment, undef);
assert.equal(url.path, "/");

var url = "http://eloszka.pl/";
url = uriparser.parse(url);
console.log(url);
assert.equal(url.protocol, "http:");
assert.equal(url.auth, undef);
assert.equal(url.host, "eloszka.pl");
assert.equal(url.port, undef);
assert.equal(url.query, undef);
assert.equal(url.fragment, undef);
assert.equal(url.path, "/");

var url = "/1.txt?test1=okok";
url = uriparser.parse(url);
console.log(url);
assert.equal(url.protocol, undef);
assert.equal(url.auth, undef);
assert.equal(url.host, undef);
assert.equal(url.port, undef);
assert.deepEqual(url.query, {test1: "okok"});
assert.equal(url.fragment, undef);
assert.equal(url.path, "/1.txt");
