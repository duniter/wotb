#ifndef WOT_STRUCT_H

#define WOT_STRUCT_H

namespace libwot {

    using namespace std;

    const bool DISPLAY_DEBUG = false;

    struct DistanceResult {

        int32_t nbSentries;
        int32_t nbSuccess;
        bool isOutdistanced;
    };

    struct Node {

        bool enabled;
        int32_t nbLinks;
        int32_t nbIssued;
        int32_t* links;
    };

    struct WebOfTrust {

        int32_t nbMembers;
        Node* nodes;
    };

}
#endif
