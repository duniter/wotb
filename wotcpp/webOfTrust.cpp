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
#include <vector>

namespace libwot {

  using namespace std;


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
	wot->mNodes.reserve(nbMembers) ;
    for (uint32_t i = 0; i < nbMembers; i++) {
      wot->addNode();
    }

	// Random number generator
	std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, nbMembers > 0 ? nbMembers - 1 : 0);

    for(vector<Node*>::iterator it = wot->mNodes.begin(); it != wot->mNodes.end(); it++) {
      Node* certified = NULL;
      for (uint32_t i = 0; i < maxCertStock; i++) {
        do{
          certified = wot->mNodes.at(distr(eng)) ;
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
	  Node *node = new Node(this, mNodes.size());
    mNodes.push_back(node);
    return node;
  }


  void WebOfTrust::removeNode() {
    delete mNodes[mNodes.size() - 1];
    mNodes.pop_back();
  }


  uint32_t WebOfTrust::getNodeIndex(Node* node) const {
	  if (node->get_index() != UINT32_MAX)
		  return node->get_index() ;
	  else
	  {
		vector<Node*>::const_iterator it = find(mNodes.begin(), mNodes.end(), node);
		return distance(mNodes.begin(), it);
	  }
  }


  Node* WebOfTrust::getRandomNode() {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, mNodes.size() > 0 ? mNodes.size() - 1 : 0);
    return mNodes.at(distr(eng));
  }


  WebOfTrust* WebOfTrust::clone() {
    WebOfTrust* clone = new WebOfTrust(this->mMaxCert);
    // Create the nodes
    for (uint32_t i = 0; i < this->mNodes.size(); i++) {
      clone->addNode();
      clone->getNodeAt(i)->setEnabled(this->mNodes[i]->isEnabled());
    }
    // Create the links
    for (uint32_t i = 0; i < this->mNodes.size(); i++) {
      Node* node = this->mNodes[i];
      Node* clonedNode = clone->mNodes[i];
      for (uint32_t j = 0; j < node->getLinks().size(); j++) {
        uint32_t to = j;
        Node* sourceNode = node->getLinkAt(j);
        uint32_t from = getNodeIndex(sourceNode);
        clone->getNodeAt(from)->addLinkTo(clonedNode);
      }
    }
    return clone;
  }


  WebOfTrust* WebOfTrust::showTable() {
    cout << this->dump();
    return this;
  }


  std::string WebOfTrust::dump() {
    std::stringstream theDump;
    int fieldWidth = (mNodes.size() >= 1) ? countDigits(mNodes.size() - 1) : 1;
    int i = 0;
    theDump << "[" << setw(fieldWidth) << right << "M" << "] [E] [R] [" << setw(fieldWidth) << right << "I" << "] -> " << "Links[maxCert = " << mMaxCert << "]" << endl;
    for (vector<Node*>::iterator itNode = mNodes.begin(); itNode != mNodes.end(); itNode++) {
      Node* node = (Node*)*itNode;
      theDump << "[" << setw(fieldWidth) << right << i++ << "] [" << node->isEnabled() << "] [" << node->getNbLinks() << "] [" << setw(fieldWidth) << right << node->getNbIssued() << "] -> ";

      vector<Node*> links = node->getLinks();
      for(vector<Node*>::iterator itLink = links.begin(); itLink != links.end(); itLink++) {
        Node* certified = ((Node*)*itLink);
        theDump << setw(fieldWidth) << right << getNodeIndex(certified) << " | ";
      }

      theDump << endl;
    }

    return theDump.str();
  }


  WebOfTrust* WebOfTrust::showGraphviz() {
    int i = 0;
    cout << "digraph G {" << endl << endl;
    for(vector<Node*>::iterator itNode = mNodes.begin(); itNode != mNodes.end(); itNode++) {
      vector<Node*> links = ((Node*)*itNode)->getLinks();
      for(vector<Node*>::iterator itLink = links.begin(); itLink != links.end(); itLink++) {
        cout << "    " << getNodeIndex((Node*)*itLink) << " -> " << i << "" << endl;
      }
      i++;
    }
    cout << "}" << endl;

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
      sentries[i] = node->isEnabled() && node->getNbIssued() >= d_min && node->getNbLinks() >= d_min;
      wotMatches[i] = false;
    }
    // The member to check is not considered a sentry
    sentries[member] = false;
    wotMatches[member] = true;
    result.nbSuccess = 0;
    findMatches(member, k_max, wotMatches);
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      if (sentries[i]) {
        result.nbSentries++;
        if (wotMatches[i]) {
          Log() << "Sentry " << i << ": OK";
          result.nbSuccess++;
        } else {
          Log() << "Sentry " << i << ": KO";
        }
      } else {
        Log() << "NON-Sentry "  << i;
      }
    }
    result.isOutdistanced = result.nbSuccess < result.nbSentries;
    delete[] wotMatches;
    delete[] sentries;

    // We override the result with X% rule
    Log() << "Success = " << result.nbSuccess << " / " << x_percent * result.nbSentries << " (" << x_percent << " x " << result.nbSentries << ")";
    result.isOutdistanced = result.nbSuccess < x_percent * result.nbSentries;
    return result;
  }

  void WebOfTrust::findMatches(uint32_t m1, uint32_t k_max, bool *wotChecked) {
    if (k_max >= 1) {
      checkMatches(m1, 1, k_max, wotChecked);
    }
  }

  void WebOfTrust::checkMatches(uint32_t m1, uint32_t distance, uint32_t distanceMax, bool *wotChecked) {
    // Mark as checked the linking nodes at this level
    for (uint32_t j = 0; j < mNodes.at(m1)->getNbLinks(); j++) {
      uint32_t by = getNodeIndex(mNodes.at(m1)->getLinkAt(j));
      Log() << "Match " << by << " -> " << m1;
      wotChecked[by] = true;
    }
    if (distance < distanceMax) {
      // Look one level deeper
      for (uint32_t j = 0; j < mNodes.at(m1)->getNbLinks(); j++) {
        uint32_t by = getNodeIndex(mNodes.at(m1)->getLinkAt(j));
        checkMatches(by, distance + 1, distanceMax, wotChecked);
      }
    }
  }

  vector<uint32_t> WebOfTrust::getSentries(int d_min) {
    vector<uint32_t> set;
	set.reserve(mNodes.size()) ;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      Node *node = mNodes.at(i);
      if (node->isEnabled() && d_min >= 0 && node->getNbIssued() >= (uint32_t)d_min && node->getNbLinks() >= (uint32_t)d_min) {
        set.push_back(i);
      }
    }
    return set;
  }

  vector<uint32_t> WebOfTrust::getNonSentries(int d_min) {
    vector<uint32_t> set;
	set.reserve(mNodes.size()) ;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      Node *node = mNodes.at(i);
      if (node->isEnabled() && d_min >= 0 && (node->getNbIssued() < (uint32_t)d_min || node->getNbLinks() < (uint32_t)d_min)) {
        set.push_back(i);
      }
    }
    return set;
  }

  std::vector<uint32_t> WebOfTrust::getDisabled() {
    vector<uint32_t> set;
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      Node *node = mNodes.at(i);
      if (!node->isEnabled()) {
        set.push_back(i);
      }
    }
    return set;
  }

  std::vector<std::vector<uint32_t>> WebOfTrust::getPaths(uint32_t from, uint32_t to, uint32_t k_max) const {
    std::vector<WotStep*> paths;
    std::vector<WotStep*> matchingPaths;
    std::vector<std::vector<uint32_t>> result;
    uint32_t* wotDistance = new uint32_t[mNodes.size()];
    for (uint32_t i = 0; i < mNodes.size(); i++) {
      // A map to remember the fact we checked a node
      wotDistance[i] = k_max + 1;
    }
    wotDistance[to] = 0;
    
	// Initial step
	WotStep* root = new WotStep();
    root->distance = 0;
    root->member = to;
    root->previous = NULL;
    paths.push_back(root);

    lookup(from, to, 1, k_max, root, &paths, &matchingPaths, wotDistance) ;
	
	result.reserve(matchingPaths.size()) ;

	// Formatting as vectors
    for (size_t i = 0; i < matchingPaths.size(); ++i) 
	{
      std::vector<uint32_t> thePath ;
	  thePath.reserve(k_max+1) ;
      WotStep* step = matchingPaths[i];
      while (step != NULL) 
	  {
		thePath.push_back(step->member) ;
        step = step->previous;
      }
      result.push_back(thePath);
    }
    // Clean memory
    delete[] wotDistance;
    for (size_t i = 0; i < paths.size(); i++) {
      delete paths[i];
    }
    // Result
    return result;
  }

  void WebOfTrust::lookup(uint32_t source, uint32_t target, uint32_t distance, uint32_t distanceMax, WotStep* previous, std::vector<WotStep*>* paths, std::vector<WotStep*>* matchingPaths, uint32_t* wotDistance) const 
  {
    if (source != target && distance <= distanceMax) 
	{
      std::vector<WotStep*> local_paths;
      // Mark as checked the linking nodes at this level
      for (uint32_t j = 0; j < mNodes.at(target)->getNbLinks(); ++j) {
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
