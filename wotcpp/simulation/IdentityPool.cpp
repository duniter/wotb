#include <Duniter.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "simulation/Identity.h"
#include "webOfTrust.h"
#include "log.h"

namespace libsimu {

  using namespace std;
  using namespace libsimu;

    IdentityPool::IdentityPool(uint32_t sigMoy, uint32_t sigStock, uint32_t sigQty) : sigMoy(sigMoy), sigStock(sigStock), sigQty(sigQty) {
      newcomers = vector<Identity*>();
    }

    IdentityPool::~IdentityPool() {
    }

    void IdentityPool::alimenteEnNouveaux(uint32_t valuesNewSize) {
      auto start = std::chrono::high_resolution_clock::now();
      for (unsigned long i = newcomers.size(); i < valuesNewSize; i++) {
        uint32_t sigPersoCible = sigStock; // Le maximum possible
//        uint32_t sigPersoCible = nombreAleatoireGaussienDeMoyenneX(sigMoy, 0, sigStock);
        Identity* newcomer = new Identity(sigPersoCible);
        newcomer->uid = counter++;
        newcomer->wotb_id = 0; // La valeur doit être écrasée systématiquement
        newcomer->joinDate = 0;
        newcomers.push_back(newcomer);
        statCourante->nombreDIdentitesGenereesEnPisicine++;
      }
      statCourante->tempsExecutionAlimenteEnNouveaux = StatsDuTour::compteMicrosecondesDepuis(start);
    }

    void IdentityPool::newcomer2member(Identity *identity) {
      // Retire de la liste des newcomers
      vector<Identity*>::iterator it = std::find(newcomers.begin(), newcomers.end(), identity);
      if (it != newcomers.end()) {
        newcomers.erase(it);
      }
      // Ajoute dans la liste des membres
      members.push_back(identity);
      statCourante->nombreDeTentativesDAjoutMembreSucces++;
    };

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

    void IdentityPool::allNewcomersBecomeMembers(WebOfTrust* wot) {
      for (int i = 0; i < newcomers.size(); i++) {
        wot->addNode();
        // Affecte un identifiant wotb
        Identity* nouveau = newcomers[i];
        nouveau->wotb_id = wot->getSize() - 1;
        nouveau->estMembre = true;
        nouveau->aEteMembre = true;
        nouveau->wotb_node = wot->getNodeAt(nouveau->wotb_id);
        newcomer2member(nouveau);
        i--;
      }
    }
}
