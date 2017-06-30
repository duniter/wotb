#include "include/webOfTrust.h"
#include "include/MemoryWoT.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <random>
#include <fstream>
#include <vector>

namespace libwot {

  using namespace std;
  using namespace libwot;

  MemoryWoT::MemoryWoT() {
    wot = new WebOfTrust(3);
  }

  MemoryWoT::MemoryWoT(WebOfTrust* newWoT) {
    wot = newWoT;
  }

  MemoryWoT::~MemoryWoT() {
    delete wot;
  }

  void MemoryWoT::reset() {
    wot->reset();
  }

  WebOfTrust* MemoryWoT::cloneWoT() {
    return wot->clone();
  }

  void MemoryWoT::showWoT() {
    wot->showTable();
  }

  std::string MemoryWoT::dumpWoT() {
    return wot->dump();
  }

  void MemoryWoT::showGraph() {
    wot->showTable();
  }

  uint32_t MemoryWoT::getWoTSize() {
    return wot->getSize();
  }

  uint32_t MemoryWoT::addNode() {
    wot->addNode();
    return wot->getSize() - 1;
  }

  uint32_t MemoryWoT::removeNode() {
    wot->removeNode();
    return wot->getSize() - 1;
  }

  void MemoryWoT::setMaxCert(uint32_t maxCert) {
    wot->setMaxCert(maxCert);
  }

  uint32_t MemoryWoT::getMaxCert() {
    return wot->getMaxCert();
  }

  bool MemoryWoT::isEnabled(uint32_t nodeIndex) {
    if (nodeIndex >= wot->getSize()) return false;
    return wot->getNodeAt(nodeIndex)->isEnabled();
  }

  bool MemoryWoT::setEnabled(bool enabled, uint32_t nodeIndex) {
    return wot->getNodeAt(nodeIndex)->setEnabled(enabled)->isEnabled();
  }

  bool MemoryWoT::existsLink(uint32_t from, uint32_t to) {
    if (from >= wot->getSize()) return false;
    if (to >= wot->getSize()) return false;
    return wot->getNodeAt(from)->hasLinkTo(to);
  }

  uint32_t MemoryWoT::addLink(uint32_t from, uint32_t to) {
    wot->getNodeAt(from)->addLinkTo(to);
    return wot->getNodeAt(to)->getNbLinks();
  }

  uint32_t MemoryWoT::removeLink(uint32_t from, uint32_t to) {
    wot->getNodeAt(from)->removeLinkTo(to);
    return wot->getNodeAt(to)->getNbLinks();
  }

  DistanceResult MemoryWoT::computeDistance(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent) {
    return wot->computeDistance(member, d_min, k_max, x_percent);
  }

  vector<uint32_t> MemoryWoT::getSentries(int d_min) {
    return wot->getSentries(d_min);
  }

  vector<uint32_t> MemoryWoT::getNonSentries(int d_min) {
    return wot->getNonSentries(d_min);
  }

  vector<uint32_t> MemoryWoT::getDisabled() {
    return wot->getDisabled();
  }

  std::vector<std::vector<uint32_t>> MemoryWoT::getPaths(uint32_t from, uint32_t to, uint32_t k_max) {
    return wot->getPaths(from, to, k_max);
  }
}
