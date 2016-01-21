
module.exports = new function() {

  var binding = require('bindings')('wotb');

  this.plugOnFile = binding.plugOnFile;
  this.resetWoT = binding.resetWoT;

  this.getWoTSize = binding.getWoTSize;
  this.addNode = binding.addNode;
  this.removeNode = binding.removeNode;
  this.isEnabled = binding.isEnabled;
  this.setEnabled = binding.setEnabled;
  this.existsLink = binding.existsLink;
  this.addLink = binding.addLink;
  this.removeLink = binding.removeLink;
  this.isOutdistanced = binding.isOutdistanced;
};
