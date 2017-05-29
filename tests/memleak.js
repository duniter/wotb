"use strict";

const fs     = require('fs');
const path   = require('path');
const addon  = require('./../index');
const assert = require('assert');
const should = require('should');

const NB_ITERATIONS = 10000
const NB_NODES_INIT = 100
const NB_NODES_ADDED = 100
const PATH_TO_FILE_INSTANCE = path.join(__dirname, 'leak-test.bin')

let baseFileInstance

describe('Memory leaks', function() {

  this.timeout(10000) // 10 seconds max for a test

  before(() => {
    if (fs.existsSync(PATH_TO_FILE_INSTANCE)) {
      fs.unlinkSync(PATH_TO_FILE_INSTANCE)
    }
    baseFileInstance = addon.newFileInstance(PATH_TO_FILE_INSTANCE)
    for (let i = 0; i < NB_NODES_INIT; i++) {
      assert.equal(baseFileInstance.addNode(), i);
      // Each node certify its neighbor, the very last will certify the first.
      if (i > 0) {
        if (i < NB_NODES_INIT - 1) {
          baseFileInstance.addLink(i - 1, i)
        } else {
          baseFileInstance.addLink(i, 0)
        }
      }
    }
  })

  it('copy from file instance should have no leak', () => {
    const rssStart = getMemoryUsageInMB()
    for (let it = 0; it < NB_ITERATIONS; it++) {
      const clone = baseFileInstance.memCopy()
      clone.clear()
    }
    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, 1);
  })

  it('copy from memory instance should have no leak', () => {
    const memInstance = baseFileInstance.memCopy()
    const rssStart = getMemoryUsageInMB()
    for (let it = 0; it < NB_ITERATIONS; it++) {
      const clone = memInstance.memCopy()
      clone.clear()
    }
    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, 1);
  })

  it('wotb.clear() should have no leak', () => {
    const baseInstance = addon.newMemoryInstance()
    for (let i = 0; i < NB_NODES_INIT; i++) {
      assert.equal(baseInstance.addNode(), i);
    }
    const rssStart = getMemoryUsageInMB()
    for (let it = 0; it < NB_ITERATIONS; it++) {
      const clone = baseInstance.memCopy()
      for (let i = 0; i < NB_NODES_ADDED; i++) {
        assert.equal(clone.addNode(), NB_NODES_INIT + i);
      }
      clone.getWoTSize()
      clone.clear()
    }
    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, 1);
  })

  it('wotb.getSentries() should have no leak', () => {
    const approx0MB = 0
    const approx1MB = 1
    const approx2MB = 2
    const approx5MB = 5
    const rssStart = testGetSentries(1, approx0MB)
    testGetSentries(10, approx0MB)     // should add 0MB memory relatively to previous test
    testGetSentries(100, approx1MB)    // should add 1MB ...
    testGetSentries(1000, approx2MB)   // should add 2MB ...
    testGetSentries(10000, approx5MB)  // should add 5MB ...
    testGetSentries(10000, approx1MB)  // should add 1MB until the end of the test
    testGetSentries(10000, approx1MB)
    testGetSentries(10000, approx1MB)
    testGetSentries(10000, approx1MB)
    testGetSentries(10000, approx1MB)

    // Conclusion: no leak.
    // => only the magnitude of the WoT makes an increase of memory usage, but this not a leak.

    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, approx5MB);
  })

  it('wotb.getNonSentries() should have no leak', () => {
    const approx0MB = 0
    const approx1MB = 1
    const approx5MB = 5
    const rssStart = testGetNonSentries(1, approx0MB)
    testGetNonSentries(10, approx0MB)     // should add 0MB memory relatively to previous test
    testGetNonSentries(100, approx0MB)    // should add 0MB ...
    testGetNonSentries(1000, approx1MB)   // should add 1MB ...
    testGetNonSentries(10000, approx5MB)  // should add 5MB ...
    testGetNonSentries(10000, approx0MB)  // should add 0MB until the end of the test
    testGetNonSentries(10000, approx0MB)
    testGetNonSentries(10000, approx0MB)
    testGetNonSentries(10000, approx0MB)
    testGetNonSentries(10000, approx0MB)

    // Conclusion: no leak.
    // => only the magnitude of the WoT makes an increase of memory usage, but this not a leak.

    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, approx5MB);
  })

  it('wotb.getDisabled() should have no leak', () => {
    const approx0MB = 0
    const approx1MB = 1
    const approx5MB = 5
    const rssStart = testGetDisabled(1, approx0MB)
    testGetDisabled(10, approx0MB)     // should add 0MB memory relatively to previous test
    testGetDisabled(100, approx0MB)    // should add 0MB ...
    testGetDisabled(1000, approx1MB)   // should add 1MB ...
    testGetDisabled(10000, approx5MB)  // should add 5MB ...
    testGetDisabled(10000, approx0MB)  // should add 0MB until the end of the test
    testGetDisabled(10000, approx0MB)
    // testGetDisabled(10000, approx0MB)
    // testGetDisabled(10000, approx0MB)
    for (let i = 0; i < 100; i++) {
      testGetDisabled(1000, approx1MB)  // should add 0MB until the end of the test
    }

    // Conclusion: no leak.
    // => only the magnitude of the WoT makes an increase of memory usage, but this not a leak.

    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, approx5MB);
  })

  it('wotb.getPaths() should have no leak', () => {
    const approx0MB = 0
    const approx1MB = 1
    const approx2MB = 2
    const approx6MB = 6
    const rssStart = testGetPaths(1, approx0MB)
    testGetPaths(10, approx0MB)     // should add 0MB memory relatively to previous test
    testGetPaths(100, approx0MB)    // should add 0MB ...
    testGetPaths(1000, approx1MB)   // should add 1MB ...
    testGetPaths(10000, approx6MB)  // should add 6MB ...
    for (let i = 0; i < 1000; i++) {
      testGetPaths(1000, approx2MB)  // should add MAX 1MB on each test
    }
    
    // Conclusion: no leak.
    // => only the magnitude of the WoT makes an increase of memory usage, but this not a leak.

    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, approx6MB); // From the beginning, only 6MB should have been added.
  })

  it('wotb.isOutdistanced() should have no leak', () => {
    const approx0MB = 0
    const approx1MB = 1
    const approx2MB = 2
    const approx6MB = 6
    const rssStart = testDistance(1, approx0MB)
    testDistance(10, approx0MB)     // should add 0MB memory relatively to previous test
    testDistance(100, approx0MB)    // should add 0MB ...
    testDistance(1000, approx1MB)   // should add 1MB ...
    testDistance(10000, approx6MB)  // should add 6MB ...
    for (let i = 0; i < 1000; i++) {
      testDistance(1000, approx2MB)  // should add MAX 1MB on each test
    }

    // Conclusion: no leak.
    // => only the magnitude of the WoT makes an increase of memory usage, but this not a leak.

    const rssEnd = getMemoryUsageInMB()
    rssEnd.should.be.approximately(rssStart, approx6MB); // From the beginning, only 6MB should have been added.
  })
})

function getMemoryUsageInMB() {
  return Math.floor(process.memoryUsage().rss / 1024 / 1024)
}

function testGetSentries(repetitions, approximation) {
  const nbNodes = 100
  const baseInstance = createBaseWoT(nbNodes)
  const rssStart = getMemoryUsageInMB()
  for (let it = 0; it < repetitions; it++) {
    assert.equal(baseInstance.getSentries(0).length, nbNodes);
  }
  const rssEnd = getMemoryUsageInMB()
  rssEnd.should.be.approximately(rssStart, approximation);
  return rssStart
}

function testGetNonSentries(repetitions, approximation) {
  const nbNodes = 100
  const baseInstance = createBaseWoT(nbNodes)
  const rssStart = getMemoryUsageInMB()
  for (let it = 0; it < repetitions; it++) {
    assert.equal(baseInstance.getNonSentries(5).length, nbNodes);
  }
  const rssEnd = getMemoryUsageInMB()
  rssEnd.should.be.approximately(rssStart, approximation);
  return rssStart
}

function testGetDisabled(repetitions, approximation) {
  const nbNodes = 100
  const baseInstance = createBaseWoT(nbNodes)
  // Disable nodes
  for (let i = 0; i < nbNodes; i++) {
    baseInstance.setEnabled(false, i)
    assert.equal(baseInstance.isEnabled(i), false)
  }
  const rssStart = getMemoryUsageInMB()
  for (let it = 0; it < repetitions; it++) {
    assert.equal(baseInstance.getDisabled().length, nbNodes);
  }
  const rssEnd = getMemoryUsageInMB()
  rssEnd.should.be.approximately(rssStart, approximation);
  baseInstance.clear();
  return rssStart
}

function testGetPaths(repetitions, approximation) {
  const nbNodes = 100
  const baseInstance = createBaseWoT(nbNodes)
  const rssStart = getMemoryUsageInMB()
  for (let it = 0; it < repetitions; it++) {
    assert.equal(baseInstance.getPaths(1, 2, 1).length, 1);
  }
  const rssEnd = getMemoryUsageInMB()
  rssEnd.should.be.approximately(rssStart, approximation);
  baseInstance.clear();
  return rssStart
}

function testDistance(repetitions, approximation) {
  const nbNodes = 100
  const baseInstance = createBaseWoT(nbNodes)
  const rssStart = getMemoryUsageInMB()
  for (let it = 0; it < repetitions; it++) {
    assert.equal(baseInstance.isOutdistanced(1, 1, 5, 0.5), 1);
  }
  const rssEnd = getMemoryUsageInMB()
  rssEnd.should.be.approximately(rssStart, approximation);
  baseInstance.clear();
  return rssStart
}

function createBaseWoT(nbNodes) {
  const baseInstance = addon.newMemoryInstance()
  for (let i = 0; i < nbNodes; i++) {
    assert.equal(baseInstance.addNode(), i);
    // Each node certify its neighbor, the very last will certify the first.
    if (i > 0) {
      if (i < nbNodes - 1) {
        baseInstance.addLink(i - 1, i)
      } else {
        baseInstance.addLink(i, 0)
      }
    }
  }
  return baseInstance
}
