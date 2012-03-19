var uriparser = require("../lib/bindings.js");
var url = "/1.txt?test1=okok";
console.log(url, uriparser.parse(url));
