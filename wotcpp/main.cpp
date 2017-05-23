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
  wot->addNode();

  wot->getNodeAt(1)->addLinkTo((uint32_t)0);
  wot->getNodeAt(2)->addLinkTo((uint32_t)0);
  wot->getNodeAt(3)->addLinkTo((uint32_t)0);
  wot->getNodeAt(4)->addLinkTo((uint32_t)0);
  wot->getNodeAt(5)->addLinkTo((uint32_t)0);
  // Add a path from 0 to 1
  wot->getNodeAt(0)->addLinkTo((uint32_t)2);
  wot->getNodeAt(2)->addLinkTo((uint32_t)3);
  wot->getNodeAt(3)->addLinkTo((uint32_t)1);
  wot->getNodeAt(3)->addLinkTo((uint32_t)1);
  // Add a path from 0 to 1
  wot->getNodeAt(0)->addLinkTo((uint32_t)4);
  wot->getNodeAt(4)->addLinkTo((uint32_t)1);
  wot->getNodeAt(5)->setEnabled(false);

  // We can make mem copies
  WebOfTrust* wot2 = wot->clone();
  delete wot;
//  WebOfTrust* wot2 = wot;

  Log::setEnabled(true);


//  wot2->showTable();
//  wot2->showGraphviz();
  Log() << "DUMP:";
  Log() << wot2->dump();
  Log() << "Sentries count = " << wot2->getSentries(1).size();
  Log() << "Non-Sentries count = " << wot2->getNonSentries(1).size();
  Log() << "Disabled count = " << wot2->getDisabled().size();

  uint32_t by = 0;
  uint32_t target = 1;
  uint32_t k_max = 3;
  std::vector<std::vector<uint32_t>> steps = wot2->getPaths(by, target, k_max);
  if (steps.size() == 0) {
    Log() << target << " is NOT reachable by " << by;
  } else {
    for (int i = 0; i < steps.size(); i++) {
      cout << target << " is reached by " << by << " through: ";
      for (int j = 0; j < steps[i].size(); j++) {
        if (j > 0) {
          cout << " -> ";
        }
        cout << steps[i][j];
      }
      cout << endl;
    }
  }

  delete wot2;
}
