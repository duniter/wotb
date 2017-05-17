"use strict";

const addon  = require('./../index');
const assert = require('assert');
const should = require('should');

const NB_ITERATIONS = 10000
const NB_NODES_INIT = 100
const NB_NODES_ADDED = 100

describe('Memory leak', () => {

  it('cleaning should avoid memory usage increasing', () => {
    const baseInstance = addon.newMemoryInstance()
    for (let i = 0; i < NB_NODES_INIT; i++) {
      assert.equal(baseInstance.addNode(), i);
    }
    const rssStart = Math.floor(process.memoryUsage().rss / 1024 / 1024)
    for (let it = 0; it < NB_ITERATIONS; it++) {
      const clone = baseInstance.memCopy()
      for (let i = 0; i < NB_NODES_ADDED; i++) {
        assert.equal(clone.addNode(), NB_NODES_INIT + i);
      }
      clone.getWoTSize()
      clone.clear()
    }
    const rssEnd = Math.floor(process.memoryUsage().rss / 1024 / 1024)
    rssEnd.should.be.approximately(rssStart, 1);
  })
})
