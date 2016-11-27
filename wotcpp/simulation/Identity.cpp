#include <Duniter.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "simulation/Identity.h"

namespace libsimu {

  using namespace std;
  using namespace libsimu;

    Identity::Identity(uint32_t sigPersoCible) : sigPersoCible(sigPersoCible) {
        wotb_node = NULL;
    }

    Identity::~Identity() {
    }
}
