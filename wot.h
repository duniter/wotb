#ifndef WOT_H

#define WOT_H

#include <chrono>
#include "wot_struct.h"

namespace libwot {

    using namespace std::chrono;

    bool hasCert(Node *node, int32_t number, int32_t sizeOfArray);

    void showTable(WebOfTrust *wot);

    void showDurationMS(high_resolution_clock::time_point t1);

    void showDuration(high_resolution_clock::time_point t1);

    bool checkLinks(int32_t m1, int32_t m2, int32_t distance, int32_t distanceMax, int32_t maxCertStock, int32_t **wot);

    void checkMatches(int32_t m1, int32_t distance, int32_t distanceMax, WebOfTrust *wot, bool *wotChecked);

    void findMatches(int32_t m1, int32_t distanceMax, WebOfTrust *wot, bool *wotChecked);

    WebOfTrust *createRandomWoT(int32_t nbMembers, int32_t maxCertStock);

    void freeWoT(WebOfTrust *wot);

    void writeNode(FILE *pFile, Node *node);

    void writeWoT(string f, WebOfTrust *wot);

    WebOfTrust *readWoT(string f);

    int32_t wotMatch(int32_t member, WebOfTrust *wot);

    // =========== FUNCTIONAL ==============

    int32_t getWoTSize(string f);

    int32_t addNode(string f);

}

#endif
