"use strict";

const binary = require('node-pre-gyp');
const path = require('path');
const binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
const binding = require(binding_path);

module.exports = {

  newFileInstance: (filePath) => {
    const instance = Object.create(WotB)
    if (process.platform == 'win32') {
      let bufferedPath = new Buffer(filePath,'ascii');
      instance.init(binding.newFileInstanceWin32(bufferedPath, bufferedPath.length))
    } else {
      instance.init(binding.newFileInstance(filePath));
    }
    instance.setFilePath(filePath)
    return instance
  },

  newMemoryInstance: () => {
    const instance = Object.create(WotB)
    const id = binding.newMemoryInstance()
    instance.init(id)
    return instance
  },

  setVerbose: (verbose) => binding.setVerbose(verbose)
};

const WotB = {

  instanceID: -1,
  filePath: "",

  init: function(instanceId){
    this.instanceID = instanceId
  },

  /**
   * Eventually has a file path if it is a file instance.
   * @param filePath
   */
  setFilePath: function(filePath){
    this.filePath = filePath
  },

  getId: function() { return this.instanceID },

  /**
   * Creates a copy of given instanceID as a new memory instance
   * @returns {WotB} A WotB wrapper.
   */
  memCopy: function() {
    const instance = Object.create(WotB)
    instance.init(binding.newMemoryInstance(this.instanceID))
    return instance
  },

  clear: function() {
    return binding.clearInstance(this.instanceID);
  },

  showWoT: function() {
    return binding.showWoT(this.instanceID);
  },

  dumpWoT: function() {
    return binding.dumpWoT(this.instanceID);
  },

  showGraph: function() {
    return binding.showGraph(this.instanceID);
  },

  resetWoT: function() {
    return binding.resetWoT(this.instanceID);
  },

  getWoTSize: function() {
    return binding.getWoTSize(this.instanceID);
  },

  addNode: function() {
    return binding.addNode(this.instanceID);
  },

  removeNode: function() {
    return binding.removeNode(this.instanceID);
  },

  setMaxCert: function(max) {
    return binding.setMaxCert(this.instanceID, max);
  },

  getMaxCert: function() {
    return binding.getMaxCert(this.instanceID);
  },

  isEnabled: function(node) {
    return binding.isEnabled(this.instanceID, node);
  },

  setEnabled: function(enabled, node) {
    return binding.setEnabled(this.instanceID, enabled, node);
  },

  existsLink: function(from, to) {
    return binding.existsLink(this.instanceID, from, to);
  },

  addLink: function(from, to) {
    return binding.addLink(this.instanceID, from, to);
  },

  removeLink: function(from, to) {
    return binding.removeLink(this.instanceID, from, to);
  },

  isOutdistanced: function(node, d_min, k_max, x_percent) {
    return binding.isOutdistanced(this.instanceID, node, d_min, k_max, x_percent);
  },

  detailedDistance: function(node, d_min, k_max, x_percent) {
    return binding.detailedDistance(this.instanceID, node, d_min, k_max, x_percent);
  },

  getSentries: function(d_min) {
    return binding.getSentries(this.instanceID, d_min);
  },

  getNonSentries: function(d_min) {
    return binding.getNonSentries(this.instanceID, d_min);
  },

  getDisabled: function() {
    return binding.getDisabled(this.instanceID);
  },

  getPaths: function(from, to, k_max) {
    return binding.getPaths(this.instanceID, from, to, k_max);
  }
}
