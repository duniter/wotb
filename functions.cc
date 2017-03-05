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
using v8::String;
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

NAN_METHOD(newFileInstanceWin32) {
  v8::Local<v8::Object> path_to_file = Nan::To<v8::Object>(info[0]).ToLocalChecked();
  int path_length = Nan::To<int>(info[1]).FromJust();
  const char* buf = node::Buffer::Data(path_to_file);
  std::string path_ascii;
  path_ascii.assign(buf, path_length);
  AbstractWoT* wot = new FileWoT(path_ascii);
  wots.push_back(wot);
  info.GetReturnValue().Set(New<Number>(wots.size() - 1));
}

NAN_METHOD(newMemoryInstance) {
  AbstractWoT* wot;
  if (info.Length() == 1) {
    // Make a copy
    int wotID = Nan::To<int>(info[0]).FromJust();
    AbstractWoT* sourceWoT = wots[wotID];
    wot = new MemoryWoT(sourceWoT->getWoT());
  } else {
    // Create a new one
    wot = new MemoryWoT();
  }
  wots.push_back(wot);
  info.GetReturnValue().Set(New<Number>(wots.size() - 1));
}

NAN_METHOD(clearInstance) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  // Free the memory of this wot
  delete wot;
  // Eventually remove the wot from the list
  if (wotID == wots.size()) {
    wots.pop_back();
  }
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

NAN_METHOD(dumpWoT) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  std::string value(wot->dumpWoT());
  info.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), value.c_str(), String::kNormalString));
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

NAN_METHOD(getSentries) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t d_min = Nan::To<int32_t>(info[1]).FromJust();
  Local<Array> result_list = New<Array>();
  WoTSet sentries = wot->getSentries(d_min);
  for (unsigned int i = 0; i < sentries.nbNodes; i++ ) {
    Local<Number> sentry = New<Number>(sentries.nodes[i]);
    result_list->Set(i, sentry);
  }
  delete[] sentries.nodes;
  info.GetReturnValue().Set(result_list);
}

NAN_METHOD(getNonSentries) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t d_min = Nan::To<int32_t>(info[1]).FromJust();
  Local<Array> result_list = New<Array>();
  WoTSet nonSentries = wot->getNonSentries(d_min);
  for (unsigned int i = 0; i < nonSentries.nbNodes; i++ ) {
    Local<Number> nonSentry = New<Number>(nonSentries.nodes[i]);
    result_list->Set(i, nonSentry);
  }
  delete[] nonSentries.nodes;
  info.GetReturnValue().Set(result_list);
}

NAN_METHOD(getDisabled) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t d_min = Nan::To<int32_t>(info[1]).FromJust();
  Local<Array> result_list = New<Array>();
  WoTSet disabled = wot->getDisabled();
  for (unsigned int i = 0; i < disabled.nbNodes; i++ ) {
    Local<Number> nonSentry = New<Number>(disabled.nodes[i]);
    result_list->Set(i, nonSentry);
  }
  delete[] disabled.nodes;
  info.GetReturnValue().Set(result_list);
}

NAN_METHOD(getPaths) {
  int wotID = Nan::To<int>(info[0]).FromJust();
  AbstractWoT* wot = wots[wotID];
  int32_t from = Nan::To<int32_t>(info[1]).FromJust();
  int32_t to = Nan::To<int32_t>(info[2]).FromJust();
  int32_t k_max = Nan::To<int32_t>(info[3]).FromJust();
  Local<Array> result_list = New<Array>();
  std::vector<std::vector<uint32_t>> paths = wot->getPaths(from, to, k_max);
  for (unsigned int i = 0; i < paths.size(); i++ ) {
    Local<Array> path = New<Array>();
    for (int j = 0; j < paths[i].size(); j++) {
      path->Set(j, New<Number>(paths[i][j]));
    }
    result_list->Set(i, path);
  }
  info.GetReturnValue().Set(result_list);
}
