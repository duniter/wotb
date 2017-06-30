#include <node.h>
#include <nan.h>

NAN_METHOD(newFileInstance);
NAN_METHOD(newFileInstanceWin32);
NAN_METHOD(newMemoryInstance);
NAN_METHOD(clearInstance);
NAN_METHOD(setVerbose);
NAN_METHOD(showWoT);
NAN_METHOD(dumpWoT);
NAN_METHOD(showGraph);
NAN_METHOD(resetWoT);
NAN_METHOD(getWoTSize);
NAN_METHOD(addNode);
NAN_METHOD(removeNode);
NAN_METHOD(setMaxCert);
NAN_METHOD(getMaxCert);
NAN_METHOD(isEnabled);
NAN_METHOD(setEnabled);
NAN_METHOD(existsLink);
NAN_METHOD(addLink);
NAN_METHOD(removeLink);
NAN_METHOD(isOutdistanced);
NAN_METHOD(detailedDistance);

// Data extraction
NAN_METHOD(getSentries);
NAN_METHOD(getNonSentries);
NAN_METHOD(getDisabled);
NAN_METHOD(getPaths);
