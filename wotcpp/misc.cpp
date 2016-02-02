#include "include/misc.h"

namespace libwot {

  uint32_t countDigits(uint32_t number) {
    return (number < 10) ? 1 : countDigits(number / 10) + 1;
  }

}
