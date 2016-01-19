
module.exports = new function() {

  var binding = require('bindings')('wotb');

  this.verify = binding.verify;
  this.getWoTSize = binding.getWoTSize;
  this.addNode = binding.addNode;
};
