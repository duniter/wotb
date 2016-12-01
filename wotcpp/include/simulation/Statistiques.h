//
// Created by cgeek on 01/12/16.
//

#ifndef WOTCPP_STATISTIQUES_H
#define WOTCPP_STATISTIQUES_H


#include <vector>
#include "StatsDuTour.h"

using namespace std;

namespace libsimu {

    class Statistiques {
    public:
        Statistiques(int nbBlocsDeSimulation);

        vector<StatsDuTour*> parTour;
    };
}

#endif //WOTCPP_STATISTIQUES_H
