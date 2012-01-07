##About:
Wrapper for Uriparser (http://uriparser.sourceforge.net/), is 3 times fater that node url.parse

##How to build:
    git submodule update --init --recursive
    node-waf configure build

##Example:
See ````examples/test.js````

##Benchmarks:
Just run ````time node examples/benchmark1.js```` and ````time node examples/benchmark2.js```` to see the difference :)
