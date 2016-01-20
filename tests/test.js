"use strict";

var addon  = require('./../index');
var should = require('should');
var path = require('path');
var fs = require('fs');

const TEST_FILE_PATH = path.join(__dirname, 'wot.bin');

describe('Basic operations', function() {

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

  it('first 4 nodes should be enabled', function() {
    should.equal(addon.isEnabled(0, TEST_FILE_PATH), true);
    should.equal(addon.isEnabled(1, TEST_FILE_PATH), true);
    should.equal(addon.isEnabled(2, TEST_FILE_PATH), true);
    should.equal(addon.isEnabled(3, TEST_FILE_PATH), true);
  });

  it('should be able to disable some nodes', function() {
    should.equal(addon.setEnabled(false, 0, TEST_FILE_PATH), false);
    should.equal(addon.setEnabled(false, 1, TEST_FILE_PATH), false);
    should.equal(addon.setEnabled(false, 2, TEST_FILE_PATH), false);
    should.equal(addon.setEnabled(true, 1, TEST_FILE_PATH), true);
  });

  it('nodes 0 and 2 should be disabled', function() {
    should.equal(addon.isEnabled(0, TEST_FILE_PATH), false);
    should.equal(addon.isEnabled(1, TEST_FILE_PATH), true);
    should.equal(addon.isEnabled(2, TEST_FILE_PATH), false);
    should.equal(addon.isEnabled(3, TEST_FILE_PATH), true);
  });

  it('should not exist a link from 2 to 0', function() {
    should.equal(addon.existsLink(2, 0, TEST_FILE_PATH), false);
  });

  it('should be able to add some links', function() {
    should.equal(addon.addLink(2, 0, TEST_FILE_PATH), 1);
    should.equal(addon.addLink(4, 0, TEST_FILE_PATH), 2);
    should.equal(addon.addLink(4, 0, TEST_FILE_PATH), 2);
    should.equal(addon.addLink(4, 0, TEST_FILE_PATH), 2);
    should.equal(addon.addLink(5, 0, TEST_FILE_PATH), 3);
  });

  it('should exist new links', function() {
    /**
     * WoT is:
     *
     * 2 --> 0
     * 4 --> 0
     * 5 --> 0
     */
    should.equal(addon.existsLink(2, 0, TEST_FILE_PATH), true);
    should.equal(addon.existsLink(4, 0, TEST_FILE_PATH), true);
    should.equal(addon.existsLink(5, 0, TEST_FILE_PATH), true);
    should.equal(addon.existsLink(2, 1, TEST_FILE_PATH), false);
  });

  it('should be able to remove some links', function() {
    should.equal(addon.removeLink(4, 0, TEST_FILE_PATH), 2);
    /**
     * WoT is now:
     *
     * 2 --> 0
     * 5 --> 0
     */
  });

  it('should exist less links', function() {
    should.equal(addon.existsLink(2, 0, TEST_FILE_PATH), true);
    should.equal(addon.existsLink(4, 0, TEST_FILE_PATH), false);
    should.equal(addon.existsLink(5, 0, TEST_FILE_PATH), true);
    should.equal(addon.existsLink(2, 1, TEST_FILE_PATH), false);
  });

  it('should successfully use distance rule', function() {
    const X_PERCENT = 1.0;
    const MAX_DISTANCE_1 = 1;
    const MAX_DISTANCE_2 = 2;
    const FROM_MEMBERS_WITH_AT_LEAST_1_LINKS_ISSUED = 1;
    const FROM_MEMBERS_WITH_AT_LEAST_2_LINKS_ISSUED = 2;
    const FROM_MEMBERS_WITH_AT_LEAST_3_LINKS_ISSUED = 3;
    should.equal(addon.isOutdistanced(0, FROM_MEMBERS_WITH_AT_LEAST_1_LINKS_ISSUED, MAX_DISTANCE_1, X_PERCENT, TEST_FILE_PATH), false); // No because 2,4,5 have certified him
    should.equal(addon.isOutdistanced(0, FROM_MEMBERS_WITH_AT_LEAST_2_LINKS_ISSUED, MAX_DISTANCE_1, X_PERCENT, TEST_FILE_PATH), false); // No because only member 2 has 2 certs, and has certified him
    should.equal(addon.isOutdistanced(0, FROM_MEMBERS_WITH_AT_LEAST_3_LINKS_ISSUED, MAX_DISTANCE_1, X_PERCENT, TEST_FILE_PATH), false); // No because no member has issued 3 certifications
    // We add links from member 3
    should.equal(addon.addLink(3, 1, TEST_FILE_PATH), 1);
    should.equal(addon.addLink(3, 2, TEST_FILE_PATH), 1);
    /**
     * WoT is now:
     *
     * 2 --> 0
     * 5 --> 0
     * 3 --> 1
     * 3 --> 2
     */
    should.equal(addon.isOutdistanced(0, FROM_MEMBERS_WITH_AT_LEAST_1_LINKS_ISSUED, MAX_DISTANCE_1, X_PERCENT, TEST_FILE_PATH), true); // KO: No path 3 --> 0
    should.equal(addon.isOutdistanced(0, FROM_MEMBERS_WITH_AT_LEAST_2_LINKS_ISSUED, MAX_DISTANCE_1, X_PERCENT, TEST_FILE_PATH), true); // KO: No path 3 --> 0
    should.equal(addon.isOutdistanced(0, FROM_MEMBERS_WITH_AT_LEAST_3_LINKS_ISSUED, MAX_DISTANCE_1, X_PERCENT, TEST_FILE_PATH), false); // OK: no sentry with 3 links issued
    should.equal(addon.isOutdistanced(0, FROM_MEMBERS_WITH_AT_LEAST_2_LINKS_ISSUED, MAX_DISTANCE_2, X_PERCENT, TEST_FILE_PATH), false); // OK: 3 --> 2 --> 0
  });

  //it('should success on verify', function() {
  //  let nbMembers = 1000 * 1000; // 1 million
  //  let nbCerts = 16; // 16 per member
  //  let octetsPerCert = 4;
  //  console.log('Allocate %s MB', octetsPerCert * nbCerts * nbMembers / 1000 / 1000);
  //  should.equal(addon.verify(nbMembers, nbCerts, TEST_FILE_PATH), 8);
  //});

  after(() => {
    if (fs.existsSync(TEST_FILE_PATH)) {
      fs.unlinkSync(TEST_FILE_PATH);
    }
    return Promise.resolve();
  });
});
