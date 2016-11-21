#include <iostream>
#include "include/webOfTrust.h"
#include "include/log.h"

#define WOT_FILE "wot.bin"

int main(int argc, char **argv) {
  using namespace std;
  using namespace libwot;
  srand(time(NULL));


  int32_t nbMembers = 8;
  int32_t maxCertStock = 3;

  if (nbMembers < maxCertStock) {
    cout << "NbMembers must be > maxCertStock" << endl;
    return 1;
  }

  // Allocating
  WebOfTrust* wot = new WebOfTrust(3);

  wot->addNode();
  wot->addNode();
  wot->addNode();
  wot->addNode();
  wot->addNode();
  wot->addNode();

  wot->getNodeAt(1)->addLinkTo((uint32_t)0);
  wot->getNodeAt(2)->addLinkTo((uint32_t)0);
  wot->getNodeAt(3)->addLinkTo((uint32_t)0);
  wot->getNodeAt(4)->addLinkTo((uint32_t)0);
  wot->getNodeAt(5)->addLinkTo((uint32_t)0);

  Log::setEnabled(true);


  wot->showTable();
  wot->showGraphviz();
  Log() << "Sentries count = " << wot->getSentries(1).nbNodes;
  Log() << "Non-Sentries count = " << wot->getNonSentries(1).nbNodes;

  delete wot;
}
