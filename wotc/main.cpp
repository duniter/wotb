#include <iostream>
#include "wot.cpp"

using namespace std;
using namespace libwot;

int main() {

    string EXPORT_FILE = "/home/cgeek/dev/wotb/wotc/wot.bin";

    srand(time(NULL));

    int32_t nbMembers = 6*1;
    int32_t maxCertStock = 3;

    if (nbMembers < maxCertStock) {
        cout << "NbMembers must be > maxCertStock" << endl;
        return 1;
    }

    // Allocating
    WebOfTrust* wot = libwot::createRandomWoT(nbMembers, maxCertStock);

    showTable(wot);

    //======================
    // WoT match
    //======================
//    bool match = wotMatch(0, 1, 5, wot).isOutdistanced;

    //======================
    // Write WoT
    //======================
    writeWoT(EXPORT_FILE, wot);

    setEnabled(false, 0, EXPORT_FILE);
    setEnabled(true, 1, EXPORT_FILE);
    setEnabled(false, 2, EXPORT_FILE);

    //======================
    // Read WoT
    //======================
    WebOfTrust* wot2 = readWoT(EXPORT_FILE);

    showTable(wot2);

    for (int32_t i = 1; i < nbMembers; i++) {
        cout << "Link from 0 to " << i << " exists: " << existsLink(0, i, EXPORT_FILE) << endl;
    }
    freeWoT(wot2);
    addLink(10, 0, EXPORT_FILE);
    addLink(43, 2, EXPORT_FILE);
    addLink(46, 2, EXPORT_FILE);
    removeLink(46, 2, EXPORT_FILE);
    removeLink(46, 2, EXPORT_FILE);
    removeLink(43, 2, EXPORT_FILE);
    wot2 = readWoT(EXPORT_FILE);

    showTable(wot2);

    // Free
    freeWoT(wot);
    freeWoT(wot2);

    for (int j = 0; j < nbMembers; ++j) {
        cout << "Is outdistanced " << j << " from the WoT = " << isOutdistanced(j, 1, 2, 1.0, EXPORT_FILE).isOutdistanced << endl;
    }

    return 0;
}


//    cout << "N";
//    for (int32_t stock = 0; stock < 33; stock++) {
//        cout << "\t" << stock;
//    }
//    cout << endl;
//    for (int32_t members = 10; members <= 1000*1000; members *= 10) {
//        cout << members;
//        for (int32_t stock = 0; stock < 33; stock++) {
//            int32_t** wot = createRandomWoT(members, stock);
//            int32_t matchedMembers = wotMatch(0, wot, members, stock);
//            cout << "\t" << matchedMembers;
//            freeWoT(members, wot);
//        }
//        cout << endl;
//    }