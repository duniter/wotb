#ifndef WEB_OF_TRUST_H
#define WEB_OF_TRUST_H

#include <cstdint>
#include <vector>
#include <string>

#include "node.h"
#include "distanceResult.h"
#include "wotStep.h"


namespace libwot {

  class WebOfTrust {

    public :

      static WebOfTrust* createRandom(uint32_t nbMembers, uint32_t maxCertStock);
      static WebOfTrust* readFromDisk(std::string filename);
      WebOfTrust(uint32_t maxCert);
      ~WebOfTrust();

      void setMaxCert(uint32_t maxCert) { mMaxCert = maxCert; };
      uint32_t getMaxCert() {return mMaxCert;};
      uint32_t getNodeIndex(Node* node) const ;
      uint32_t getSize() { return mNodes.size();};
      uint32_t getEnabledCount() {
        int count = 0;
        for (size_t n = 0; n < mNodes.size(); ++n) {
          if (mNodes[n]->isEnabled()) {
            ++count;
          }
        }
        return count;
      };
      Node* getNodeAt(uint32_t index) { return mNodes.at(index);};

	  /**
		\brief Provide a node randomly
		\note slow method if repeatedly called
	  */
      Node* getRandomNode();

      Node* addNode();
      void removeNode();

      void reset();

      WebOfTrust* clone();
      WebOfTrust* showTable();
      std::string dump();
      WebOfTrust* showGraphviz();

      bool writeToDisk(std::string filename);

      DistanceResult computeDistance(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent);
      void findMatches(uint32_t m1, uint32_t k_max, bool *wotChecked);
      void checkMatches(uint32_t m1, uint32_t distance, uint32_t distanceMax, bool *wotChecked);

      // Data extraction
      std::vector<uint32_t> getSentries(int d_min);
      std::vector<uint32_t> getNonSentries(int d_min);
      std::vector<uint32_t> getDisabled();
      std::vector<std::vector<uint32_t>> getPaths(uint32_t from, uint32_t to, uint32_t k_max) const ;

    private :

      void lookup(uint32_t from, uint32_t to, uint32_t distance, uint32_t distanceMax, WotStep* previous, std::vector<WotStep*>*, std::vector<WotStep*>* matchingPaths, uint32_t* wotChecked) const ;
      std::vector<Node*> mNodes;	
      uint32_t mMaxCert;
  };
}

#endif
