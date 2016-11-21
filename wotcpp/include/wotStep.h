#ifndef WOTSTEP_H
#define WOTSTEP_H

namespace libwot {

  struct WotStep {
    WotStep* previous;
    uint32_t member;
    uint32_t distance;
  };
}

#endif
