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
    stats = new Statistiques(NOMBRE_DE_BLOCKS_DE_SIMULATION);
  }

  /**
   * Mécanique principale qui ajoute des exclu les membres ne répondant plus aux critères et tente d'ajouter
   * un maximum de nouveaux membres selon les règles établies.
   */
  void Duniter::ajouteUnBloc() {
    Log() << "--------------- TOUR " << blocCourant << " ---------------";

    // Stats
    auto start = std::chrono::high_resolution_clock::now();
    statCourante = new StatsDuTour();
    iPool->statCourante = statCourante;
    cPool->statCourante = statCourante;

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
    statCourante->nombreDeMembresEnToile = wot->getEnabledCount();
    statCourante->nombreDeMembresPassesEnToile = wot->getSize();
    statCourante->nombreDeLiensEnToile = wot->getNbLinks();
    statCourante->nombreDeMembresEnAttenteFinDeTour = iPool->newcomers.size();
    statCourante->nombreDeMembresCertifsEnAttenteFinDeTour = cPool->getNbCertifsEnPiscine();
    statCourante->nombreDeMembresAjoutes = statCourante->nombreDeTentativesDAjoutMembreSucces - statCourante->nombreDeMembresExclusParManqueDeCertif;
    statCourante->tempsExecutionDuBloc = StatsDuTour::compteMicrosecondesDepuis(start);
  };

  void Duniter::creeBlockInitialEtSaCommunaute() {
    statCourante = new StatsDuTour();
    iPool->statCourante = statCourante;
    cPool->statCourante = statCourante;
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
//    Log() << setw(7) << statCourante->tempsExecutionSuppressionLiens << " µs pour <supprimeLien>";
//    Log() << setw(7) << statCourante->tempsExecutionMembreEmetUneCertifSiPossible << " µs pour <membreEmetUneCertifSiPossible>";
//    Log() << setw(7) << statCourante->tempsExecutionIntegrationLiensInternes << " µs pour <essaieIntegrerLiensInternes>";
//    Log() << setw(7) << statCourante->tempsExecutionIntegrationNouveauxVenus << " µs pour <essaieIntegrerNouveauxVenus>";
//    Log() << setw(7) << statCourante->tempsExecutionAlimenteEnNouveaux << " µs pour <alimenteEnNouveaux>";
    double variationN = statCourante->nombreDeMembresAjoutes;
    double variationL = statCourante->nombreDeTentativesDAjoutCertInterneSucces + statCourante->nombreDeTentativesDAjoutCertNouveauVenuSucces - statCourante->nombreDeCertifsExpirees;
    Log() << setw(7) << statCourante->nombreDeMembresEnToile << " Membres en toile (" << (variationN >= 0 ? "+" : "") << variationN << ")";
    Log() << setw(7) << statCourante->nombreDeLiensEmisPourEtreSentry << " Liens émis pour être point de contrôle";
    Log() << setw(7) << statCourante->nombreDeSentries << " Points de contrôles (xPercent = " << X_PERCENT*100 << "% => " <<  int(floor(statCourante->nombreDeSentries * X_PERCENT)) << " points effectifs)";
    Log() << setw(7) << statCourante->nombreDeLiensEnToile << " Liens en toile (" << (variationL >= 0 ? "+" : "") << variationL << ")";
    Log() << setw(7) << statCourante->nombreDeTentativesDAjoutMembre << " Tentatives d'ajout de membre";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutMembreSucces << " Succès";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutMembreEchouees << " Echecs";
    Log() << setw(7) << "" << "    * dont " << statCourante->nombreDeTentativesDAjoutMembreEchoueesParQteLiens << " par le nombre de liens";
    Log() << setw(7) << "" << "    * dont " << statCourante->nombreDeTentativesDAjoutMembreEchoueesParDistance << " par la distance";
    Log() << setw(7) << statCourante->nombreDeTentativesDAjoutCertInterne + statCourante->nombreDeTentativesDAjoutCertNouveauVenu << " Tentatives d'intégration de lien en toile";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertInterneSucces << " Succès (interne)";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuSucces<< " Succès (nouveau venu)";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertInterneEchouees << " Echecs (interne)";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertInterneEchoueesParAdhesion << " par l'adhésion";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertInterneEchoueesParStock<< " par le stock";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchouees<< " Echecs (nouveau venu)";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParQteLiens << " par le nombre de liens";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParDistance << " par la distance";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParStock<< " par le stock";
    Log() << setw(7) << statCourante->nombreDeMembresExclusParManqueDeCertif << " Membres exclus (par nombre de liens)";
    Log() << setw(7) << statCourante->nombreDeCertifsExpirees << " Liens détruits en toile (expiration)";
    Log() << setw(7) << statCourante->nombreDeMembresEnAttenteFinDeTour << " Identités en piscine";
    Log() << setw(7) << "" << "  * -" << statCourante->nombreDeTentativesDAjoutMembreSucces << " Identités devenues membres";
    Log() << setw(7) << "" << "  * +" << statCourante->nombreDIdentitesGenereesEnPisicine << " Identités nouvelles générées";
    Log() << setw(7) << statCourante->nombreDeMembresCertifsEnAttenteFinDeTour << " Certifications en piscine";
    Log() << setw(7) << "" << "  * -" << statCourante->nombreDeCertifsTransfereesEnToile << " Transférées en toile";
    Log() << setw(7) << "" << "  * +" << statCourante->nombreDeCertifsGenereesEnPisicine << " Nouvellement émises";
    Log() << setw(7) << "  --------";
    Log() << setw(7) << statCourante->tempsExecutionDuBloc << " µs pour l'ajout du bloc";
    Log() << setw(7) << "  --------";
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
