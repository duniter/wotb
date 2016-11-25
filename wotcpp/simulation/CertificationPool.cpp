#include <Duniter.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "simulation/CertificationPool.h"
#include "log.h"

namespace libsimu {

  using namespace std;
  using namespace libsimu;

    CertificationPool::CertificationPool() {
    }


    CertificationPool::~CertificationPool() {
    }

    void CertificationPool::createNew(uint32_t from, uint32_t to, uint32_t date) {
      Log() << "Nouvelle certif de UID " << from << " -> " << to;
      Certification cert;
      cert.dateOfIssuance = date;
      Lien link;
      link.first = from;
      link.second = to;
      cert.link = link;
      certs[link] = cert;
    }
}
