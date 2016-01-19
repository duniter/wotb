
module.exports = new function() {

  var binding = require('bindings')('wotb');

  this.verify = binding.verify;
  this.getWoTSize = binding.getWoTSize;
  this.addNode = binding.addNode;
  this.isEnabled = binding.isEnabled;
  this.setEnabled = binding.setEnabled;
  this.existsLink = binding.existsLink;
  this.addLink = binding.addLink;
};
