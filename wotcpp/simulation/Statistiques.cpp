//
// Created by cgeek on 01/12/16.
//

#include "Statistiques.h"

namespace libsimu {

    Statistiques::Statistiques(int nbBlocsDeSimulation) {
      parTour = vector<StatsDuTour*>(nbBlocsDeSimulation);
    }
}
