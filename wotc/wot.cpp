#include <iostream>
#include <iomanip>
#include <cstdio>
#include <tgmath.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include "wot.h"

namespace libwot {

    using namespace std;
    using namespace std::chrono;
    using namespace libwot;

    int32_t* pointerOfCert(Node *node, int32_t number) {
        int32_t* end = node->links + node->nbLinks;
        return find(node->links, end, number);
    }

    bool hasCert(Node *node, int32_t number) {
        int32_t* end = node->links + node->nbLinks;
        int32_t* result = pointerOfCert(node, number);
        return result != end;
    }

    void showTable(WebOfTrust *wot) {
        char s[32];
        cout << "[" << setw(sprintf(s, "%d", wot->nbMembers - 1)) << right << "M" << "] [E] [R] [I] -> " << "Links" <<
        endl;
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            cout << "[" << setw(sprintf(s, "%d", wot->nbMembers - 1)) << right << i << "] [" << wot->nodes[i].enabled << "] [" << wot->nodes[i].nbLinks << "] [" << wot->nodes[i].nbIssued << "] -> ";
            for (int32_t j = 0; j < wot->nodes[i].nbLinks; j++) {
                cout << setw(sprintf(s, "%d", wot->nbMembers)) << right << wot->nodes[i].links[j] << " | ";
            }
            cout << endl;
        }
    }

    void showLinks(int32_t member, int32_t distance, int32_t distanceMax, int32_t maxCertStock, int32_t **wot) {
        if (distance > distanceMax) {
            return;
        }
        for (int32_t j = 0; j < maxCertStock; j++) {
            cout << "    " << wot[member][j] << " -> " << member << ";" << endl;
        }
        for (int32_t j = 0; j < maxCertStock; j++) {
            showLinks(wot[member][j], distance + 1, distanceMax, maxCertStock, wot);
        }
    }

// -------------------------------------------

    bool checkLinks(int32_t m1, int32_t m2, int32_t distance, int32_t distanceMax, int32_t maxCertStock,
                    int32_t **wot) {
        bool ok = false;
        for (int32_t j = 0; j < maxCertStock; j++) {
            if (wot[m1][j] == m2) {
                ok = true;
                break;
            }
        }
        if (ok) {
            return true;
        }
        else {
            if (distance < distanceMax) {
                // Look one level deeper
                for (int32_t j = 0; j < maxCertStock; j++) {
                    if (checkLinks(wot[m1][j], m2, distance + 1, distanceMax, maxCertStock, wot)) {
                        ok = true;
                        break;
                    }
                }
            }
            return ok;
        }
    }

    bool checkDistance(int32_t m1, int32_t m2, int32_t distanceMax, int32_t nbMembers, int32_t maxCertStock,
                       int32_t **wot) {
        return checkLinks(m1, m2, 1, distanceMax, maxCertStock, wot);
    }

    void checkMatches(int32_t m1, int32_t distance, int32_t distanceMax, WebOfTrust *wot, bool *wotChecked) {
        // Mark as checked the linking nodes at this level
        for (int32_t j = 0; j < wot->nodes[m1].nbLinks; j++) {
            if (DISPLAY_DEBUG) {
                cout << "Match " << wot->nodes[m1].links[j] << " -> " << m1 << endl;
            }
            wotChecked[wot->nodes[m1].links[j]] = true;
        }
        if (distance < distanceMax) {
            // Look one level deeper
            for (int32_t j = 0; j < wot->nodes[m1].nbLinks; j++) {
                checkMatches(wot->nodes[m1].links[j], distance + 1, distanceMax, wot, wotChecked);
            }
        }
    }

    void findMatches(int32_t m1, int32_t k_max, WebOfTrust *wot, bool *wotChecked) {
        if (k_max >= 1) {
            checkMatches(m1, 1, k_max, wot, wotChecked);
        }
    }

    WebOfTrust *createRandomWoT(int32_t nbMembers, int32_t maxCertStock) {
        WebOfTrust *wot = new WebOfTrust;
        wot->nbMembers = nbMembers;
        wot->nodes = new Node[nbMembers];
        for (int32_t i = 0; i < nbMembers; i++) {
            wot->nodes[i].enabled = true;
            wot->nodes[i].nbLinks = maxCertStock;
            wot->nodes[i].links = new int32_t[maxCertStock];
            // Init to empty value -1
            for (int32_t j = 0; j < maxCertStock; j++) {
                wot->nodes[i].links[j] = -1;
            }
            // Random valuation
            for (int32_t j = 0; j < maxCertStock; j++) {
                double r;
                int32_t randMember;
                bool isNotValid = false;
                do {
                    r = ((double) rand() / (RAND_MAX));
                    double randDouble = floor(r * (nbMembers));
                    randMember = (int32_t) ((int) randDouble);
                    bool isTheLocalMember = randMember == i;
                    bool hasTheCert = hasCert(&wot->nodes[i], randMember);
                    isNotValid = isTheLocalMember || hasTheCert;
                }
                while (isNotValid);
                wot->nodes[i].links[j] = randMember;
                wot->nodes[randMember].nbIssued++;
            }
        }
        return wot;
    }

    void freeNode(Node* node) {
        delete[] node->links;
    }

    void freeWoT(WebOfTrust *wot) {
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            freeNode(&wot->nodes[i]);
        }
        delete[] wot->nodes;
    }

    void writeNode(FILE *pFile, Node *node) {
        fwrite(&node->nbLinks, 1, sizeof(int32_t), pFile);
        fwrite(&node->nbIssued, 1, sizeof(int32_t), pFile);
        fwrite(&node->enabled, 1, sizeof(bool), pFile);
        fwrite(node->links, 1, node->nbLinks * sizeof(int32_t), pFile);
    }

    void writeWoT(string f, WebOfTrust *wot) {
        FILE *pFile;
        pFile = fopen((char *) f.c_str(), "wb");
        fwrite(&wot->nbMembers, sizeof(int32_t), 1, pFile);
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            writeNode(pFile, &wot->nodes[i]);
        }
        fclose(pFile);
    }

    WebOfTrust *readWoT(string f) {
        ifstream myFile((char *) f.c_str(), ios::in | ios::binary);
        if (!myFile) {
            cout << "Error reading file" << endl;
        }
        WebOfTrust *wot2 = new WebOfTrust;
        myFile.read((char *) &wot2->nbMembers, sizeof(int32_t));
        wot2->nodes = new Node[wot2->nbMembers];
        for (int32_t i = 0; i < wot2->nbMembers; i++) {
            myFile.read((char *) &wot2->nodes[i].nbLinks, sizeof(int32_t));
            myFile.read((char *) &wot2->nodes[i].nbIssued, sizeof(int32_t));
            myFile.read((char *) &wot2->nodes[i].enabled, sizeof(bool));
            wot2->nodes[i].links = new int32_t[wot2->nodes[i].nbLinks];
            myFile.read((char *) wot2->nodes[i].links, sizeof(int32_t) * wot2->nodes[i].nbLinks);
        }
        myFile.close();
        return wot2;
    }

    DistanceResult wotMatch(int32_t memberToCheck, int32_t d_min, int32_t k_max, WebOfTrust *wot) {
        DistanceResult result;
        result.nbSentries = 0;
        bool *wotMatches = new bool[wot->nbMembers];
        bool *sentries = new bool[wot->nbMembers];
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            // We will check only members with at least d_min links (other do not participate the distance rule)
            sentries[i] = wot->nodes[i].nbIssued >= d_min;
            wotMatches[i] = false;
        }
        // The member to check is not considered a sentry
        sentries[memberToCheck] = false;
        wotMatches[memberToCheck] = true;
        result.nbSuccess = 0;
        findMatches(memberToCheck, k_max, wot, wotMatches);
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            if (sentries[i]) {
                result.nbSentries++;
                if (wotMatches[i]) {
                    if (DISPLAY_DEBUG) {
                        cout << "Sentry " << i << ": OK" << endl;
                    }
                    result.nbSuccess++;
                } else {
                    if (DISPLAY_DEBUG) {
                        cout << "Sentry " << i << ": KO" << endl;
                    }
                }
            } else {
                if (DISPLAY_DEBUG) {
                    cout << "NON-Sentry "  << i << endl;
                }
            }
        }
        result.isOutdistanced = result.nbSuccess < result.nbSentries;
        delete[] wotMatches;
        delete[] sentries;
        return result;
    }

    //============== FUNCTIONAL ==================

    int32_t getWoTSize(string f) {
        ifstream myFile((char *) f.c_str(), ios::in | ios::binary);
        int32_t wotSize = 0;
        if (myFile) {
            myFile.read((char *) &wotSize, sizeof(int32_t));
            myFile.close();
        }
        return wotSize;
    }

    int32_t addNode(string f) {
        ifstream myFile((char *) f.c_str(), ios::in | ios::binary);

        // Create a file with an empty WoT if not exists
        if (!myFile) {
            WebOfTrust* wot = createRandomWoT(0, 0);
            writeWoT(f, wot);
            freeWoT(wot);
        }
        // Get current WoT size
        int32_t newWotSize = getWoTSize(f) + 1;

        // Create a new Node
        Node* node = new Node;
        node->nbLinks = 0;
        node->nbIssued = 0;
        node->links = new int32_t[0];

        // Write new node
        FILE *pFile;
        pFile = fopen((char *) f.c_str(), "a+b");
        writeNode(pFile, node);

        // Close file
        fclose(pFile);

        // Increment the number of members
        pFile = fopen((char *) f.c_str(), "r+b");
        fwrite(&newWotSize, sizeof(int32_t), 1, pFile);

        // Close file
        fclose(pFile);

        // Free memory
        freeNode(node);
        return newWotSize - 1;
    }

    bool isEnabled(int32_t member, string f) {
        bool enabled = false;
        WebOfTrust* wot = readWoT(f);
        enabled = wot->nodes[member].enabled;
        freeWoT(wot);
        return enabled;
    }

    bool setEnabled(bool enabled, int32_t member, string f) {
        WebOfTrust* wot = readWoT(f);
        wot->nodes[member].enabled = enabled;
        writeWoT(f, wot);
        freeWoT(wot);
        return enabled;
    }

    bool existsLink(int32_t from, int32_t to, string f) {
        WebOfTrust* wot = readWoT(f);
        bool exists = hasCert(&wot->nodes[to], from);
        freeWoT(wot);
        return exists;
    }

    int32_t addLink(int32_t from, int32_t to, string f) {
        WebOfTrust* wot = readWoT(f);
        Node* node = &wot->nodes[to];
        // Add only if not exists already & from node exists
        if (!hasCert(node, from) && from < wot->nbMembers) {
            int32_t* newCerts = new int32_t[node->nbLinks + 1];
            for (int i = 0; i < node->nbLinks; ++i) {
                newCerts[i] = node->links[i];
            }
            newCerts[node->nbLinks] = from;
            node->nbLinks++;
            delete[] node->links;
            node->links = newCerts;
            // Increment issued links number of "from"
            wot->nodes[from].nbIssued++;
            writeWoT(f, wot);
        }
        int32_t linksCount = node->nbLinks;
        freeWoT(wot);
        return linksCount;
    }

    int32_t removeLink(int32_t from, int32_t to, string f) {
        WebOfTrust* wot = readWoT(f);
        Node* node = &wot->nodes[to];
        int32_t* index = pointerOfCert(node, from);
        int32_t* end = node->links + node->nbLinks;
        if (index != end) {
            // Remove only if not exists
            int32_t* newCerts = new int32_t[node->nbLinks - 1];
            bool found = false;
            for (int i = 0; i < node->nbLinks; ++i) {
                if (!found && &node->links[i] == index) {
                    found = true;
                }
                if (&node->links[i] != index) {
                    newCerts[!found ? i : i - 1] = node->links[i];
                }
            }
            node->nbLinks--;
            delete[] node->links;
            node->links = newCerts;
            wot->nodes[from].nbIssued--;
            writeWoT(f, wot);
        }
        int32_t linksCount = node->nbLinks;
        freeWoT(wot);
        return linksCount;
    }

    DistanceResult isOutdistanced(int32_t member, int32_t d_min, int32_t k_max, double x_percent, string f) {
        WebOfTrust* wot = readWoT(f);
        DistanceResult result = wotMatch(member, d_min, k_max, wot);
        // We override the result with X% rule
        if (DISPLAY_DEBUG) {
            cout << "Success = " << result.nbSuccess << " / " << x_percent * result.nbSentries << " (" << x_percent << " x " << result.nbSentries << ")" << endl;
        }
        result.isOutdistanced = result.nbSuccess < x_percent * result.nbSentries;
        freeWoT(wot);
        return result;
    }

}
