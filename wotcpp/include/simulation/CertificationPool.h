#ifndef CERTIFICATION_POOL_H
#define CERTIFICATION_POOL_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include "simulation/Identity.h"
#include "simulation/Certification.h"

namespace libsimu {

  class CertificationPool {

    public :

      CertificationPool(int dureeSimulationEnBlocks, double xPercent, uint32_t stepMax, uint32_t minNew, double maxNewPercent, uint32_t sigMoy, uint32_t sigStock, uint32_t sigQty, uint32_t sigPeriod);
      ~CertificationPool();

      void createNew(Identity* from, Identity* to, uint32_t date);
      void cert2lien(Certification* cert, int i, int j, bool majWoTb);
      void faitExpirerLesLiens(uint32_t blocCourant);
      void supprimeLien(Certification* cert, int to, int j);
      void allPendingsToLinks();
      void essaieIntegrerLiensInternes(IdentityPool *iPool);
      bool essaieIntegrerLien(Certification* cert, int to, int j);
      void essaieIntegrerNouveauxVenus(WebOfTrust *wot, IdentityPool *iPool);
      void essaieIntegrerNouveauVenu(Identity *identity, WebOfTrust* wot, IdentityPool* iPool);
      void membreEmetUneCertifSiPossible(IdentityPool* iPool, Identity *emetteur, uint32_t blocCourant);
      void emetDeNouvellesCertifications(IdentityPool* iPool, uint32_t blocCourant);
      int nombreAleatoireUniformeEntreXetY(uint32_t x, uint32_t y);
      bool existeDejaCertification(Identity* emetteur, Identity* identiteCiblee);

      int getNbCertifsEnPiscine() {
        int count = 0;
        int certsSize = certs.size();
        for (int i = 0; i < certsSize; i++) {
          count += certs[i].size();
        }
        return count;
      }

      uint32_t counter;
      vector<vector<Certification*>> certs;
      vector<vector<Certification*>> liens;
      vector<vector<Certification*>> expirationsDeLiens;
      int NOMBRE_DE_BLOCKS_DE_SIMULATION;
      uint32_t STEPMAX;
      uint32_t SIG_STOCK;
      uint32_t SIG_QTY;
      uint32_t SIG_VALIDITY;
      uint32_t SIG_PERIOD;
      uint32_t COMMUNAUTE_INITIALE;
      uint32_t MIN_NEW;
      double MAX_NEW_PERCENT;
      double X_PERCENT;
      StatsDuTour* statCourante;
  };
}

#endif
