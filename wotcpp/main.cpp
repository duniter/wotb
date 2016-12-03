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

  // Paramètres voulus
  const uint32_t UNITES_TEMPS = 1; // Unité de temps = 1 jour
  const uint32_t BLOCS_PAR_UNITE_TEMPS = 3; // 1 bloc/heure
  const uint32_t SIG_QTY = 5;
  const uint32_t STEP_MAX = 6;
  const uint32_t SIG_STOCK = 150;
  const uint32_t SIG_PERIOD = 1 * UNITES_TEMPS; // 1 jour
  const uint32_t SIG_MOY = SIG_STOCK / 3;
  const double X_PERCENT = 0.8;
  const double POURCENTAGE_DE_NOUVEAUX_MEMBRES_MAXI = 0.1;
  const uint32_t MINIMUM_DE_NOUVEAUX_VENUS_PAR_UNITE_TEMPS = 2;

  /*****************************
   * SIMULATEUR DE NŒUD DUNITER
   ****************************/

  const uint32_t DUREE_SIMULATION = 7 *  UNITES_TEMPS;
  const uint32_t NOMBRE_DE_BLOCKS_DE_SIMULATION = DUREE_SIMULATION * BLOCS_PAR_UNITE_TEMPS;
  Duniter* simulateur = new Duniter(NOMBRE_DE_BLOCKS_DE_SIMULATION, X_PERCENT, STEP_MAX, MINIMUM_DE_NOUVEAUX_VENUS_PAR_UNITE_TEMPS, POURCENTAGE_DE_NOUVEAUX_MEMBRES_MAXI, SIG_MOY, SIG_STOCK, SIG_QTY, SIG_PERIOD);
//  const uint32_t NOMBRE_DE_BLOCKS_DE_SIMULATION = 5 * 12 * 24; // 24H avec 1 bloc toutes les 5 minutes
  Log::setEnabled(true);
  Log2::setEnabled(true);
  simulateur->ouvreCSV();
  Log() << "--------- SIMULATION ---------";
  Log() << "stepMax:             " << STEP_MAX << " saut(s)";
  Log() << "sigStock:            " << SIG_STOCK << " certification(s) maxi.";
  Log() << "sigQty:              " << SIG_QTY << " certification(s) mini.";
  Log() << "sigPeriod:           " << SIG_PERIOD << " jour(s) de délai";
  Log() << "sigValidity:         " << simulateur->SIG_VALIDITY << " jour(s)";
  Log() << "xPercent:            " << X_PERCENT * 100.0 << " %";
  Log() << "------------------------------";
//  Log() << "sigMoy:              " << SIG_MOY << " certification(s) émise(s) en moyenne par membre";
  Log() << "Levier par membre:   " << int(SIG_STOCK / SIG_QTY) << " membres";
  Log() << "% d'arrivants max:   " << int(POURCENTAGE_DE_NOUVEAUX_MEMBRES_MAXI * 100.0) << " %";
  Log() << "Durée de simulation: " << NOMBRE_DE_BLOCKS_DE_SIMULATION << " jour(s)";
  Log() << "------------------------------";
  Log() << "        TOILE INITIALE        ";

  // 1. Creation d'une communauté initiale reconnue
  simulateur->creeBlockInitialEtSaCommunaute();
  simulateur->wot->showTable();
  Log();
  Log();

  for (int jour = 0; jour < DUREE_SIMULATION; jour++) {
    // A chaque période de temps humaine, on alimente les piscines
    simulateur->alimenteLesPiscines(jour);
    // Puis, le réseau intègre techniquement ces données en BLOCS_PAR_UNITE_TEMPS opérations ou « blocs »
    for (int b = 0; b < BLOCS_PAR_UNITE_TEMPS; b++) {
      simulateur->ajouteUnBloc();
      simulateur->afficheStats();
      simulateur->genereCSV();
    }
    simulateur->fermeCSV();
//    simulateur->wot->showTable();
//    Log();
  }

//  simulateur->wot->showTable();
}
