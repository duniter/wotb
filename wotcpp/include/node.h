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
      Node(WebOfTrust* wot, uint32_t pIndex);
      ~Node();

      bool isEnabled() {return mEnabled;};
      uint32_t getNbLinks() {return mCert.size();};
      Node* getLinkAt(uint32_t index) {return mCert.at(index);};
      uint32_t getNbIssued() {return mNbIssued;};


      Node* setEnabled(bool enable);
	  const std::vector<Node*> & getLinks() const {return mCert;};

      bool addLinkTo(uint32_t to);
      bool addLinkTo(Node* to);
      bool hasLinkTo(uint32_t to);
      bool hasLinkTo(Node* to);
      bool hasLinkFrom(uint32_t from);
      bool hasLinkFrom(Node* from);
      void removeLinkTo(uint32_t to);

	  uint32_t get_index() const { return mIndex ; }
	  void set_index(uint32_t pIndex) ;

    private :

      WebOfTrust *mWot;
      bool mEnabled;
      std::vector<Node*> mCert;
      uint32_t mNbIssued;

	  /// \brief Index of the node in the WoT
	  uint32_t mIndex ;
  };
}

#endif
