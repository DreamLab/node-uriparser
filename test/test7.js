var uriparser = require("../lib/bindings.js");
var url = "http://eloszka.pl/hejka/?hejka=123";
console.log(url, uriparser.parse(url));
