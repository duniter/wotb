#ifndef IDENTITY_POOL_H
#define IDENTITY_POOL_H

#include <cstdint>
#include <cstddef>
#include <map>
#include "simulation/Identity.h"


namespace libsimu {

  using namespace std;
  using namespace libwot;

  class IdentityPool {

    public :

      IdentityPool(uint32_t sigMoy, uint32_t sigStock, uint32_t sigQty);
      ~IdentityPool();

      void alimenteEnNouveaux(uint32_t valuesNewSize);
      void newcomer2member(Identity *identity);
      void allNewcomersBecomeMembers(WebOfTrust* wot);
      uint32_t nombreAleatoireGaussienDeMoyenneX(uint32_t x, int min, int max);
      uint32_t lastUID() { return counter; };

      uint32_t counter = 0;
      uint32_t sigQty = 0;
      uint32_t sigStock = 0;
      uint32_t sigMoy = 0;

      vector<Identity*> newcomers;
      vector<Identity*> members;
  };
}

#endif
