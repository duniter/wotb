#ifndef CERT_H
#define CERT_H

#include <cstdint>
#include <cstddef>
#include <vector>


namespace libsimu {

    typedef pair<uint32_t, uint32_t> Lien;

  class Certification {

    public :

      Certification();
      ~Certification();

      Lien link;
      Identity* emetteur;
      Identity* receveur;
      uint32_t dateOfIssuance;
  };
}

#endif
