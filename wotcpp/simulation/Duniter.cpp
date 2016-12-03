#include <Duniter.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "simulation/IdentityPool.h"
#include "webOfTrust.h"
#include "log.h"
#include <math.h>
#include <fstream>
#include <iomanip>
#include <sstream> // stringstream


namespace libsimu {

  using namespace std;
  using namespace libwot;

  Duniter::Duniter(int dureeSimulationEnBlocks, double xPercent, uint32_t stepMax, uint32_t minNew, double maxNewPercent,
                   uint32_t sigMoy, uint32_t sigStock, uint32_t sigQty, uint32_t sigPeriod, uint32_t sigWindow) {
    NOMBRE_DE_BLOCKS_DE_SIMULATION = dureeSimulationEnBlocks;
    X_PERCENT = xPercent;
    STEPMAX = stepMax;
    MIN_NEW = minNew;
    MAX_NEW_PERCENT = maxNewPercent;
    SIG_STOCK = sigStock;
    SIG_QTY = sigQty;
    SIG_PERIOD = sigPeriod;
    SIG_VALIDITY = sigStock * SIG_PERIOD;
    SIG_WINDOW = sigWindow;
    COMMUNAUTE_INITIALE = sigQty + 1;
    wot = new WebOfTrust(sigStock);
    iPool = new IdentityPool(sigMoy, sigStock, sigQty);
    cPool = new CertificationPool(NOMBRE_DE_BLOCKS_DE_SIMULATION, X_PERCENT, STEPMAX, MIN_NEW, MAX_NEW_PERCENT, sigMoy, SIG_STOCK, SIG_QTY, SIG_PERIOD);
    stats = new Statistiques(NOMBRE_DE_BLOCKS_DE_SIMULATION);
  }

  void Duniter::resetStatsCourantes() {

    statCourante = new StatsDuBloc();
    iPool->statCourante = statCourante;
    cPool->statCourante = statCourante;
  }
  /**
   * Mécanique principale qui ajoute des exclu les membres ne répondant plus aux critères et tente d'ajouter
   * un maximum de nouveaux membres selon les règles établies.
   */
  void Duniter::ajouteUnBloc() {
    Log() << "--------------- BLOC " << blocCourant << " ---------------";

    // Stats
    auto start = std::chrono::high_resolution_clock::now();

    cPool->essaieIntegrerLiensInternes(iPool, blocCourant);
    cPool->essaieIntegrerNouveauxVenus(wot, iPool, blocCourant);

    blocCourant++;

    // Le temps s'est incrémenté, conséquences :
    cPool->faitExpirerLesLiens(blocCourant);

    // TODO: actualisation tous les X periodes => Test de distance
    // TODO expiration des certifications en piscine
    statCourante->nombreDeMembresEnToile = wot->getEnabledCount();
    statCourante->nombreDeMembresPassesEnToile = wot->getSize();
    statCourante->nombreDeLiensEnToile = wot->getNbLinks();
    statCourante->nombreDeMembresEnAttenteFinDeTour = iPool->newcomers.size();
    statCourante->nombreDeMembresCertifsEnAttenteFinDeTour = cPool->getNbCertifsEnPiscine();
    statCourante->nombreDeMembresAjoutes = statCourante->nombreDeTentativesDAjoutMembreSucces - statCourante->nombreDeMembresExclusParManqueDeCertif;
    statCourante->tempsExecutionDuBloc = Statistiques::compteMicrosecondesDepuis(start);
  };

  void Duniter::creeBlockInitialEtSaCommunaute() {
    statCourante = new StatsDuBloc();
    statDuJourEnCours = new StatsDuJour();
    iPool->statCourante = statCourante;
    cPool->statCourante = statCourante;
    iPool->statDuJourEnCours = statDuJourEnCours;
    cPool->statDuJourEnCours = statDuJourEnCours;
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
    Log() << setw(7) << statCourante->nombreDeMembresPassesEnToile << " Membres passés et présents";
    Log() << setw(7) << statDuJourEnCours->nombreDeMembresStockEpuise << " Membres avec stock épuisé";
    Log() << setw(7) << statCourante->nombreDeLiensEmisPourEtreSentry << " Liens émis pour être point de contrôle";
    Log() << setw(7) << statCourante->nombreDeSentries << " Points de contrôles (xPercent = " << X_PERCENT*100 << "% => " <<  int(floor(statCourante->nombreDeSentries * X_PERCENT)) << " points effectifs)";
    Log() << setw(7) << statCourante->nombreDeLiensEnToile << " Liens en toile (" << (variationL >= 0 ? "+" : "") << variationL << ")";
    Log() << setw(7) << statCourante->nombreDeTentativesDAjoutMembre << " Tentatives d'ajout de membre";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutMembreSucces << " Succès";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutMembreEchouees << " Echecs";
    Log() << setw(7) << "" << "    * dont " << statCourante->nombreDeTentativesDAjoutMembreEchoueesParQteLiens << " par le nombre de liens";
    Log() << setw(7) << "" << "    * dont " << statCourante->nombreDeTentativesDAjoutMembreEchoueesParDistance << " par la distance";
    Log() << setw(7) << statCourante->nombreDeTentativesDAjoutCertInterne << " Tentatives d'ajout de lien (membre à membre)";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertInterneSucces << " Succès";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertInterneEchouees << " Echecs";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertInterneEchoueesParAdhesion << " par l'adhésion";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertInterneEchoueesParStock<< " par le stock";
    Log() << setw(7) << statCourante->nombreDeTentativesDAjoutCertNouveauVenu << " Tentatives d'ajout de lien (membre à nouveau venu)";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuSucces<< " Succès";
    Log() << setw(7) << "" << "  * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchouees<< " Echecs";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParQteLiens << " par le nombre de liens";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParDistance << " par la distance";
    Log() << setw(7) << "" << "    * " << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParStock<< " par le stock";
    Log() << setw(7) << statCourante->nombreDeMembresExclusParManqueDeCertif << " Membres exclus (par nombre de liens)";
    Log() << setw(7) << statCourante->nombreDeCertifsExpirees << " Liens détruits en toile (expiration)";
    Log() << setw(7) << statCourante->nombreDeMembresEnAttenteFinDeTour << " Identités en piscine (-" << statCourante->nombreDeTentativesDAjoutMembreSucces << " devenues membres)";
    Log() << setw(7) << statCourante->nombreDeMembresCertifsEnAttenteFinDeTour << " Certifications en piscine (-" << statCourante->nombreDeCertifsTransfereesEnToile << " transférées en toile)";
    Log() << setw(7) << "  --------";
    Log() << setw(7) << statCourante->tempsExecutionDuBloc << " µs pour l'ajout du bloc";
    Log() << setw(7) << "  --------";
    Log();
  };

    void Duniter::ouvreCSV() {

      auto now = std::chrono::system_clock::now();
      auto in_time_t = std::chrono::system_clock::to_time_t(now);
      std::stringstream ss;
      ss << "wotcpp-stats-";
      ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M");
      ss << ".csv";
      Log() << "Stats dans " << ss.str();
      csvFile.open (ss.str());
      csvFile << "Membres en toile;Membres passes et presents;Membres avec stock epuise;";
      csvFile << "Liens emis pour etre point de controle;Points de controle;Liens en toile;";
      csvFile << "(Ajout membre) Succes;(Ajout membre) Echecs par nombre de liens;(Ajout membre) Echecs par la distance;";
      csvFile << "(M2M) Succes ajout de lien;(M2M) Echecs par adhésion;(M2M) Echecs par le stock;";
      csvFile << "(M2N) Succes ajout de lien;(M2N) Echecs par nombre de liens;(M2N) Echecs par la distance;(M2N) Echecs par le stock;";
      csvFile << "Membres exclus;Liens détruits;Identités en piscine;Nouveaux membres;Certifications en piscine;Certifications intégrées";
      csvFile << "\n";
    }
    void Duniter::sauvegardeWoT() {

      auto now = std::chrono::system_clock::now();
      auto in_time_t = std::chrono::system_clock::to_time_t(now);
      std::stringstream ss;
      ss << "wotcpp-wot-";
      ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M");
      ss << ".dot";
      std::string filename = ss.str();
      Log() << "WoT dans " << ss.str();

      ofstream dotFile;
      dotFile.open (filename);
      dotFile << this->wot->getGraphviz();
      dotFile.close();
    }

    void Duniter::sauvegardeGephi() {

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "wotcpp-wot-";
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M");
        ss << ".gexf";
        std::string filename = ss.str();
        Log() << "WoT Gephi dans " << ss.str();

        ofstream gefxFile;
        gefxFile.open (filename);
        gefxFile << this->cPool->getGephiHistorique(blocCourant);
        gefxFile.close();
    }

    void Duniter::fermeCSV() {
      csvFile.close();
    }
    void Duniter::genereCSV() {;
      csvFile << statCourante->nombreDeMembresEnToile << ";";
      csvFile << statCourante->nombreDeMembresPassesEnToile << ";";
      csvFile << statDuJourEnCours->nombreDeMembresStockEpuise << ";";

      csvFile << statCourante->nombreDeLiensEmisPourEtreSentry << ";";
      csvFile << statCourante->nombreDeSentries << ";";
      csvFile << statCourante->nombreDeLiensEnToile << ";";

      csvFile << statCourante->nombreDeTentativesDAjoutMembreSucces  << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutMembreEchoueesParQteLiens << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutMembreEchoueesParDistance  << ";";

      csvFile << statCourante->nombreDeTentativesDAjoutCertInterneSucces  << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutCertInterneEchoueesParAdhesion  << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutCertInterneEchoueesParStock << ";";

      csvFile << statCourante->nombreDeTentativesDAjoutCertNouveauVenuSucces  << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParQteLiens  << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParDistance  << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutCertNouveauVenuEchoueesParStock  << ";";

      csvFile << statCourante->nombreDeMembresExclusParManqueDeCertif << ";";
      csvFile << statCourante->nombreDeCertifsExpirees  << ";";
      csvFile << statCourante->nombreDeTentativesDAjoutMembreSucces << ";";
      csvFile << statCourante->nombreDeMembresEnAttenteFinDeTour  << ";";
      csvFile << statCourante->nombreDeMembresCertifsEnAttenteFinDeTour << ";";
      csvFile << statCourante->nombreDeCertifsTransfereesEnToile << ";";
      csvFile << "\n";
    };

  void Duniter::alimenteLesPiscines(int jour) {

    statDuJourEnCours = new StatsDuJour();
    iPool->statDuJourEnCours = statDuJourEnCours;
    cPool->statDuJourEnCours = statDuJourEnCours;

    Log() << ">>>>>>>>>>>>>>>> JOUR " << jour << " <<<<<<<<<<<<<<<<<<<<";

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

    // Stats
    statDuJourEnCours->nombreDeMembresEnAttenteFinDeTour = iPool->newcomers.size();
    statDuJourEnCours->nombreDeMembresCertifsEnAttenteFinDeTour = cPool->getNbCertifsEnPiscine();
    Log() << setw(7) << statDuJourEnCours->nombreDeMembresEnAttenteFinDeTour << " Identités en piscine (+" << statDuJourEnCours->nombreDIdentitesGenereesEnPisicine << ")";
    Log() << setw(7) << statDuJourEnCours->nombreDeMembresCertifsEnAttenteFinDeTour << " Certifications en piscine (+" << statDuJourEnCours->nombreDeCertifsGenereesEnPisicine << ")";
  };

  void Duniter::purgeLesPiscines() {

      /*********************
       *   CERTIFICATIONS
       */
      cPool->purgeLesCertifications(SIG_WINDOW, blocCourant);
  };
}
