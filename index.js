"use strict";

var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var binding = require(binding_path);

module.exports = {

  newFileInstance: (filePath) =>
    new WotB(binding.newFileInstance(filePath)),

  newMemoryInstance: () =>
    new WotB(binding.newMemoryInstance()),

  setVerbose: (verbose) => binding.setVerbose(verbose)
};

function WotB(instanceID) {

  this.showWoT = () => {
    return binding.showWoT(instanceID);
  };

  this.showGraph = () => {
    return binding.showGraph(instanceID);
  };

  this.resetWoT = () => {
    return binding.resetWoT(instanceID);
  };

  this.getWoTSize = () => {
    return binding.getWoTSize(instanceID);
  };

  this.addNode = () => {
    return binding.addNode(instanceID);
  };

  this.removeNode = () => {
    return binding.removeNode(instanceID);
  };

  this.setMaxCert = (max) => {
    return binding.setMaxCert(instanceID, max);
  };

  this.getMaxCert = () => {
    return binding.getMaxCert(instanceID);
  };

  this.isEnabled = (node) => {
    return binding.isEnabled(instanceID, node);
  };

  this.setEnabled = (enabled, node) => {
    return binding.setEnabled(instanceID, enabled, node);
  };

  this.existsLink = (from, to) => {
    return binding.existsLink(instanceID, from, to);
  };

  this.addLink = (from, to) => {
    return binding.addLink(instanceID, from, to);
  };

  this.removeLink = (from, to) => {
    return binding.removeLink(instanceID, from, to);
  };

  this.isOutdistanced = (node, d_min, k_max, x_percent) => {
    return binding.isOutdistanced(instanceID, node, d_min, k_max, x_percent);
  };
}
