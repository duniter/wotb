#include <node.h>
#include <nan.h>

NAN_METHOD(newFileInstance);
NAN_METHOD(newMemoryInstance);
NAN_METHOD(showWoT);
NAN_METHOD(showGraph);
NAN_METHOD(resetWoT);
NAN_METHOD(getWoTSize);
NAN_METHOD(addNode);
NAN_METHOD(removeNode);
NAN_METHOD(isEnabled);
NAN_METHOD(setEnabled);
NAN_METHOD(existsLink);
NAN_METHOD(addLink);
NAN_METHOD(removeLink);
NAN_METHOD(isOutdistanced);
