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
      ~FileWoT();
      void reset();
      void showWoT();
      void showGraph();
      uint32_t getWoTSize();
      uint32_t addNode();
      uint32_t removeNode();
      void setMaxCert(uint32_t maxCert);
      bool isEnabled(uint32_t nodeIndex);
      bool setEnabled(bool enabled, uint32_t nodeIndex);
      bool existsLink(uint32_t from, uint32_t to);
      uint32_t addLink(uint32_t from, uint32_t to);
      uint32_t removeLink(uint32_t from, uint32_t to);
      bool isOutdistanced(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent);

    private:
      std::string filename;
  };
}

#endif
