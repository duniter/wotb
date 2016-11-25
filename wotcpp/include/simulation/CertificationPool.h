#ifndef CERTIFICATION_POOL_H
#define CERTIFICATION_POOL_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include "simulation/Certification.h"

namespace libsimu {

  class CertificationPool {

    public :

      CertificationPool();
      ~CertificationPool();

      void createNew(uint32_t from, uint32_t to, uint32_t date);

      map<Lien, Certification> certs;
      map<Lien, Certification> liens;
  };
}

#endif
