"use strict";

var addon  = require('./../index');
var should = require('should');
var path = require('path');
var fs = require('fs');

const TEST_FILE_PATH = path.join(__dirname, 'wot.bin');

describe('Crypto', function() {

  before(() => {
    if (fs.existsSync(TEST_FILE_PATH)) {
      fs.unlinkSync(TEST_FILE_PATH);
    }
    return Promise.resolve();
  });

  it('should have an initial size of 0', function() {
    should.equal(addon.getWoTSize(TEST_FILE_PATH), 0);
  });

  it('should give number 0 if we add a node', function() {
    // Add a node
    should.equal(addon.addNode(TEST_FILE_PATH), 0);
    should.equal(addon.getWoTSize(TEST_FILE_PATH), 1);
    // Add another
    should.equal(addon.addNode(TEST_FILE_PATH), 1);
    should.equal(addon.getWoTSize(TEST_FILE_PATH), 2);
    // Add 100 nodes
    for (let i = 0; i < 10; i++) {
      should.equal(addon.addNode(TEST_FILE_PATH), i + 2);
    }
    should.equal(addon.getWoTSize(TEST_FILE_PATH), 2 + 10);
  });

  //it('should give number 1 if we add a node', function() {
  //  should.equal(addon.addNode(TEST_FILE_PATH), 1);
  //});

  //it('should success on verify', function() {
  //  let nbMembers = 1000 * 1000; // 1 million
  //  let nbCerts = 16; // 16 per member
  //  let octetsPerCert = 4;
  //  console.log('Allocate %s MB', octetsPerCert * nbCerts * nbMembers / 1000 / 1000);
  //  should.equal(addon.verify(nbMembers, nbCerts, TEST_FILE_PATH), 8);
  //});

  //after(() => {
  //  if (fs.existsSync(TEST_FILE_PATH)) {
  //    fs.unlinkSync(TEST_FILE_PATH);
  //  }
  //  return Promise.resolve();
  //});
});
