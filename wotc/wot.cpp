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

    bool hasCert(Node *node, int32_t number, int32_t sizeOfArray) {
        int32_t *end = node->links + sizeOfArray;
        // find the value 0:
        int32_t *result = find(node->links, end, number);
        return result != end;
    }

    void showTable(WebOfTrust *wot) {
        char s[32];
        cout << "[" << setw(sprintf(s, "%d", wot->nbMembers - 1)) << right << "M" << "] -> " << "Certifications" <<
        endl;
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            cout << "[" << setw(sprintf(s, "%d", wot->nbMembers - 1)) << right << i << "]" << " -> ";
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
            wotChecked[wot->nodes[m1].links[j]] = true;
        }
        if (distance < distanceMax) {
            // Look one level deeper
            for (int32_t j = 0; j < wot->nodes[m1].nbLinks; j++) {
                checkMatches(wot->nodes[m1].links[j], distance + 1, distanceMax, wot, wotChecked);
            }
        }
    }

    void findMatches(int32_t m1, int32_t distanceMax, WebOfTrust *wot, bool *wotChecked) {
        return checkMatches(m1, 1, distanceMax, wot, wotChecked);
    }

    WebOfTrust *createRandomWoT(int32_t nbMembers, int32_t maxCertStock) {
        WebOfTrust *wot = new WebOfTrust;
        wot->nbMembers = nbMembers;
        wot->nodes = new Node[nbMembers];
        for (int32_t i = 0; i < nbMembers; i++) {
            wot->nodes[i].nbLinks = maxCertStock;
            wot->nodes[i].links = new int32_t[maxCertStock];
            for (int32_t j = 0; j < maxCertStock; j++) {
                double r;
                int32_t randMember;
                bool isNotValid = false;
                do {
                    r = ((double) rand() / (RAND_MAX));
                    randMember = (int32_t) lrint(r * (nbMembers - 1));
                    isNotValid = randMember == i && hasCert(&wot->nodes[i], randMember, maxCertStock);
                }
                while (isNotValid);
                wot->nodes[i].links[j] = randMember;
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
            wot2->nodes[i].links = new int32_t[wot2->nodes[i].nbLinks];
            myFile.read((char *) wot2->nodes[i].links, sizeof(int32_t) * wot2->nodes[i].nbLinks);
        }
        myFile.close();
        return wot2;
    }

    int32_t wotMatch(int32_t member, WebOfTrust *wot) {
        bool *wotMatches = new bool[wot->nbMembers];
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            wotMatches[i] = false;
        }
        int32_t memberToCheck = 0;
        wotMatches[memberToCheck] = true;
        int32_t success = 0;
        findMatches(memberToCheck, 5, wot, wotMatches);
        for (int32_t i = 0; i < wot->nbMembers; i++) {
            if (wotMatches[i]) {
                success++;
            }
        }
        return success;
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
        node->nbLinks = 2;
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

}
