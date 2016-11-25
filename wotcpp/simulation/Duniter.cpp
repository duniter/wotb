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

    std::random_device rd;
    std::mt19937 eng(rd());

  Duniter::Duniter(double xPercent, uint32_t stepMax, uint32_t minNew, double maxNewPercent, uint32_t sigMoy, uint32_t sigStock, uint32_t sigQty, uint32_t sigPeriod) {
    wot = new WebOfTrust(sigStock);
    iPool = new IdentityPool(sigMoy, sigStock);
    cPool = new CertificationPool();
    // Paramètres déduits
    X_PERCENT = xPercent;
    STEPMAX = stepMax;
    MIN_NEW = minNew;
    MAX_NEW_PERCENT = maxNewPercent;
    SIG_STOCK = sigStock;
    SIG_QTY = sigQty;
    SIG_PERIOD = sigPeriod;
    SIG_VALIDITY = sigStock * SIG_PERIOD;
    COMMUNAUTE_INITIALE = sigQty + 1;
  }

  void Duniter::creeLaCommunauteInitialeEnPiscine() {
    iPool->alimenteEnNouveaux(COMMUNAUTE_INITIALE);
    for (int i = 0; i < iPool->newcomers.size(); i++) {
      // Each initial member certifies the SIG_QTY following initial members
      Identity* from = iPool->newcomers[i];
      for (int j = 0; j < SIG_QTY; j++) {
        // Certify next newcomer, and loop on first when necessary
        Identity* to = iPool->newcomers[(i + j + 1) % COMMUNAUTE_INITIALE];
        to->derniereEmissionDeCertif = 0; // t = 0
        cPool->createNew(from->uid, to->uid, 0);
      }
    }
  }
    /**
     * Mécanique principale qui ajoute des exclu les membres ne répondant plus aux critères et tente d'ajouter
     * un maximum de nouveaux membres selon les règles établies.
     */
    void Duniter::ajouteUnBloc() {
      auto start = std::chrono::high_resolution_clock::now();
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
      if (blocCourant == 0 && iPool->members.size() == 0) {

        Log() << "--------------- TOUR 0 ---------------";

        /*****************
         * INITIALISATION
         ****************/

        // Ajoute les membres initiaux
        for (int i = 0; i < iPool->newcomers.size(); i++) {
          wot->addNode();
          // Affecte un identifiant wotb
          iPool->newcomers[i]->wotb_id = wot->getSize() - 1;
          newcomer2member(iPool->newcomers[i]);
          i--;
        }
        // Ajoute les liens initiaux
        for (int to = 0; to < cPool->certs.size(); to++) {
          for (int j = 0; j < cPool->certs[to].size(); j++) {
            cert2lien(cPool->certs[to][j], to, j, true);
          }
        }
        blocCourant = 0;
      } else {

        Log() << "--------------- TOUR " << blocCourant + 1 << " ---------------";

        /*****************
         * BLOC NORMAL
         ****************/
        start = std::chrono::high_resolution_clock::now();
        // Ajoute les liens internes (membre à membre)
        for (int to = 0; to < iPool->lastUID(); to++) {
          for (int j = 0; j < cPool->certs[to].size(); j++) {
            essaieIntegrerLien(cPool->certs[to][j], to, j);
          }
        }
        elapsed = std::chrono::high_resolution_clock::now() - start;
        microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        Log() << setw(6) << microseconds << "µs for essaieIntegrerLien";

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iPool->newcomers.size(); i++) {
          essaieIntegrerNouveauVenu(iPool->newcomers[i]);
        }
        elapsed = std::chrono::high_resolution_clock::now() - start;
        microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        Log() << setw(6) << microseconds << "µs for essaieIntegrerNouveauVenu";

        // TODO: actualisation tous les X periodes => Test de distance

        blocCourant++;
      }

      /*********************
       * PASSAGE DU TEMPS
       *
       * Expiration des liens
       * -------------------
       *
       * Les liens existants dans la WoT expirent et s'auto-détruisent.
       */

      start = std::chrono::high_resolution_clock::now();
      for (int to = 0; to < wot->getSize(); to++) {
        for (int j = 0; j < cPool->liens[to].size(); j++) {
          if (cPool->liens[to][j]->dateOfIssuance + SIG_VALIDITY <= blocCourant) {
            supprimeLien(cPool->liens[to][j], to, j);
          }
        }
      }
      elapsed = std::chrono::high_resolution_clock::now() - start;
      microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
      Log() << setw(6) << microseconds << "µs for supprimeLien";

      // TODO expiration des certifications en piscine


      /*********************
       * PASSAGE DU TEMPS
       *
       * Règle du nombre de liens
       * ------------------------
       *
       * A tout instant, les membres qui n'ont pas assez de certifications doivent être exclus.
       */
      start = std::chrono::high_resolution_clock::now();
      for (uint32_t i = 0; i < wot->getSize(); i++) {
        Node* node = wot->getNodeAt(i);
        if (node->getNbLinks() < SIG_QTY) {
          node->setEnabled(false);
        }
      }
      elapsed = std::chrono::high_resolution_clock::now() - start;
      microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
      Log() << setw(6) << microseconds << "µs for setEnabled";

      /*********************
       * NOUVEAUX VENUS
       *
       * Stratégie d'arrivée
       * -------------------
       *
       * Il existe toujours *a minima* MIN_NEW nouveaux membre, et *a maxima* MAX_NEW_PERCENT % de membres supplémentaires
       * de la taille actuelle de la toile de confiance.
       */
      start = std::chrono::high_resolution_clock::now();
      int taillePiscineDeMembres = max(int(MIN_NEW), int(wot->getSize() * MAX_NEW_PERCENT));
      iPool->alimenteEnNouveaux(taillePiscineDeMembres);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
      Log() << setw(6) << microseconds << "µs for alimenteEnNouveaux";


      /*********************
       *   CERTIFICATIONS
       *
       * Stratégie d'émission (fréquence)
       * --------------------------------
       *
       * Chaque *membre* émet 1 certification par (sigPeriod unités de temps * (sigStock / sigPerso))
       * Ex. : pour 1 membre à 50 certifications en moyenne pour un stock max à 150, et un sigPeriod = 1 semaine,
       * le membre émettra 1 certification toutes les 3 semaines (il étale son niveau personnel dans le temps).
       *
       * --> Il s'agit d'une stratégie linéaire. En réalité, on peut imaginer que chaque membre émet beaucoup de
       * certifications au début (relativement à sa moyenne personnelle cible) puis moins à la fin, suivant une courbe
       * logarithmique. Il faut donc tester plusieurs stratégies.
       *
       * Stratégie d'émission (cible)
       * ----------------------------
       *
       * On considère ici que le membre émet indifféremment à la toile existante ou aux nouveaux membres potentiels.
       * Toutefois, le nombre de nouveaux membres est borné à 5% maximum de la toile existante.
       *
       * --> Il s'agit d'une stratégie particulière, d'autres peuvent être imaginées.
       */
      start = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iPool->members.size(); i++) {
        membreEmetUneCertifSiPossible(iPool->members[i]);
      }
      elapsed = std::chrono::high_resolution_clock::now() - start;
      microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
      Log() << setw(6) << microseconds << "µs for membreEmetUneCertifSiPossible";
    };

    void Duniter::alimenteLesPiscines() {

    };

    void Duniter::afficheWoT() {
//      wot->showTable();
      Log() << "Nouveaux membres en attente : " << iPool->newcomers.size();
      Log() << "Nombre de membres : " << wot->getEnabledCount();
      Log() << "Nombre d'individus passés : " << wot->getSize();
    };

    void Duniter::newcomer2member(Identity *identity) {
      // Retire de la liste des newcomers
      vector<Identity*>::iterator it = std::find(iPool->newcomers.begin(), iPool->newcomers.end(), identity);
      if (it != iPool->newcomers.end()) {
        iPool->newcomers.erase(it);
      }
      // Ajoute dans la liste des membres
      iPool->members.push_back(identity);
      wotMembers[identity->uid] = identity;
      wotIdentities[identity->wotb_id] = identity;
    };

    void Duniter::cert2lien(Certification* cert, int to, int j, bool majWoTb) {
      Lien lien = cert->link;
      uint32_t from = lien.first;
      Identity* certifieur = wotMembers[from];
      Identity* certifie = wotMembers[to];
      if (certifie != NULL) {
        Log2() << "LIEN de UID " << from << " -> " << to << " | WID " << certifieur->wotb_id << " -> " << certifie->wotb_id;
        if (majWoTb) {
          wot->getNodeAt(certifieur->wotb_id)->addLinkTo(wot->getNodeAt(certifie->wotb_id));
        }
        // Ajoute dans la liste des membres
        cPool->liens[to].push_back(cert);
        // Retire de la liste des certificats potentiels
        cPool->certs[to].erase(cPool->certs[to].begin() + j);
      }
    };

    void Duniter::supprimeLien(Certification* cert, int to, int j) {
      Log2() << "SUPPRIME Lien de UID " << cert->link.first << " -> " << cert->link.second;
      uint32_t from = cert->link.first;
      Identity* certifieur = wotMembers[from];
      Identity* certifie = wotMembers[to];
      wot->getNodeAt(certifieur->wotb_id)->removeLinkTo(certifie->wotb_id);
      // Retire de la liste des certificats de toile
      cPool->liens[to].erase(cPool->liens[to].begin() + j);
    };

    void Duniter::membreEmetUneCertifSiPossible(Identity *emetteur) {
      if (emetteur->sigPersoCible > 0) {
        uint32_t multipleDeSigPeriod = SIG_STOCK / emetteur->sigPersoCible;
        if (emetteur->derniereEmissionDeCertif + multipleDeSigPeriod * SIG_PERIOD <= blocCourant) {
          // Emet une certification aléatoirement
          uint32_t nbPossibilites = iPool->newcomers.size() + iPool->members.size();
          int nbEssais = 0;
          Identity* identiteCiblee = NULL;
          // Priorite aux liens bidirectionnels
//          Node* noeudEmetteur = wot->getNodeAt(emetteur->wotb_id);
//          for (int i = 0; identiteCiblee == NULL && i < noeudEmetteur->getNbLinks(); i++) {
//            WID indexNoeudCertifieurDeSoi = wot->getNodeIndex(noeudEmetteur->mCert[i]);
//            identiteCiblee = wotIdentities[indexNoeudCertifieurDeSoi];
//            if (existeDejaCertification(emetteur, identiteCiblee)) {
//              identiteCiblee = NULL;
//            }
//          }
          // Puis bascule éventuellement sur la certification d'un membre de façon aléatoire
          while (identiteCiblee == NULL && nbEssais < 100) {
            int cible = nombreAleatoireUniformeEntreXetY(0, nbPossibilites - 1);
            if (cible < iPool->newcomers.size()) {
              identiteCiblee = iPool->newcomers[cible];
            } else {
              identiteCiblee = iPool->members[cible - iPool->newcomers.size()];
            }
            bool luiMeme = emetteur->uid == identiteCiblee->uid;
            if (luiMeme) {
              identiteCiblee = NULL;
            }
            else if (existeDejaCertification(emetteur, identiteCiblee)) {
              identiteCiblee = NULL;
            }
            nbEssais++;
          };
          if (identiteCiblee != NULL) {
            cPool->createNew(emetteur->uid, identiteCiblee->uid, blocCourant);
          }
        }
      }
    };

    bool Duniter::existeDejaCertification(Identity* emetteur, Identity* identiteCiblee) {
      // Test en piscine
      vector<Certification*> recuesEnPiscine = cPool->certs[identiteCiblee->uid];
      bool existeEnPiscine = false;
      for (int i = 0; !existeEnPiscine && i < recuesEnPiscine.size(); i++) {
        int fromUID = recuesEnPiscine[i]->link.first;
        if (fromUID == emetteur->uid) {
          existeEnPiscine = true;
        }
      }
      if (existeEnPiscine) {
        return true;
      }
      // Test en toile
      vector<Certification*> recuesEnToile = cPool->liens[identiteCiblee->uid];
      bool existeEnToile = false;
      for (int i = 0; !existeEnToile && i < recuesEnToile.size(); i++) {
        int fromUID = recuesEnToile[i]->link.first;
        if (fromUID == emetteur->uid) {
          existeEnToile = true;
        }
      }
      return existeEnToile;
    }

    void Duniter::essaieIntegrerLien(Certification* cert, int to, int j) {
      uint32_t from = cert->link.first;
      Identity* certifieur = wotMembers[from];
      Identity* certifie = wotMembers[to];
      if (certifieur != NULL && certifie != NULL) {
        Node* wotbCertifieur = wot->getNodeAt(certifieur->wotb_id);
        Node* wotbCertifie = wot->getNodeAt(certifie->wotb_id);
        if (wotbCertifieur->isEnabled() && wotbCertifie->isEnabled() && wotbCertifieur->getNbIssued() < SIG_STOCK) {
          if (wotbCertifieur->addLinkTo(wotbCertifie)) {
            cert2lien(cert, to, j, false);
          } else {
            Log2() << "ECHEC de l'ajout du lien UID " << from << " -> " << to << " | WID " << certifieur->wotb_id << " -> " << certifie->wotb_id;
          }
        }
      }
    };

    void Duniter::essaieIntegrerNouveauVenu(Identity *nouveau) {
      vector<Certification*> liensPotentiels = cPool->certs[nouveau->uid];
      vector<int> liensEffectifs;
      if (liensPotentiels.size() >= SIG_QTY) {
        // Passe la règle de quantités de signatures.
        // Ensuite, tester la distance :
        wot->addNode();
        uint32_t wotb_id = wot->getSize() - 1;
        for (int i = 0; i < liensPotentiels.size(); i++) {
          uint32_t from = liensPotentiels[i]->link.first;
          Identity* certifieur = wotMembers[from];
          Node* noeudCertifieur = wot->getNodeAt(certifieur->wotb_id);
          if (noeudCertifieur->getNbIssued() < SIG_STOCK && noeudCertifieur->addLinkTo(wotb_id)) {
            liensEffectifs.push_back(i);
          } else {
            Log() << "ECHEC de l'ajout du lien UID " << from << " -> " << nouveau->uid << " | WID " << certifieur->wotb_id << " -> " << nouveau->wotb_id;
          }
        }
        uint32_t d_min = ceil(pow(wot->getSize(), 1 / STEPMAX));
        if (false && liensEffectifs.size() >= SIG_QTY && wot->computeDistance(wotb_id, d_min, STEPMAX, X_PERCENT).isOutdistanced) {
          // Rollback
//          for (auto i = 0; i < liensPotentiels.size(); i++) {
//            uint32_t from = liensPotentiels[i].first;
//            Identity* certifieur = wotMembers[from];
//            wot->getNodeAt(certifieur->wotb_id)->removeLinkTo(wotb_id);
//          }
          // Pop the last node
          wot->removeNode();
        } else {
          nouveau->wotb_id = wotb_id;
          newcomer2member(nouveau);
          for (int j = liensEffectifs.size() - 1; j >= 0; j--) {
            int position = liensEffectifs[j];
            cert2lien(liensPotentiels[position], nouveau->uid, position, false);
          }
        }
      }
    };

    int Duniter::nombreAleatoireUniformeEntreXetY(uint32_t x, uint32_t y) {
      std::uniform_int_distribution<> distr(x, y);
      return distr(eng);
    }
}
