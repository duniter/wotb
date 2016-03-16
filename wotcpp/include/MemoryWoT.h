#ifndef MEMORY_WEB_OF_TRUST_H
#define MEMORY_WEB_OF_TRUST_H

#include <cstdint>
#include <vector>
#include <string>

#include "AbstractWoT.h"


namespace libwot {

  class MemoryWoT : public AbstractWoT {

    public:
      MemoryWoT();
      ~MemoryWoT();
      virtual void reset();
      virtual void showWoT();
      virtual void showGraph();
      virtual uint32_t getWoTSize();
      virtual uint32_t addNode();
      virtual uint32_t removeNode();
      virtual bool isEnabled(uint32_t nodeIndex);
      virtual bool setEnabled(bool enabled, uint32_t nodeIndex);
      virtual bool existsLink(uint32_t from, uint32_t to);
      virtual uint32_t addLink(uint32_t from, uint32_t to);
      virtual uint32_t removeLink(uint32_t from, uint32_t to);
      virtual bool isOutdistanced(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent);

    private:
      WebOfTrust* wot;
  };
}

#endif
