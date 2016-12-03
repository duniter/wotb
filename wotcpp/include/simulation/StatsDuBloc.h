//
// Created by cgeek on 01/12/16.
//

#ifndef WOTCPP_StatsDuBloc_H
#define WOTCPP_StatsDuBloc_H


#include <cstdint>
#include <chrono>

namespace libsimu {

    class StatsDuBloc {
    public:
        double nombreDeMembresEnToile = 0;
        double nombreDeMembresPassesEnToile = 0;
        double nombreDeLiensEnToile = 0;
        double nombreDeMembresAjoutes = 0;
        double nombreDeMembresExclusParManqueDeCertif = 0;
        double nombreDeTentativesDAjoutMembre = 0;
        double nombreDeTentativesDAjoutMembreSucces = 0;
        double nombreDeTentativesDAjoutMembreEchouees = 0;
        double nombreDeTentativesDAjoutMembreEchoueesParQteLiens = 0;
        double nombreDeTentativesDAjoutMembreEchoueesParDistance = 0;
        double nombreDeTentativesDAjoutCertInterne = 0;
        double nombreDeTentativesDAjoutCertInterneSucces = 0;
        double nombreDeTentativesDAjoutCertInterneEchouees = 0;
        double nombreDeTentativesDAjoutCertInterneEchoueesParAdhesion = 0;
        double nombreDeTentativesDAjoutCertInterneEchoueesParStock = 0;
        double nombreDeTentativesDAjoutCertNouveauVenu = 0;
        double nombreDeTentativesDAjoutCertNouveauVenuSucces = 0;
        double nombreDeTentativesDAjoutCertNouveauVenuEchouees = 0;
        double nombreDeTentativesDAjoutCertNouveauVenuEchoueesParStock = 0;
        double nombreDeTentativesDAjoutCertNouveauVenuEchoueesParQteLiens = 0;
        double nombreDeTentativesDAjoutCertNouveauVenuEchoueesParDistance = 0;
        double nombreDeCertifsExpirees = 0;
        double nombreDeCertifsTransfereesEnToile = 0;
        double nombreDeSentries = 0;
        double nombreDeLiensEmisPourEtreSentry = 0;
        double nombreDeMembresEnAttenteFinDeTour = 0;
        double nombreDeMembresCertifsEnAttenteFinDeTour = 0;
        long long tempsExecutionIntegrationLiensInternes = 0;
        long long tempsExecutionIntegrationNouveauxVenus = 0;
        long long tempsExecutionSuppressionLiens = 0;
        long long tempsExecutionAlimenteEnNouveaux = 0;
        long long tempsExecutionDuBloc = 0;
    };
}


#endif //WOTCPP_StatsDuBloc_H
