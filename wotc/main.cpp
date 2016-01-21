#include <iostream>
#include "wot.cpp"

using namespace std;
using namespace libwot;

void showGraphviz(WebOfTrust* wot) {
    char s[32];
    cout << "digraph G {" << endl << endl;
    for (int32_t i = 0; i < wot->nbMembers; i++) {
        for (int32_t j = 0; j < wot->nodes[i].nbLinks; j++) {
            cout << "    " << i << " -> " << wot->nodes[i].links[j] << "" << endl;
        }
//        cout << endl;
    }
    cout << "}" << endl;
}

int main() {

    string EXPORT_FILE = "/home/cgeek/dev/wotb/wotc/wot.bin";

    srand(time(NULL));

    int32_t nbMembers = 8*1;
    int32_t maxCertStock = 3;

    if (nbMembers < maxCertStock) {
        cout << "NbMembers must be > maxCertStock" << endl;
        return 1;
    }

    // Allocating
    WebOfTrust* wot = libwot::createRandomWoT(nbMembers, maxCertStock);

    showTable(wot);
    showGraphviz(wot);

    //======================
    // WoT match
    //======================
//    bool match = wotMatch(0, 1, 5, wot).isOutdistanced;

    //======================
    // Write WoT
    //======================
    writeWoT(EXPORT_FILE, wot);

    setEnabled(false, 0, wot);
    setEnabled(true, 1, wot);
    setEnabled(false, 2, wot);

    //======================
    // Read WoT
    //======================
    WebOfTrust* wot2 = readWoT(EXPORT_FILE);

    showTable(wot2);

    for (int32_t i = 1; i < nbMembers; i++) {
        cout << "Link from 0 to " << i << " exists: " << existsLink(0, i, wot) << endl;
    }
    freeWoT(wot2);
    addLink(10, 0, wot);
    addLink(43, 2, wot);
    addLink(46, 2, wot);
    removeLink(46, 2, wot);
    removeLink(46, 2, wot);
    removeLink(43, 2, wot);
    wot2 = readWoT(EXPORT_FILE);

    removeNode(wot2);
    showTable(wot2);

    for (int j = 0; j < nbMembers; ++j) {
        cout << "Is outdistanced " << j << " from the WoT = " << isOutdistanced(j, 1, 2, 1.0, wot2).isOutdistanced << endl;
    }

    // Free
    freeWoT(wot);
    freeWoT(wot2);

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