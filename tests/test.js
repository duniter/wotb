"use strict";

var addon  = require('./../index');
var should = require('should');
var path = require('path');
var fs = require('fs');

const FILE = path.join(__dirname, 'wot.bin');
const X_PERCENT = 1.0;
const _100_PERCENT = 1.0;
const MAX_DISTANCE_1 = 1;
const MAX_DISTANCE_2 = 2;
const MAX_DISTANCE_3 = 3;
const MAX_DISTANCE_4 = 4;
const MAX_DISTANCE_5 = 5;
const FROM_1_LINK_SENTRIES = 1;
const FROM_2_LINKS_SENTRIES = 2;
const FROM_3_LINKS_SENTRIES = 3;
const __OUTDISTANCED__ = true;
const __OK__ = false;

describe('Basic operations', function() {

  before(() => {
    if (fs.existsSync(FILE)) {
      fs.unlinkSync(FILE);
    }
    return Promise.resolve();
  });

  it('should have an initial size of 0', function() {
    should.equal(addon.getWoTSize(FILE), 0);
  });

  it('should give number 0 if we add a node', function() {
    // Add a node
    should.equal(addon.addNode(FILE), 0);
    should.equal(addon.getWoTSize(FILE), 1);
    // Add another
    should.equal(addon.addNode(FILE), 1);
    should.equal(addon.getWoTSize(FILE), 2);
    // Add 100 nodes
    for (let i = 0; i < 10; i++) {
      should.equal(addon.addNode(FILE), i + 2);
    }
    should.equal(addon.getWoTSize(FILE), 2 + 10);
  });

  it('first 4 nodes should be enabled', function() {
    should.equal(addon.isEnabled(0, FILE), true);
    should.equal(addon.isEnabled(1, FILE), true);
    should.equal(addon.isEnabled(2, FILE), true);
    should.equal(addon.isEnabled(3, FILE), true);
  });

  it('should be able to disable some nodes', function() {
    should.equal(addon.setEnabled(false, 0, FILE), false);
    should.equal(addon.setEnabled(false, 1, FILE), false);
    should.equal(addon.setEnabled(false, 2, FILE), false);
    should.equal(addon.setEnabled(true, 1, FILE), true);
  });

  it('nodes 0 and 2 should be disabled', function() {
    should.equal(addon.isEnabled(0, FILE), false);
    should.equal(addon.isEnabled(1, FILE), true);
    should.equal(addon.isEnabled(2, FILE), false);
    should.equal(addon.isEnabled(3, FILE), true);
  });

  it('should not exist a link from 2 to 0', function() {
    should.equal(addon.existsLink(2, 0, FILE), false);
  });

  it('should be able to add some links', function() {
    should.equal(addon.addLink(2, 0, FILE), 1);
    should.equal(addon.addLink(4, 0, FILE), 2);
    should.equal(addon.addLink(4, 0, FILE), 2);
    should.equal(addon.addLink(4, 0, FILE), 2);
    should.equal(addon.addLink(5, 0, FILE), 3);
  });

  it('should exist new links', function() {
    /**
     * WoT is:
     *
     * 2 --> 0
     * 4 --> 0
     * 5 --> 0
     */
    should.equal(addon.existsLink(2, 0, FILE), true);
    should.equal(addon.existsLink(4, 0, FILE), true);
    should.equal(addon.existsLink(5, 0, FILE), true);
    should.equal(addon.existsLink(2, 1, FILE), false);
  });

  it('should be able to remove some links', function() {
    should.equal(addon.removeLink(4, 0, FILE), 2);
    /**
     * WoT is now:
     *
     * 2 --> 0
     * 5 --> 0
     */
  });

  it('should exist less links', function() {
    should.equal(addon.existsLink(2, 0, FILE), true);
    should.equal(addon.existsLink(4, 0, FILE), false);
    should.equal(addon.existsLink(5, 0, FILE), true);
    should.equal(addon.existsLink(2, 1, FILE), false);
  });

  it('should successfully use distance rule', function() {
    should.equal(addon.isOutdistanced(0, FROM_1_LINK_SENTRIES, MAX_DISTANCE_1, X_PERCENT, FILE), __OK__); // No because 2,4,5 have certified him
    should.equal(addon.isOutdistanced(0, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_1, X_PERCENT, FILE), __OK__); // No because only member 2 has 2 certs, and has certified him
    should.equal(addon.isOutdistanced(0, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_1, X_PERCENT, FILE), __OK__); // No because no member has issued 3 certifications
    // We add links from member 3
    should.equal(addon.addLink(3, 1, FILE), 1);
    should.equal(addon.addLink(3, 2, FILE), 1);
    /**
     * WoT is now:
     *
     * 2 --> 0
     * 5 --> 0
     * 3 --> 1
     * 3 --> 2
     */
    should.equal(addon.isOutdistanced(0, FROM_1_LINK_SENTRIES, MAX_DISTANCE_1, X_PERCENT, FILE), __OUTDISTANCED__); // KO: No path 3 --> 0
    should.equal(addon.isOutdistanced(0, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_1, X_PERCENT, FILE), __OUTDISTANCED__); // KO: No path 3 --> 0
    should.equal(addon.isOutdistanced(0, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_1, X_PERCENT, FILE), __OK__); // OK: no sentry with 3 links issued
    should.equal(addon.isOutdistanced(0, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_2, X_PERCENT, FILE), __OK__); // OK: 3 --> 2 --> 0
  });

  after(() => {
    if (fs.existsSync(FILE)) {
      fs.unlinkSync(FILE);
    }
    return Promise.resolve();
  });
});

describe('Building a larger WoT', function() {

  before(() => {
    if (fs.existsSync(FILE)) {
      fs.unlinkSync(FILE);
    }
    /**
     * We build WoT:
     *
     * 0 --> 1 --> 2 --> 4 --> 5 <==> 6 --> 7
     *             ^
     *            ||
     *            ##==> 3 <-- 8 <-- 9 <========##
     *                       |                 ||
     *                       `> 10 <==> 11 <===##
     */
    // Add nodes
    for (let i = 0; i < 12; i++) {
      should.equal(addon.addNode(FILE), i);
    }
    // First line
    should.equal(addon.addLink(0, 1, FILE), 1);
    should.equal(addon.addLink(1, 2, FILE), 1);
    should.equal(addon.addLink(2, 4, FILE), 1);
    should.equal(addon.addLink(4, 5, FILE), 1);
    should.equal(addon.addLink(5, 6, FILE), 1);
    should.equal(addon.addLink(6, 7, FILE), 1);
    should.equal(addon.addLink(6, 5, FILE), 2);
    // 2n level
    should.equal(addon.addLink(2, 3, FILE), 1);
    should.equal(addon.addLink(3, 2, FILE), 2);
    should.equal(addon.addLink(8, 3, FILE), 2);
    should.equal(addon.addLink(9, 8, FILE), 1);
    // 3rd level
    should.equal(addon.addLink(8, 10, FILE), 1);
    should.equal(addon.addLink(10, 11, FILE), 1);
    should.equal(addon.addLink(11, 10, FILE), 2);
    should.equal(addon.addLink(11, 9, FILE), 1);
    should.equal(addon.addLink(9, 11, FILE), 2);

    should.equal(addon.getWoTSize(FILE), 12);
    return Promise.resolve();
  });

  it('should have an initial size of 0', function() {
    should.equal(addon.getWoTSize(FILE), 12);
  });

  describe('testing around 2 with d = 1', () => {

    /**
     * Sentries of 1 link (X are not sentries):
     *
     * 0 --> 1 --> 2 --> 4 --> 5 <==> 6 --> X
     *             ^
     *            ||
     *            ##==> 3 <-- 8 <-- 9 <========##
     *                       |                 ||
     *                       `> 10 <==> 11 <===##
     */
    // => It can be seen 0..6, 8..11 = 11 sentries
    // => MINUS the sentry #2 (which is tested and is not to be included)
    // => 10 sentries

    it('distance k = 1', function() {
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_1, _100_PERCENT, FILE), __OUTDISTANCED__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_1, 0.5, FILE), __OUTDISTANCED__);
      // 20% of the sentries: OK
      // => 20% x 10 = 2 sentries to reach
      // => we have 1 --> 2
      // => we have 3 --> 2
      // => OK (1,3)
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_1, 0.2, FILE), __OK__);
      // Who can pass 20% can pass 10%
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_1, 0.1, FILE), __OK__);
      // But cannot pass 21%
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_1, 0.21, FILE), __OUTDISTANCED__);
    });

    it('distance k = 2', function() {
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_2, _100_PERCENT, FILE), __OUTDISTANCED__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_2, 0.5, FILE), __OUTDISTANCED__);
      // 40% of the sentries: OK
      // => 40% x 10 = 4 sentries to reach
      // With k = 2 we have the following paths:
      // 0 --> 1 --> 2
      // 8 --> 3 --> 2
      // => OK (0,1,8,3)
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_2, 0.4, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_2, 0.3, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_2, 0.2, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_2, 0.1, FILE), __OK__);
      // But cannot pass 41%
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_2, 0.41, FILE), __OUTDISTANCED__);
    });

    it('distance k = 5', function() {
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_5, _100_PERCENT, FILE), __OUTDISTANCED__);
      // 70% of the sentries: OK
      // => 70% x 10 = 7 sentries to reach
      // With k = 5 we have the following paths:
      // 0 --> 1 --> 2
      // 10 --> 11 --> 9 --> 8 --> 3 --> 2
      // => OK (0,1,10,11,9,8,3)
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_5, 0.7, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_5, 0.3, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_5, 0.2, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_5, 0.1, FILE), __OK__);
      // But cannot pass 71%
      should.equal(addon.isOutdistanced(2, FROM_1_LINK_SENTRIES, MAX_DISTANCE_5, 0.71, FILE), __OUTDISTANCED__);
    });
  });

  describe('testing around 2 with d = 2', () => {
    /**
     * Sentries of 2 links (X are not sentries):
     *
     * X --> X --> 2 --> X --> X <==> 6 --> X
     *             ^
     *            ||
     *            ##==> X <-- 8 <-- 9 <========##
     *                       |                 ||
     *                       `> X  <==> 11 <===##
     */
    // => It can be seen 2,6,8,9,11 = 5 sentries
    // => MINUS the sentry #2 (which is tested and is not to be included)
    // => 4 sentries

    it('distance k = 1', function() {
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_5, _100_PERCENT, FILE), __OUTDISTANCED__);
      // With k = 1 we have no paths
      // => ALWAYS KO
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_1, 0.99, FILE), __OUTDISTANCED__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_1, 0.5, FILE), __OUTDISTANCED__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_1, 0.01, FILE), __OUTDISTANCED__);
    });

    it('distance k = 2', function() {
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_2, _100_PERCENT, FILE), __OUTDISTANCED__);
      // 25% of the sentries: OK
      // => 25% x 4 = 1 sentries to reach
      // With k = 2 we have the following paths:
      // 8 --> X --> 2
      // => OK (8)
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_2, 0.25, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_2, 0.24, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_2, 0.251, FILE), __OUTDISTANCED__);
    });

    it('distance k = 3', function() {
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_3, _100_PERCENT, FILE), __OUTDISTANCED__);
      // 50% of the sentries: OK
      // => 50% x 4 = 2 sentries to reach
      // With k = 5 we have the following paths:
      // 9 --> 8 --> X --> 2
      // => OK (9,8)
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_3, 0.50, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_3, 0.49, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_3, 0.51, FILE), __OUTDISTANCED__);
    });

    it('distance k = 4', function() {
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_4, _100_PERCENT, FILE), __OUTDISTANCED__);
      // 75% of the sentries: OK
      // => 75% x 4 = 3 sentries to reach
      // With k = 5 we have the following paths:
      // 11 --> 9 --> 8 --> X --> 2
      // => OK (11,9,8)
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_4, 0.75, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_4, 0.74, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_4, 0.76, FILE), __OUTDISTANCED__);
    });

    it('distance k = 5', function() {
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_5, _100_PERCENT, FILE), __OUTDISTANCED__);
      // 75% of the sentries: OK
      // => 75% x 4 = 3 sentries to reach
      // With k = 5 we have the following paths:
      // 11 --> 9 --> 8 --> X --> 2
      // => OK (11,9,8)
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_5, 0.75, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_5, 0.74, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_2_LINKS_SENTRIES, MAX_DISTANCE_5, 0.76, FILE), __OUTDISTANCED__);
    });
  });

  describe('testing around 2 with d = 3', () => {
    /**
     * Sentries of 3 links (X are not sentries):
     *
     * X --> X --> 2 --> X --> X <==> X --> X
     *             ^
     *            ||
     *            ##==> X <-- X <-- X <========##
     *                       |                 ||
     *                       `> X  <==> X <===##
     */
    // => It can be seen 2 = 1 sentries
    // => MINUS the sentry #2 (which is tested and is not to be included)
    // => 0 sentries
    // => ALWAYS OK, no sentries to constraint

    it('distance k = 1', function() {
      should.equal(addon.isOutdistanced(2, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_1, _100_PERCENT, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_1, 0.01, FILE), __OK__);
    });

    it('distance k = 2', function() {
      should.equal(addon.isOutdistanced(2, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_2, _100_PERCENT, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_2, 0.01, FILE), __OK__);
    });

    it('distance k = 5', function() {
      should.equal(addon.isOutdistanced(2, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_5, _100_PERCENT, FILE), __OK__);
      should.equal(addon.isOutdistanced(2, FROM_3_LINKS_SENTRIES, MAX_DISTANCE_5, 0.01, FILE), __OK__);
    });
  });

  after(() => {
    if (fs.existsSync(FILE)) {
      fs.unlinkSync(FILE);
    }
    return Promise.resolve();
  });
});
