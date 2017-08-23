#include <node.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <limits>

#include "include/webOfTrust.h"
#include "include/log.h"

namespace libwot {

  using namespace std;

  Node::Node(WebOfTrust* wot) {
    mWot = wot;
    mEnabled = true;
    mNbIssued = 0;
    mCert = vector<Node*>();
	mCert.reserve(wot->getMaxCert()) ;
	mIndex = numeric_limits<int>::max() ;
  }

  Node::Node(WebOfTrust* wot, uint32_t pIndex) {
    mWot = wot;
    mEnabled = true;
    mNbIssued = 0;
    mCert = vector<Node*>();
	mCert.reserve(wot->getMaxCert()) ;
	mIndex = pIndex ;
  }

  Node::~Node() {
  }


  Node* Node::setEnabled(bool enabled) {
    mEnabled = enabled;
    return this;
  }


  bool Node::addLinkTo(Node* to) {
    if(mNbIssued >= mWot->getMaxCert() ||
      to == this ||
      to->hasLinkFrom(this)) {
        return false;
    }

    to->mCert.push_back(this);
    mNbIssued++;
    return true;
  }


  bool Node::addLinkTo(uint32_t to) {
    Node *node = mWot->getNodeAt(to);
    return addLinkTo(node);
  }


  bool Node::hasLinkTo(uint32_t to) {
    if (to >= mWot->getSize()) return false;
    Node *node = mWot->getNodeAt(to);
    return hasLinkTo(node);
  }


  bool Node::hasLinkTo(Node* to) {
    return to->hasLinkFrom(this);
  }


  bool Node::hasLinkFrom(uint32_t from) {
    Node* node = mWot->getNodeAt(from);
    return node->hasLinkFrom(node);
  }


  bool Node::hasLinkFrom(Node* from) {
    if(find(mCert.begin(), mCert.end(), from) == mCert.end()) {
      return false;
    }
    return true;
  }


  void Node::removeLinkTo(uint32_t to) {
    Node* node = mWot->getNodeAt(to);
    vector<Node*>::iterator it = find(node->mCert.begin(), node->mCert.end(), this);
    if (it != mCert.end()) {
      node->mCert.erase(it);
      mNbIssued--;
    }
  }

	void Node::set_index(uint32_t pIndex)
	{
		mIndex = pIndex ;
	}
}

