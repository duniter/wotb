#ifndef NODE_H
#define NODE_H

#include <cstdint>
#include <cstddef>
#include <vector>


namespace libwot {

  class WebOfTrust;

  class Node {

    public :

      Node(WebOfTrust* wot);
      ~Node();

      bool isEnabled() {return mEnabled;};
      uint32_t getNbLinks() {return mCert.size();};
      Node* getLinkAt(uint32_t index) {return mCert.at(index);};
      uint32_t getNbIssued() {return mNbIssued;};


      Node* setEnabled(bool enable);
      std::vector<Node*> getLinks() {return mCert;};

      bool addLinkTo(uint32_t to);
      bool addLinkTo(Node* to);
      bool hasLinkTo(uint32_t to);
      bool hasLinkTo(Node* to);
      bool hasLinkFrom(uint32_t from);
      bool hasLinkFrom(Node* from);
      void removeLinkTo(uint32_t to);
      std::vector<Node*> mCert;

    private :

      WebOfTrust *mWot;
      bool mEnabled;
      uint32_t mNbIssued;
  };
}

#endif
