#include "include/webOfTrust.h"
#include "include/misc.h"
#include "include/log.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>

namespace libwot {

  using namespace std;

  vector<uint32_t> levels;

  WebOfTrust::WebOfTrust(uint32_t maxCert) {
    mNodes = vector<Node*>(0);
    mMaxCert = maxCert;
  }


  WebOfTrust::~WebOfTrust() {
    reset();
  }


  void WebOfTrust::reset() {
    for(vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); it++) {
      delete (Node*)*it;
    }
    mNodes.clear();
  }


  WebOfTrust* WebOfTrust::createRandom(uint32_t nbMembers, uint32_t maxCertStock) {
    WebOfTrust *wot = new WebOfTrust(maxCertStock);
    for (uint32_t i = 0; i < nbMembers; i++) {
      wot->addNode();
    }

    for(vector<Node*>::iterator it = wot->mNodes.begin(); it != wot->mNodes.end(); it++) {
      Node* certified = NULL;
      for (uint32_t i = 0; i < maxCertStock; i++) {
        do{
          certified = wot->getRandomNode();
        } while (((Node*)*it)->addLinkTo(certified) != true);	
      }
      ((Node*)*it)->setEnabled(true);
    }
    return wot;
  }


  WebOfTrust* WebOfTrust::readFromDisk(string filename) {
    ifstream file(filename, ios::binary);
    if(file.fail()) {
      cout << "Failed to open file for reading : " << filename << endl;
      return NULL;
    }

    uint32_t maxCert, nbNodes;

    file.read((char*)&maxCert, sizeof(uint32_t));
    if(file.eof()) return NULL;
    file.read((char*)&nbNodes, sizeof(uint32_t));
    if(file.eof()) return NULL;

    WebOfTrust* wot = new WebOfTrust(maxCert);
    for(uint32_t i = 0; i < nbNodes; i++) {
      wot->addNode();
    }

    uint8_t enabled;
    uint32_t nbLinks;
    uint32_t link;

    for(vector<Node*>::iterator itNode = wot->mNodes.begin(); itNode != wot->mNodes.end(); itNode++) {
      Node* node = (Node*)*itNode;
      file.read((char*)&enabled, sizeof(uint8_t));
      if(file.eof()) {
        delete wot;
        return NULL;
      }
      file.read((char*)&nbLinks, sizeof(uint32_t));
      if(file.eof()) {
        delete wot;
        return NULL;
      }

      node->setEnabled((enabled == 0) ? false : true);
      for(uint32_t i = 0; i < nbLinks; i++) {
        file.read((char*)&link, sizeof(uint32_t));
        if(file.eof()) {
          delete wot;
          return NULL;
        }
        wot->getNodeAt(link)->addLinkTo(node);
      }
    }

    file.close();
    return wot;
  }


  bool WebOfTrust::writeToDisk(string filename) {
    ofstream file(filename, ios::binary | ios::trunc);
    if(file.fail()) {
      cout << "Failed to open file for writing : " << filename << endl;
      return false;
    }

    uint32_t nbNodes = mNodes.size();

    file.write((char*)&mMaxCert, sizeof(uint32_t));
    file.write((char*)&nbNodes, sizeof(uint32_t));

    uint8_t enabled;
    uint32_t nbLinks;
    uint32_t index;

    for(vector<Node*>::iterator itNode = mNodes.begin(); itNode != mNodes.end();itNode++) {
      Node* node = (Node*)*itNode;	
      enabled = node->isEnabled() ? 1 : 0;
      nbLinks = node->getNbLinks();

      file.write((char*)&enabled, sizeof(uint8_t));
      file.write((char*)&nbLinks, sizeof(uint32_t));

      vector<Node*> links = node->getLinks();
      for(vector<Node*>::iterator itLink = links.begin(); itLink != links.end(); itLink++) {
        Node* node = (Node*)*itLink;
        index = getNodeIndex(node);
        file.write((char*)&index, sizeof(uint32_t));
      }
    }

    file.close();
    return true;
  }


  Node* WebOfTrust::addNode() {
    Node *node = new Node(this);
    mNodes.push_back(node);
    return node;
  }


  void WebOfTrust::removeNode() {
    mNodes.pop_back();
  }


  uint32_t WebOfTrust::getNodeIndex(Node* node) {
    vector<Node*>::iterator it = find(mNodes.begin(), mNodes.end(), node);
    return distance(mNodes.begin(), it);
  }


  Node* WebOfTrust::getRandomNode() {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, mNodes.size() > 0 ? mNodes.size() - 1 : 0);
    return mNodes.at(distr(eng));
  }


  WebOfTrust* WebOfTrust::showTable() {
    int fieldWidth = (mNodes.size() >= 1) ? countDigits(mNodes.size() - 1) : 1;
    int i = 0;
    cout << "[" << setw(fieldWidth) << right << "M" << "] [E] [R] [" << setw(fieldWidth) << right << "I" << "] -> " << "Links[maxCert = " << mMaxCert << "]" << endl;
    for (vector<Node*>::iterator itNode = mNodes.begin(); itNode != mNodes.end(); itNode++) {
      Node* node = (Node*)*itNode;
      cout << "[" << setw(fieldWidth) << right << i++ << "] [" << node->isEnabled() << "] [" << node->getNbLinks() << "] [" << setw(fieldWidth) << right << node->getNbIssued() << "] -> ";

      vector<Node*> links = node->getLinks();
      for(vector<Node*>::iterator itLink = links.begin(); itLink != links.end(); itLink++) {
        Node* certified = ((Node*)*itLink);
        cout << setw(fieldWidth) << right << getNodeIndex(certified) << " | ";
      }

      cout << endl;
    }

    return this;
  }


    string WebOfTrust::getGraphviz() {
      int i = 0;
      stringstream iss;
      iss << "digraph G {" << endl << endl;
      for(vector<Node*>::iterator itNode = mNodes.begin(); itNode != mNodes.end(); itNode++) {
        vector<Node*> links = ((Node*)*itNode)->getLinks();
        for(vector<Node*>::iterator itLink = links.begin(); itLink != links.end(); itLink++) {
          iss << "    " << getNodeIndex((Node*)*itLink) << " -> " << i << "" << endl;
        }
        i++;
      }
      iss << "}" << endl;

      return iss.str();
    }

  WebOfTrust* WebOfTrust::showGraphviz() {
    cout << getGraphviz();

    return this;
  }


  DistanceResult WebOfTrust::computeDistance(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent) {
    DistanceResult result;
    result.nbSentries = 0;
    // An array of members reached by `member`
    bool *wotMatches = new bool[mNodes.size()];
    // An array of members that are sentries
    bool *sentries = new bool[mNodes.size()];
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      // We will check only members with at least d_min links (other do not participate the distance rule)
      Node *node = mNodes.at(i);
      sentries[i] = node->isEnabled() && node->getNbIssued() >= d_min;
      wotMatches[i] = false;
    }
    // The member to check is not considered a sentry
    sentries[member] = false;
    wotMatches[member] = true;
    result.nbSuccess = 0;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      if (sentries[i]) {
        result.nbSentries++;
      }
    }
    uint32_t minimumRequired = int(floor(x_percent * result.nbSentries));
    findMatches(member, k_max, wotMatches, minimumRequired);
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      if (sentries[i]) {
        if (wotMatches[i]) {
//          Log() << "Sentry " << i << ": OK";
          result.nbSuccess++;
        } else {
//          Log() << "Sentry " << i << ": KO";
        }
      } else {
//        Log() << "NON-Sentry "  << i;
      }
    }
    delete[] wotMatches;
    delete[] sentries;

    // We override the result with X% rule
    result.isOutdistanced = result.nbSuccess < minimumRequired;
//    Log() << "Success = " << (result.isOutdistanced ? "false" : "true") << ": reached " << result.nbSuccess << " / " << x_percent * result.nbSentries << " (needed " << (x_percent * 100) << "% of " << result.nbSentries << ")";
    return result;
  }

  void WebOfTrust::findMatches(uint32_t m1, uint32_t k_max, bool *wotChecked, uint32_t minimumRequired) {
    if (k_max >= 1) {
      checkMatches(m1, 1, k_max, wotChecked, minimumRequired);
    }
  }

  void WebOfTrust::checkMatches(uint32_t m1, uint32_t distance, uint32_t distanceMax, bool *wotChecked, uint32_t minimumRequired) {
    if (levels.size() < distance) {
      levels.push_back(m1);
    }
    levels[distance - 1] = m1;
    // Mark as checked the linking nodes at this level
    for (uint32_t j = 0; j < mNodes.at(m1)->getNbLinks(); j++) {
      uint32_t by = getNodeIndex(mNodes.at(m1)->getLinkAt(j));
      wotChecked[by] = true;
    }
    uint32_t matchedCount = 0;
    if (distance < distanceMax) {
      // Look one level deeper
      for (uint32_t j = 0; j < mNodes.at(m1)->getNbLinks(); j++) {
        // Count how much we have reached yet
        for (uint32_t i = 0; i < getSize(); i++) {
          if (wotChecked[i]) {
            matchedCount++;
          }
          if (matchedCount >= minimumRequired) {
            return;
          }
        }
        // If not enough, we continue
        uint32_t by = getNodeIndex(mNodes.at(m1)->getLinkAt(j));
        checkMatches(by, distance + 1, distanceMax, wotChecked, minimumRequired);
      }
    }
  }

  WoTSet WebOfTrust::getSentries(int d_min) {
    WoTSet set;
    set.nbNodes = 0;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      Node *node = mNodes.at(i);
      if (node->isEnabled() && node->getNbIssued() >= d_min) {
        set.nbNodes++;
      }
    }
    set.nodes = new uint32_t[set.nbNodes];
    int j = 0;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      Node *node = mNodes.at(i);
      if (node->isEnabled() && node->getNbIssued() >= d_min) {
        set.nodes[j] = i;
        j++;
      }
    }
    return set;
  }

  WoTSet WebOfTrust::getNonSentries(int d_min) {
    WoTSet set;
    set.nbNodes = 0;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      Node *node = mNodes.at(i);
      if (node->isEnabled() && node->getNbIssued() < d_min) {
        set.nbNodes++;
      }
    }
    set.nodes = new uint32_t[set.nbNodes];
    int j = 0;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      Node *node = mNodes.at(i);
      if (node->isEnabled() && node->getNbIssued() < d_min) {
        set.nodes[j] = i;
        j++;
      }
    }
    return set;
  }

  std::vector<std::vector<uint32_t>> WebOfTrust::getPaths(uint32_t from, uint32_t to, uint32_t k_max) {
    std::vector<WotStep*> paths;
    std::vector<WotStep*> matchingPaths;
    std::vector<std::vector<uint32_t>> result;
    uint32_t* wotDistance = new uint32_t[mNodes.size()];
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      // A map to remember the fact we checked a node
      wotDistance[i] = k_max + 1;
    }
    wotDistance[to] = 0;
    WotStep* root = new WotStep();
    root->distance = 0;
    root->member = to;
    root->previous = NULL;
    paths.push_back(root);
    lookup(from, to, 1, k_max, root, &paths, &matchingPaths, wotDistance);
    // Formatting as vectors
    for (int i = 0; i < matchingPaths.size(); i++) {
      std::vector<uint32_t> thePath;
      WotStep* step = matchingPaths[i];
      while (step != NULL) {
        thePath.push_back(step->member);
        step = step->previous;
      }
      result.push_back(thePath);
    }
    // Clean memory
    for (int i = 0; i < paths.size(); i++) {
      delete paths[i];
    }
    // Result
    return result;
  }

  void WebOfTrust::lookup(uint32_t source, uint32_t target, uint32_t distance, uint32_t distanceMax, WotStep* previous, std::vector<WotStep*>* paths, std::vector<WotStep*>* matchingPaths, uint32_t* wotDistance) {
    if (source != target && distance <= distanceMax) {
      std::vector<WotStep*> local_paths;
      // Mark as checked the linking nodes at this level
      for (uint32_t j = 0; j < mNodes.at(target)->getNbLinks(); j++) {
        uint32_t by = getNodeIndex(mNodes.at(target)->getLinkAt(j));
        // Do not compute a same path twice if the distance is not shorter
        if (distance < wotDistance[by]) {
          wotDistance[by] = distance;
          WotStep* step = new WotStep();
          step->distance = distance;
          step->member = by;
          step->previous = previous;
          paths->push_back(step);
          local_paths.push_back(step);
          // WIN
          if (by == source) {
            matchingPaths->push_back(step);
          }
        }
      }
      if (distance <= distanceMax) {
        // Look one level deeper
        for (uint32_t j = 0; j < local_paths.size(); j++) {
          lookup(source, local_paths[j]->member, distance + 1, distanceMax, local_paths[j], paths, matchingPaths, wotDistance);
        }
      }
    }
  }
}
