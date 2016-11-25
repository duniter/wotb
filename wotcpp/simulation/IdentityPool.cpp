#include <Duniter.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "simulation/Identity.h"
#include "webOfTrust.h"
#include "log.h"

namespace libsimu {

  using namespace std;
  using namespace libsimu;

    IdentityPool::IdentityPool(uint32_t sigMoy, uint32_t sigStock) : sigMoy(sigMoy), sigStock(sigStock) {
      newcomers = vector<Identity*>();
    }

    IdentityPool::~IdentityPool() {
    }

    void IdentityPool::alimenteEnNouveaux(uint32_t valuesNewSize) {
      for (unsigned long i = newcomers.size(); i < valuesNewSize; i++) {
        uint32_t sigPersoCible = sigStock; // Le maximum possible
//        uint32_t sigPersoCible = nombreAleatoireGaussienDeMoyenneX(sigMoy, 0, sigStock);
        Identity* newcomer = new Identity(sigPersoCible);
        newcomer->uid = counter++;
        newcomer->wotb_id = 0; // La valeur doit être écrasée systématiquement
        newcomer->joinDate = 0;
        newcomers.push_back(newcomer);
      }
    }

    uint32_t IdentityPool::nombreAleatoireGaussienDeMoyenneX(uint32_t x, int min, int max) {
      std::random_device rd;
      std::mt19937 eng(rd());
      std::normal_distribution<> distr(x, x / 2);
      double number = -1;
      // Max 1000 essais de trouver un nombre entre min et max, de moyenne x
      for (int i = 0; (number < min || number > max) && i < 1000; i++) {
        number = distr(eng);
      }
      return number;
    }
}
