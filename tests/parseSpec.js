/*
 * Copyright (C) DreamLab Onet.pl Sp. z o. o.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the 'Software'), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
var uriparser = require('../bin/uriparser.node');

describe('uriparser - parse', function () {
    it('http://dreamlab.pl', function () {
        var url = uriparser.parse('http://dreamlab.pl');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('dreamlab.pl');
        expect(url.path).toBe('/');
    });

    it('http://dreamlab.pl/', function () {
        var url = uriparser.parse('http://dreamlab.pl/');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('dreamlab.pl');
        expect(url.path).toBe('/');
    });

    it('1.txt?test1=okok', function () {
        var url = uriparser.parse('1.txt?test1=okok');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.path).toBe('1.txt');
        expect(url.query).toEqual({'test1': 'okok'});
        expect(url.querySeparator).toBe('&');
    });

    it('/1.txt?test1=okok', function () {
        var url = uriparser.parse('/1.txt?test1=okok');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.path).toBe('/1.txt');
        expect(url.query).toEqual({'test1': 'okok'});
        expect(url.querySeparator).toBe('&');
    });

    it('http://dreamlab.pl/X?X', function () {
        var url = uriparser.parse('http://dreamlab.pl/X?X');

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('dreamlab.pl');
        expect(url.path).toBe('/X');
        expect(url.query).toEqual({'X': ""});
        expect(url.querySeparator).toBe('&');
    });

    it('http://dreamlab.pl/X', function () {
        var url = uriparser.parse('http://dreamlab.pl/X');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('dreamlab.pl');
        expect(url.path).toBe('/X');
    });

    it('http://dreamlab.pl/hejka/', function () {
        var url = uriparser.parse('http://dreamlab.pl/hejka/');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('dreamlab.pl');
        expect(url.path).toBe('/hejka/');
    });

    it('http://dreamlab.pl/hejka/?hejka=123', function () {
        var url = uriparser.parse('http://dreamlab.pl/hejka/?hejka=123');

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('dreamlab.pl');
        expect(url.path).toBe('/hejka/');
        expect(url.query).toEqual({'hejka': "123"})
        expect(url.querySeparator).toBe('&');
    });
    it('http://:@www.dreamlab.pl/?', function () {
        var url = uriparser.parse('http://:@www.dreamlab.pl/?');

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/');
        expect(url.querySeparator).toBe('&');
    });

    it('http://user:@www.dreamlab.pl/?', function () {
        var url = uriparser.parse('http://user:@www.dreamlab.pl/?');

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/');
        expect(url.querySeparator).toBe('&');
    });

    it('http://:pw@www.dreamlab.pl/?', function () {
        var url = uriparser.parse('http://:pw@www.dreamlab.pl/?');

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/');
    });

    it('http://user:pw@www.dreamlab.pl/?', function () {
        var url = uriparser.parse('http://user:pw@www.dreamlab.pl/?');

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.auth).toEqual({'user': "user", "password": "pw"});
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/');
        expect(url.querySeparator).toBe('&');
    });

    it('http://www.dreamlab.pl/?=&=', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/?=&=');

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/');
        expect(url.querySeparator).toBe('&');
    });

    it('http://www.dreamlab.pl//test/123', function () {
        var url = uriparser.parse('http://www.dreamlab.pl//test/123');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('//test/123');
    });

    it('http://www.dreamlab.pl/test//123', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test//123');

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test//123');
    });

    it('http://www.dreamlab.pl/test/?a=1&b=1&a=2&c=3', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test/?a=1&b=4&a=2&c=3');

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.query).toEqual({a: ['1', '2'], b: '4', c: '3'});
        expect(url.querySeparator).toBe('&');
    });

    it('http://www.dreamlab.pl/test/?b=1&c=3&a%5B%5D=1&a%5B%5D=1', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test/?b=1&c=3&a%5B%5D=1&a%5B%5D=2');

        expect(Object.keys(url).length).toEqual(6);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.query).toEqual({b: '1', c: '3', a: ['1', '2']});
        expect(url.queryArraySuffix).toEqual({a: '%5B%5D'});
        expect(url.querySeparator).toBe('&');
    });

    it('http://www.dreamlab.pl/test/test1/a[]=1&a[]=2', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test/test1/?a[]=1&a[]=2', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(6);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test/test1/');
        expect(url.query).toEqual({a: ['1', '2']});
        expect(url.queryArraySuffix).toEqual({a: '[]'});
        expect(url.querySeparator).toBe('&');
    });

    it('http://www.dreamlab.pl/test/test1/a[name]=1&a[name]=2', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test/test1/?a[name]=1&a[name]=2', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test/test1/');
        expect(url.query).toEqual({'a[name]': ['1', '2']});
        expect(url.querySeparator).toBe('&');
    });

    it('http://user:pw@www.dreamlab.pl/test/test1/a[name]=1&a[name]=2#fragment', function () {
        var url = uriparser.parse('http://user:pw@www.dreamlab.pl/test/test1/?a[name]=1&a[name]=2#fragment', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(7);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test/test1/');
        expect(url.query).toEqual({'a[name]': ['1', '2']});
        expect(url.auth).toEqual({'user': 'user', 'password': 'pw'});
        expect(url.fragment).toBe('fragment');
        expect(url.querySeparator).toBe('&');
    });

    it('http://www.dreamlab.pl/test/test1/?b=3&a=1&a=2', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test/test1/?b=3&a=1&a=2', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test/test1/');
        expect(url.query).toEqual({b:'3', a: ['1', '2']});
        expect(url.querySeparator).toBe('&');
    });

    it('http://dreamlab.pl:/', function () {
        var url = uriparser.parse('http://www.dreamlab.pl:/', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.port).toBe('');
    });

    it('http://dreamlab.pl:/ - only protocol', function () {
        var url = uriparser.parse('http://www.dreamlab.pl:/', uriparser.Uri.PROTOCOL, uriparser.Engines.NGINX);
        expect(Object.keys(url).length).toEqual(1);
        expect(url.protocol).toBe('http:');
    });

    it('http://dreamlab.pl:', function () {
        var url = uriparser.parse('http://www.dreamlab.pl:', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(4);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.port).toBe('');
    });

    it('http://dreamlab.pl:80/path/index.html?arg1=5', function () {
        var url = uriparser.parse('http://www.dreamlab.pl:80/path/index.html?arg1=5', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(6);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.port).toBe('80');
        expect(url.path).toBe('/path/index.html');
        expect(url.query).toEqual({arg1: '5'});
        expect(url.querySeparator).toBe('&');
    });

    it('http:////', function () {
        var url = uriparser.parse('http:////', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('http:');
        expect(url.path).toBe('//');
        expect(url.host).toBe('');
    });

    it('somestring', function () {
        var url = uriparser.parse('somestring', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(1);
        expect(url.path).toBe('somestring');
    });

    it('#fragmen/t', function () {
        var url = uriparser.parse('#fragmen/t', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(2);
        expect(url.fragment).toBe('fragmen/t');
    });

    it('?query', function () {
        var url = uriparser.parse('?query', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(3);
        expect(url.query).toEqual({'query': ''});
        expect(url.querySeparator).toBe('&');
    });

    it('https://user_usr:password@www.dreamlab.pl:81/path/index.html?arg1=1&arg2=2', function () {
        var url = uriparser.parse('https://user_usr:password@www.dreamlab.pl:81/path/index.html?arg1=1&arg2=2', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(7);
        expect(url.protocol).toBe('https:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.port).toBe('81');
        expect(url.path).toBe('/path/index.html');
        expect(url.auth).toEqual({user: 'user_usr', password: 'password'});
        expect(url.query).toEqual({arg1: '1', arg2: '2'});
        expect(url.querySeparator).toBe('&');
    });

    it('https://zs.usernanme_cms.test.api.pl/', function () {
        var url = uriparser.parse('https://zs.usernanme_cms.test.api.pl/', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(3);
        expect(url.protocol).toBe('https:');
        expect(url.host).toBe('zs.usernanme_cms.test.api.pl');
        expect(url.path).toBe('/');
    });

    it('http://www.dreamlab.pl/test/test1/?a=3;b=1;a=2', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test/test1/?a=3;b=1;a=2', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test/test1/');
        expect(url.query).toEqual({ a: ['3', '2'], b: '1'});
        expect(url.querySeparator).toBe(';');
    });

    it('http://www.dreamlab.pl/test/test1/?a', function () {
        var url = uriparser.parse('http://www.dreamlab.pl/test/test1/?a', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(5);
        expect(url.protocol).toBe('http:');
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test/test1/');
        expect(url.query).toEqual({ a: ''});
        expect(url.querySeparator).toBe('&');
    });

    it('//www.dreamlab.pl/test/test1/?a', function () {
        var url = uriparser.parse('//www.dreamlab.pl/test/test1/?a', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(4);
        expect(url.host).toBe('www.dreamlab.pl');
        expect(url.path).toBe('/test/test1/');
        expect(url.query).toEqual({ a: ''});
        expect(url.querySeparator).toBe('&');
    });

    it('/?a', function () {
        var url = uriparser.parse('/?a', null, uriparser.Engines.NGINX);

        expect(Object.keys(url).length).toEqual(3);
        expect(url.path).toBe('/');
        expect(url.query).toEqual({ a: ''});
        expect(url.querySeparator).toBe('&');
    });
});
