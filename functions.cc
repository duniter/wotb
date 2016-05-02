#include <node.h>
#include <nan.h>
#include "wotcpp/include/AbstractWoT.h"
#include "wotcpp/include/FileWoT.h"
#include "wotcpp/include/MemoryWoT.h"
#include "wotcpp/include/webOfTrust.h"
#include "wotcpp/include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using v8::Local;
using v8::Object;
using v8::Boolean;
using v8::Integer;
using v8::Number;
using v8::Value;
using v8::Isolate;
using v8::HandleScope;
using v8::Array;
using Nan::ReturnValue;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

using namespace std;
using namespace libwot;

std::vector<AbstractWoT*> wots;

/***********************************
* PUBLIC METHODS
************************************/

NAN_METHOD(setVerbose) {
  bool verbose = Nan::To<bool>(info[0]).FromJust();
  Log::setEnabled(verbose);
}

NAN_METHOD(newFileInstance) {
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[0]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  AbstractWoT* wot = new FileWoT(string(*fileParam));
  wots.push_back(wot);
  info.GetReturnValue().Set(New<Number>(wots.size() - 1));
}

NAN_METHOD(newMemoryInstance) {
  AbstractWoT* wot = new MemoryWoT();
  wots.push_back(wot);
  info.GetReturnValue().Set(New<Number>(wots.size() - 1));
}

NAN_METHOD(resetWoT) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  wot->reset();
}

NAN_METHOD(showWoT) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  wot->showWoT();
}

NAN_METHOD(showGraph) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  wot->showGraph();
}

NAN_METHOD(getWoTSize) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  info.GetReturnValue().Set(New<Number>(wot->getWoTSize()));
}

NAN_METHOD(addNode) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  info.GetReturnValue().Set(New<Number>(wot->addNode()));
}

NAN_METHOD(removeNode) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  info.GetReturnValue().Set(New<Number>(wot->removeNode()));
}

NAN_METHOD(setMaxCert) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  int32_t maxCerts = Nan::To<int32_t>(info[1]).FromJust();
  AbstractWoT* wot = wots[wotID];
  wot->setMaxCert(maxCerts);
}

NAN_METHOD(getMaxCert) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  info.GetReturnValue().Set(New<Number>(wot->getMaxCert()));
}

NAN_METHOD(isEnabled) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t member = Nan::To<int32_t>(info[1]).FromJust();
  info.GetReturnValue().Set(New<Boolean>(wot->isEnabled(member)));
}

NAN_METHOD(setEnabled) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  bool enabled = Nan::To<bool>(info[1]).FromJust();
  int32_t member = Nan::To<int32_t>(info[2]).FromJust();
  info.GetReturnValue().Set(New<Boolean>(wot->setEnabled(enabled, member)));
}

NAN_METHOD(existsLink) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t from = Nan::To<int32_t>(info[1]).FromJust();
  int32_t to = Nan::To<int32_t>(info[2]).FromJust();
  info.GetReturnValue().Set(New<Boolean>(wot->existsLink(from, to)));
}

NAN_METHOD(addLink) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t from = Nan::To<int32_t>(info[1]).FromJust();
  int32_t to = Nan::To<int32_t>(info[2]).FromJust();
  info.GetReturnValue().Set(New<Number>(wot->addLink(from, to)));
}

NAN_METHOD(removeLink) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t from = Nan::To<int32_t>(info[1]).FromJust();
  int32_t to = Nan::To<int32_t>(info[2]).FromJust();
  info.GetReturnValue().Set(New<Number>(wot->removeLink(from, to)));
}

NAN_METHOD(isOutdistanced) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t member = Nan::To<int32_t>(info[1]).FromJust();
  int32_t d_min = Nan::To<int32_t>(info[2]).FromJust();
  int32_t k_max = Nan::To<int32_t>(info[3]).FromJust();
  double x_percent = Nan::To<double>(info[4]).FromJust();
  info.GetReturnValue().Set(New<Boolean>((wot->isOutdistanced(member, d_min, k_max, x_percent))));
}
