#include <iostream>
#include <map>
#include "include/webOfTrust.h"
#include "include/simulation/Duniter.h"
#include "include/log.h"

#define WOT_FILE "wot.bin"

using namespace std;
using namespace libwot;
using namespace libsimu;

int main(int argc, char **argv) {
  srand(time(NULL));


  int32_t nbMembers = 8;
  int32_t maxCertStock = 3;

  if (nbMembers < maxCertStock) {
    cout << "NbMembers must be > maxCertStock" << endl;
    return 1;
  }

  // Allocating
//  WebOfTrust* wot = new WebOfTrust(3);
//
//  wot->addNode();
//  wot->addNode();
//  wot->addNode();
//  wot->addNode();
//  wot->addNode();
//  wot->addNode();
//
//  wot->getNodeAt(1)->addLinkTo((uint32_t)0);
//  wot->getNodeAt(2)->addLinkTo((uint32_t)0);
//  wot->getNodeAt(3)->addLinkTo((uint32_t)0);
//  wot->getNodeAt(4)->addLinkTo((uint32_t)0);
//  wot->getNodeAt(5)->addLinkTo((uint32_t)0);
//  // Add a path from 0 to 1
//  wot->getNodeAt(0)->addLinkTo((uint32_t)2);
//  wot->getNodeAt(2)->addLinkTo((uint32_t)3);
//  wot->getNodeAt(3)->addLinkTo((uint32_t)1);
//  // Add a path from 0 to 1
//  wot->getNodeAt(0)->addLinkTo((uint32_t)4);
//  wot->getNodeAt(4)->addLinkTo((uint32_t)1);

    // File WoT
//    WebOfTrust *wot = WebOfTrust::readFromDisk("/home/cgeek/.config/duniter/rml8/wotb.bin");

//  Log::setEnabled(true);


//  wot->showTable();
//  Log() << "Outdistanced = " << wot->computeDistance(56, 3, 5, 0.9).isOutdistanced;
//  wot->showGraphviz();
//  Log() << "Sentries count = " << wot->getSentries(3).nbNodes;
//  Log() << "Non-Sentries count = " << wot->getNonSentries(3).nbNodes;

//  uint32_t by = 25;
//  uint32_t target = 210;
//  uint32_t k_max = 5;
//  std::vector<std::vector<uint32_t>> steps = wot->getPaths(by, target, k_max);
//  if (steps.size() == 0) {
//    Log() << target << " is NOT reachable by " << by;
//  } else {
//    for (int i = 0; i < steps.size(); i++) {
//      cout << target << " is reached by " << by << " through: ";
//      for (int j = 0; j < steps[i].size(); j++) {
//        if (j > 0) {
//          cout << " -> ";
//        }
//        cout << steps[i][j];
//      }
//      cout << endl;
//    }
//  }

//  delete wot;

  // Paramètres voulus
  const double POURCENTAGE_DE_NOUVEAUX_MEMBRES_MAXI = 0.05;
  const double X_PERCENT = 1.0;
  const uint32_t DIVISIONS_PAR_UNITE_TEMPS = 1; // 1 unité de temps = 1 jour
  const uint32_t MINIMUM_DE_NOUVEAUX_VENUS_EN_PERMANENCE = 2;
  const uint32_t SIG_STOCK = 10;
  const uint32_t SIG_QTY = 1;
  const uint32_t STEP_MAX = 3;
  const uint32_t SIG_PERIOD = 1 * DIVISIONS_PAR_UNITE_TEMPS; // 1 jour
  const uint32_t SIG_MOY = SIG_STOCK / 3;

  /*****************************
   * SIMULATEUR DE NŒUD DUNITER
   ****************************/

  Duniter* simulateur = new Duniter(X_PERCENT, STEP_MAX, MINIMUM_DE_NOUVEAUX_VENUS_EN_PERMANENCE, POURCENTAGE_DE_NOUVEAUX_MEMBRES_MAXI, SIG_MOY, SIG_STOCK, SIG_QTY, SIG_PERIOD);
//  const uint32_t NOMBRE_DE_BLOCKS_DE_SIMULATION = 5 * 12 * 24; // 24H avec 1 bloc toutes les 5 minutes
  const uint32_t NOMBRE_DE_BLOCKS_DE_SIMULATION = 100 * DIVISIONS_PAR_UNITE_TEMPS;

  Log::setEnabled(true);
  Log() << "--------- SIMULATION ---------";
  Log() << "stepMax:             " << STEP_MAX << " saut(s)";
  Log() << "sigStock:            " << SIG_STOCK << " certification(s) maxi.";
  Log() << "sigQty:              " << SIG_QTY << " certification(s) mini.";
  Log() << "sigPeriod:           " << SIG_PERIOD << " jour(s) de délai";
  Log() << "sigValidity:         " << simulateur->SIG_VALIDITY / DIVISIONS_PAR_UNITE_TEMPS << " jour(s)";
  Log() << "xPercent:            " << X_PERCENT * 100.0 << " %";
  Log() << "------------------------------";
//  Log() << "sigMoy:              " << SIG_MOY << " certification(s) émise(s) en moyenne par membre";
  Log() << "Levier par membre:   " << int(SIG_STOCK / SIG_QTY) << " membres";
  Log() << "% d'arrivants max:   " << int(POURCENTAGE_DE_NOUVEAUX_MEMBRES_MAXI * 100.0) << " %";
  Log() << "Durée de simulation: " << NOMBRE_DE_BLOCKS_DE_SIMULATION << " jour(s)";
  Log() << "------------------------------";

  // 1. Creation d'une communauté initiale reconnue
  simulateur->creeLaCommunauteInitialeEnPiscine();

  for (int i = 0; i < NOMBRE_DE_BLOCKS_DE_SIMULATION; i++) {
    simulateur->ajouteUnBloc();
    simulateur->alimenteLesPiscines();
    // Affiche la WoT initiale
    simulateur->afficheWoT();
    Log();
  }

  // Affiche la WoT finale
  simulateur->afficheWoT();

  /****
   * Histoire :
   *
   *
   *   2. A chaque tour :
   *
   *     a. Commit
   *       - Ejecte les membres qui ne respectent plus la règle stepMax (tous les X temps par rapport à leur date d'arrivée)
   *       - Ajout des certifications à la WoT qui peuvent l'être (prio interne)
   *       - Ajout les membres qui peuvent l'être (+ supprime les identités et certifications de la piscine)
   *     b. Ajout d'identités en piscine (mettre une taille max)
   *     c. Emission de certifications en piscine
   */
}
