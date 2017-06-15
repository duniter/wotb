"use strict";

const addon  = require('./../index');
const assert = require('assert');
const should = require('should');

let baseInstance

describe('Testing getPaths()', function() {

  this.timeout(10000) // 10 seconds max for a test

  before(() => {
    baseInstance = addon.newMemoryInstance()
    baseInstance.addNode()
    baseInstance.addNode()
    baseInstance.addNode()
    baseInstance.addNode()
    baseInstance.addLink(0, 1)
    baseInstance.addLink(1, 2)
    baseInstance.addLink(2, 3)
    baseInstance.addLink(0, 3)
    /**
     * Wot:
     *
     *   0 --> 1 --> 2 --> 3
     *    `----------------^
     */
  })

  it('wotb.getPaths() should return arrays with the size of the paths', () => {

    // Path 0 --> 1
    assert.equal(baseInstance.getPaths(0, 1, 1).length, 1);
    assert.equal(baseInstance.getPaths(0, 1, 1)[0].length, 2); // 0, 1

    // Path 0 --> 2
    assert.equal(baseInstance.getPaths(0, 2, 1).length, 0);
    assert.equal(baseInstance.getPaths(0, 2, 2).length, 1);
    assert.equal(baseInstance.getPaths(0, 2, 2)[0].length, 3); // 0, 1, 2

    // Path 0 --> 3
    assert.equal(baseInstance.getPaths(0, 3, 1).length, 1);
    assert.equal(baseInstance.getPaths(0, 3, 1)[0].length, 2);
    assert.equal(baseInstance.getPaths(0, 3, 2).length, 1);
    assert.equal(baseInstance.getPaths(0, 3, 2)[0].length, 2);
  })
})
