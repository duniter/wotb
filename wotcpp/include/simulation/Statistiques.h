//
// Created by cgeek on 01/12/16.
//

#ifndef WOTCPP_STATISTIQUES_H
#define WOTCPP_STATISTIQUES_H


#include <vector>
#include "StatsDuBloc.h"

using namespace std;

namespace libsimu {

    class Statistiques {
    public:
        Statistiques(int nbBlocsDeSimulation);

        vector<StatsDuBloc*> parTour;

        static long long compteMicrosecondesDepuis(std::chrono::high_resolution_clock::time_point start) {
          auto elapsed = std::chrono::high_resolution_clock::now() - start;
          return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        };
    };
}

#endif //WOTCPP_STATISTIQUES_H
