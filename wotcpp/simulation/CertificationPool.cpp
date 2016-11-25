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
      certs = vector<vector<Certification*>>(10000000);
      liens = vector<vector<Certification*>>(10000000);
    }


    CertificationPool::~CertificationPool() {
    }

    void CertificationPool::createNew(uint32_t from, uint32_t to, uint32_t date) {
//      Log2() << "Nouvelle certif de UID " << from << " -> " << to;
      Certification* cert = new Certification();
      cert->dateOfIssuance = date;
      Lien link;
      link.first = from;
      link.second = to;
      cert->link = link;
      certs[to].push_back(cert);
    }
}
