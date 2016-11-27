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
      counter = 0;
    }


    CertificationPool::~CertificationPool() {
    }

    void CertificationPool::createNew(Identity* from, Identity* to, uint32_t date) {
//      Log2() << "Nouvelle certif de UID " << from << " -> " << to;
      Certification* cert = new Certification();
      cert->dateOfIssuance = date;
      cert->emetteur = from;
      cert->receveur = to;
      cert->uniqueNumber = counter++;
      Lien link;
      link.first = from->uid;
      link.second = to->uid;
      cert->link = link;
      certs[to->uid].push_back(cert);
    }
}
