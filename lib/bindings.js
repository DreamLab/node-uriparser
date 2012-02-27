/**
 * Compat for changes from node 0.4.x to 0.6.x.
 */
try {
  module.exports = require('../build/Release/uriparser.node');
} catch (e) { try {
  module.exports = require('../build/default/uriparser.node');
} catch (e) {
  throw e;
}}
