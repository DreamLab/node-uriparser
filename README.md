##About:
Bindings for Uriparser (http://uriparser.sourceforge.net/), gives > 3x better performance that built-in url.parse

##How to build/install:
    git submodule update --init --recursive
    node-waf configure build
or
    npm install uriparser

##API:
There is only one simple function: ````parse(url[, options]);````.

Optional argument ````options```` lets you tell the library what your interests are, for example you can tell it to only give you the host.

Available options: ````kProtocol````, ````kAuth````, ````kHost````, ````kPort````, ````kQuery````, ````kFragment````, ````kPath````, ````kAll````

##Example:
```
var uriparser = require('uriparser');

var u = uriparser.parse('http://github.com');
console.log(u);
{ protocol: 'http:', host: 'github.com', path: '/' }

var q = uriparser.parse('https://github.com', uriparser.kProtocol);
console.log(q);
{ protocol: 'https:' }
```

##Benchmarks:
Just run ````time node examples/benchmark1.js```` and ````time node examples/benchmark2.js```` and compare the results.
