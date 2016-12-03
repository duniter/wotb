//
// Created by cgeek on 01/12/16.
//

#ifndef WOTCPP_STATSDUJOUR_H
#define WOTCPP_STATSDUJOUR_H

#include <cstdint>
#include <chrono>

namespace libsimu {

    class StatsDuJour {
    public:
        double nombreDeMembresEnToile = 0;
        double nombreDIdentitesGenereesEnPisicine = 0;
        double nombreDeCertifsGenereesEnPisicine = 0;
        double nombreDeMembresEnAttenteFinDeTour = 0;
        double nombreDeMembresCertifsEnAttenteFinDeTour = 0;
        double nombreDeMembresStockEpuise = 0;
        long long tempsExecutionMembreEmetUneCertifSiPossible = 0;
    };
}

#endif //WOTCPP_STATSDUJOUR_H
