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
var uriparser = require("../bin/uriparser.node");


describe("uriparser - parse", function () {
    it("http://dreamlab.pl", function () {
        var url = uriparser.parse("http://dreamlab.pl");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("dreamlab.pl");
        expect(url.path).toEqual("/");
    });

    it("http://dreamlab.pl/", function () {
        var url = uriparser.parse("http://dreamlab.pl/");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("dreamlab.pl");
        expect(url.path).toEqual("/");
    });

    it("/1.txt?test1=okok", function () {
        var url = uriparser.parse("/1.txt?test1=okok");

        expect(Object.keys(url).length).toEqual(2);
        expect(url.path).toEqual("/1.txt");
        expect(url.query).toEqual({"test1": "okok"});
    });

    it("1.txt?test1=okok", function () {
        var url = uriparser.parse("1.txt?test1=okok");

        expect(Object.keys(url).length).toEqual(2);
        expect(url.path).toEqual("1.txt");
        expect(url.query).toEqual({"test1": "okok"});
    });

    it("http://dreamlab.pl/X?X", function () {
        var url = uriparser.parse("http://dreamlab.pl/X?X");

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("dreamlab.pl");
        expect(url.path).toEqual("/X");
        expect(url.query).toEqual({"X": ""});
    });

    it("http://dreamlab.pl/X", function () {
        var url = uriparser.parse("http://dreamlab.pl/X");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("dreamlab.pl");
        expect(url.path).toEqual("/X");
    });

    it("http://dreamlab.pl/hejka/", function () {
        var url = uriparser.parse("http://dreamlab.pl/hejka/");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("dreamlab.pl");
        expect(url.path).toEqual("/hejka/");
    });

    it("http://dreamlab.pl/hejka/?hejka=123", function () {
        var url = uriparser.parse("http://dreamlab.pl/hejka/?hejka=123");

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("dreamlab.pl");
        expect(url.path).toEqual("/hejka/");
        expect(url.query).toEqual({"hejka": "123"})
    });
    it("http://:@www.dreamlab.pl/?", function () {
        var url = uriparser.parse("http://:@www.dreamlab.pl/?");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.path).toEqual("/");
    });

    it("http://user:@www.dreamlab.pl/?", function () {
        var url = uriparser.parse("http://user:@www.dreamlab.pl/?");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.path).toEqual("/");
    });

    it("http://:pw@www.dreamlab.pl/?", function () {
        var url = uriparser.parse("http://:pw@www.dreamlab.pl/?");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.path).toEqual("/");
    });

    it("http://user:pw@www.dreamlab.pl/?", function () {
        var url = uriparser.parse("http://user:pw@www.dreamlab.pl/?");

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toEqual("http:");
        expect(url.auth).toEqual({"user": "user", "password": "pw"});
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.path).toEqual("/");
    });

    it("http://www.dreamlab.pl/?=&=", function () {
        var url = uriparser.parse("http://www.dreamlab.pl/?=&=");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.path).toEqual("/");
    });

    it("http://www.dreamlab.pl//test/123", function () {
        var url = uriparser.parse("http://www.dreamlab.pl//test/123");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.path).toEqual("//test/123");
    });

    it("http://www.dreamlab.pl/test//123", function () {
        var url = uriparser.parse("http://www.dreamlab.pl/test//123");

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.path).toEqual("/test//123");
    });

    it("http://www.dreamlab.pl/test/?a=1&b=1&a=2&c=3", function () {
        var url = uriparser.parse("http://www.dreamlab.pl/test/?a=1&b=4&a=2&c=3");

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.query).toEqual({a: ['1', '2'], b: '4', c: '3'});
    });

    it("http://www.dreamlab.pl/test/?a%5B%5D=1&b=1&a%5B%5D=2&c=3", function () {
        var url = uriparser.parse("http://www.dreamlab.pl/test/?a%5B%5D=1&b=1&a%5B%5D=2&c=3");

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toEqual("http:");
        expect(url.host).toEqual("www.dreamlab.pl");
        expect(url.query).toEqual({a: ['1', '2'], b: '1', c: '3'});
        expect(url.queryArraySuffix).toEqual({a: "%5B%5D"});
    });
});
