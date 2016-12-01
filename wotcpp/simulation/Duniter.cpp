#include <Duniter.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "simulation/IdentityPool.h"
#include "webOfTrust.h"
#include "log.h"

namespace libsimu {

  using namespace std;
  using namespace libwot;

  Duniter::Duniter(int dureeSimulationEnBlocks, double xPercent, uint32_t stepMax, uint32_t minNew, double maxNewPercent, uint32_t sigMoy, uint32_t sigStock, uint32_t sigQty, uint32_t sigPeriod) {
    NOMBRE_DE_BLOCKS_DE_SIMULATION = dureeSimulationEnBlocks;
    X_PERCENT = xPercent;
    STEPMAX = stepMax;
    MIN_NEW = minNew;
    MAX_NEW_PERCENT = maxNewPercent;
    SIG_STOCK = sigStock;
    SIG_QTY = sigQty;
    SIG_PERIOD = sigPeriod;
    SIG_VALIDITY = sigStock * SIG_PERIOD;
    COMMUNAUTE_INITIALE = sigQty + 1;
    wot = new WebOfTrust(sigStock);
    iPool = new IdentityPool(sigMoy, sigStock, sigQty);
    cPool = new CertificationPool(NOMBRE_DE_BLOCKS_DE_SIMULATION, X_PERCENT, STEPMAX, MIN_NEW, MAX_NEW_PERCENT, sigMoy, SIG_STOCK, SIG_QTY, SIG_PERIOD);
  }

  /**
   * Mécanique principale qui ajoute des exclu les membres ne répondant plus aux critères et tente d'ajouter
   * un maximum de nouveaux membres selon les règles établies.
   */
  void Duniter::ajouteUnBloc() {
    auto start = std::chrono::high_resolution_clock::now();

    Log() << "--------------- TOUR " << blocCourant << " ---------------";
    cPool->essaieIntegrerLiensInternes(iPool);
    cPool->essaieIntegrerNouveauxVenus(wot, iPool);

    blocCourant++;

    if (blocCourant < NOMBRE_DE_BLOCKS_DE_SIMULATION) {
      // Le temps s'est incrémenté, conséquences :
      cPool->faitExpirerLesLiens(blocCourant);
      // TODO: actualisation tous les X periodes => Test de distance
      // TODO expiration des certifications en piscine
      alimenteLesPiscines();
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    Log() << setw(7) << "   -------";
    Log() << setw(7) << microseconds << " µs pour l'ajout du bloc";
  };

  void Duniter::creeBlockInitialEtSaCommunaute() {
    iPool->alimenteEnNouveaux(COMMUNAUTE_INITIALE);
    for (int i = 0; i < iPool->newcomers.size(); i++) {
      // Each initial member certifies the SIG_QTY following initial members
      Identity* from = iPool->newcomers[i];
      for (int j = 0; j < SIG_QTY; j++) {
        // Certify next newcomer, and loop on first when necessary
        Identity* to = iPool->newcomers[(i + j + 1) % COMMUNAUTE_INITIALE];
        to->derniereEmissionDeCertif = 0; // t = 0
        cPool->createNew(from, to, 0);
      }
    }
    // Ajoute les membres initiaux
    iPool->allNewcomersBecomeMembers(wot);
    // Et leurs certifications
    cPool->allPendingsToLinks();
  }

  void Duniter::afficheStats() {
//      wot->showTable();
    Log() << setw(7) << "   -------";
    Log() << "Nouveaux membres en attente : " << iPool->newcomers.size();
    Log() << "Nombre de membres : " << wot->getEnabledCount();
    Log() << "Nombre d'individus passés : " << wot->getSize();
    Log();
  };

  void Duniter::alimenteLesPiscines() {

    /*********************
     * NOUVEAUX VENUS
     *
     * Stratégie d'arrivée
     * -------------------
     *
     * Il existe toujours *a minima* MIN_NEW nouveaux membre, et *a maxima* MAX_NEW_PERCENT % de membres supplémentaires
     * de la taille actuelle de la toile de confiance.
     */
    int taillePiscineDeMembres = max(int(MIN_NEW), int(wot->getSize() * MAX_NEW_PERCENT));
    iPool->alimenteEnNouveaux(taillePiscineDeMembres);

    /*********************
     *   CERTIFICATIONS
     */
    cPool->emetDeNouvellesCertifications(iPool, blocCourant);
  };
}
