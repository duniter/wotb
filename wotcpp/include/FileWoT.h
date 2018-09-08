#ifndef FILE_WEB_OF_TRUST_H
#define FILE_WEB_OF_TRUST_H

#include <cstdint>
#include <vector>
#include <string>

#include "AbstractWoT.h"


namespace libwot {

  class FileWoT : public AbstractWoT {
    public:
      FileWoT(std::string f);
      FileWoT(std::string f, WebOfTrust* newWoT);
      ~FileWoT();
      void reset();
      WebOfTrust* cloneWoT();
      void showWoT();
      std::string dumpWoT();
      void showGraph();
      uint32_t getWoTSize();
      uint32_t addNode();
      uint32_t removeNode();
      void setMaxCert(uint32_t maxCert);
      uint32_t getMaxCert();
      bool isEnabled(uint32_t nodeIndex);
      bool setEnabled(bool enabled, uint32_t nodeIndex);
      bool existsLink(uint32_t from, uint32_t to);
      uint32_t addLink(uint32_t from, uint32_t to);
      uint32_t removeLink(uint32_t from, uint32_t to);
      DistanceResult computeDistance(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent);
      // Data extraction
      std::vector<uint32_t> getSentries(int d_min);
      std::vector<uint32_t> getNonSentries(int d_min);
      std::vector<uint32_t> getDisabled();
      std::vector<std::vector<uint32_t>> getPaths(uint32_t from, uint32_t to, uint32_t k_max);

    private:
      std::string filename;
  };
}

#endif
