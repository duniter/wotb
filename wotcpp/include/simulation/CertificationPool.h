#ifndef CERTIFICATION_POOL_H
#define CERTIFICATION_POOL_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include "simulation/Identity.h"
#include "simulation/Certification.h"

namespace libsimu {

  class CertificationPool {

    public :

      CertificationPool();
      ~CertificationPool();

      void createNew(Identity* from, Identity* to, uint32_t date);

      uint32_t counter;
      vector<Certification*> pendingVersMembre;
      vector<Certification*> pendingVersNonMembre;
      vector<vector<Certification*>> certs;
      vector<vector<Certification*>> liens;
  };
}

#endif
