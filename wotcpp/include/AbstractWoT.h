#ifndef ABSTRACT_WEB_OF_TRUST_H
#define ABSTRACT_WEB_OF_TRUST_H

#include <cstdint>
#include <vector>
#include <string>

#include "webOfTrust.h"


namespace libwot {

  class AbstractWoT {

    public:
      virtual void reset() = 0;
      virtual void showWoT() = 0;
      virtual void showGraph() = 0;
      virtual uint32_t getWoTSize() = 0;
      virtual uint32_t addNode() = 0;
      virtual uint32_t removeNode() = 0;
      virtual void setMaxCert(uint32_t maxCert) = 0;
      virtual uint32_t getMaxCert() = 0;
      virtual bool isEnabled(uint32_t nodeIndex) = 0;
      virtual bool setEnabled(bool enabled, uint32_t nodeIndex) = 0;
      virtual bool existsLink(uint32_t from, uint32_t to) = 0;
      virtual uint32_t addLink(uint32_t from, uint32_t to) = 0;
      virtual uint32_t removeLink(uint32_t from, uint32_t to) = 0;
      virtual bool isOutdistanced(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent) = 0;
      // Data extraction
      virtual WoTSet getSentries(int d_min) = 0;
      virtual WoTSet getNonSentries(int d_min) = 0;
  };
}

#endif
