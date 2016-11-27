#ifndef IDENTITY_H
#define IDENTITY_H

#include <cstdint>
#include <cstddef>
#include <vector>

namespace libsimu {

  typedef uint32_t UID;
  typedef uint32_t WID;

  class Identity {

    public :

      Identity(uint32_t sigPersoCible);
      ~Identity();

      bool estMembre;
      bool aEteMembre;
      UID uid;
      WID wotb_id;
      libwot::Node* wotb_node;
      uint32_t joinDate;
      uint32_t sigPersoCible;
      uint32_t derniereEmissionDeCertif;
  };
}

#endif
