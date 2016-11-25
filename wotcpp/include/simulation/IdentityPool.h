#ifndef IDENTITY_POOL_H
#define IDENTITY_POOL_H

#include <cstdint>
#include <cstddef>
#include <map>
#include "simulation/Identity.h"


namespace libsimu {

  using namespace std;

  class IdentityPool {

    public :

      IdentityPool(uint32_t sigMoy, uint32_t sigStock);
      ~IdentityPool();

      void alimenteEnNouveaux(uint32_t valuesNewSize);
      uint32_t nombreAleatoireGaussienDeMoyenneX(uint32_t x, int min, int max);

      uint32_t counter = 0;
      uint32_t sigStock = 0;
      uint32_t sigMoy = 0;

      vector<Identity*> newcomers;
      vector<Identity*> members;
  };
}

#endif
