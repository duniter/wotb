#include "include/webOfTrust.h"
#include "include/FileWoT.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <random>
#include <fstream>

namespace libwot {

  using namespace std;

  FileWoT::FileWoT(std::string f) {
    filename = f;
    WebOfTrust *wot = WebOfTrust::readFromDisk(f);
    if (wot == NULL) {
      wot = new WebOfTrust(3);
      wot->writeToDisk(f);
    }

    delete wot;
  }

  FileWoT::~FileWoT() {
  }

  void FileWoT::reset() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    wot->reset();
    wot->writeToDisk(filename);
    delete wot;
  }

  WebOfTrust* FileWoT::getWoT() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    return wot;
  }

  void FileWoT::showWoT() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    wot->showTable();
    delete wot;
  }

  void FileWoT::showGraph() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    wot->showTable();
    delete wot;
  }

  uint32_t FileWoT::getWoTSize() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    uint32_t size = wot->getSize();
    delete wot;
    return size;
  }

  uint32_t FileWoT::addNode() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    wot->addNode();
    uint32_t size = wot->getSize() - 1;
    wot->writeToDisk(filename);
    delete wot;
    return size;
  }

  uint32_t FileWoT::removeNode() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    wot->removeNode();
    uint32_t size = wot->getSize() - 1;
    wot->writeToDisk(filename);
    delete wot;
    return size;
  }

  void FileWoT::setMaxCert(uint32_t maxCert) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    wot->setMaxCert(maxCert);
    wot->writeToDisk(filename);
    delete wot;
  }

  uint32_t FileWoT::getMaxCert() {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    uint32_t max = wot->getMaxCert();
    delete wot;
    return max;
  }

  bool FileWoT::isEnabled(uint32_t nodeIndex) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    if (nodeIndex >= wot->getSize()) return false;
    bool enabled = wot->getNodeAt(nodeIndex)->isEnabled();
    delete wot;
    return enabled;
  }

  bool FileWoT::setEnabled(bool enabled, uint32_t nodeIndex) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    Node* node = wot->getNodeAt(nodeIndex);
    node->setEnabled(enabled);
    bool isEnabled = node->isEnabled();
    wot->writeToDisk(filename);
    delete wot;
    return isEnabled;
  }

  bool FileWoT::existsLink(uint32_t from, uint32_t to) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    if (from >= wot->getSize()) return false;
    if (to >= wot->getSize()) return false;
    bool exists = wot->getNodeAt(from)->hasLinkTo(to);
    delete wot;
    return exists;
  }

  uint32_t FileWoT::addLink(uint32_t from, uint32_t to) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    Node* node = wot->getNodeAt(from);
    node->addLinkTo(to);
    uint32_t nbLinks = wot->getNodeAt(to)->getNbLinks();
    wot->writeToDisk(filename);
    delete wot;
    return nbLinks;
  }

  uint32_t FileWoT::removeLink(uint32_t from, uint32_t to) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    Node* node = wot->getNodeAt(from);
    node->removeLinkTo(to);
    uint32_t nbLinks = wot->getNodeAt(to)->getNbLinks();
    wot->writeToDisk(filename);
    delete wot;
    return nbLinks;
  }

  bool FileWoT::isOutdistanced(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    DistanceResult result = wot->computeDistance(member, d_min, k_max, x_percent);
    delete wot;
    return result.isOutdistanced;
  }

  WoTSet FileWoT::getSentries(int d_min) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    WoTSet result = wot->getSentries(d_min);
    delete wot;
    return result;
  }

  WoTSet FileWoT::getNonSentries(int d_min) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    WoTSet result = wot->getNonSentries(d_min);
    delete wot;
    return result;
  }

  std::vector<std::vector<uint32_t>> FileWoT::getPaths(uint32_t from, uint32_t to, uint32_t k_max) {
    WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
    std::vector<std::vector<uint32_t>> result = wot->getPaths(from, to, k_max);
    delete wot;
    return result;
  }

}
