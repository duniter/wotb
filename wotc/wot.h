#ifndef WOT_H

#define WOT_H

#include <chrono>
#include "wot_struct.h"

namespace libwot {

    using namespace std::chrono;

    bool hasCert(Node *node, int32_t number);

    void showTable(WebOfTrust *wot);

    void showGraphviz(WebOfTrust *wot);

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

    int32_t wotMatch(int32_t member, int32_t d_min, int32_t k_max, double x_percent, WebOfTrust *wot);

    // =========== FUNCTIONAL ==============

    /**
     * Display additional informations when calling making some computations.
     */
    void setVerbose(bool verbose);

    void createNewWoTIfNotExist(string f);

    /**
     * Add a new member to the WoT, enabled but without any link.
     */
    int32_t addNode(WebOfTrust* wot);

    /**
     * Remove top Node from the WoT, and return the new last Node number.
     */
    int32_t removeNode(WebOfTrust* wot);

    /**
     * Check wether a member is enabled or not.
     */
    bool isEnabled(int32_t member, WebOfTrust* wot);

    /**
     * Disable a node and return its new enabled state.
     */
    bool setEnabled(bool enabled, int32_t member, WebOfTrust* wot);

    /**
     * Check wether a link exists from member 1 to member 2
     */
    bool existsLink(int32_t from, int32_t to, WebOfTrust* wot);

    /**
     * Add a link to a member and return how many he now has.
     */
    int32_t addLink(int32_t from, int32_t to, WebOfTrust* wot);

    /**
     * Remove a link of a member and return how many he now has.
     */
    int32_t removeLink(int32_t from, int32_t to, WebOfTrust* wot);

    /**
     * Check the distance of a member against the WoT for d_min and k_max parameters.
     *
     * * d_min is the minimum number of links a member must have to be checked against the distance rule
     * * k_max is the maximum distance that can be tested for distance rule
     */
    DistanceResult isOutdistanced(int32_t member, int32_t d_min, int32_t k_max, double x_percent, WebOfTrust* wot);

}

#endif
