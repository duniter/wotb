#include <node.h>
#include <nan.h>
#include "wotc/wot.h"
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <iostream>

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

libwot::WebOfTrust* privateWoT = libwot::createRandomWoT(0, 0);
bool inited = false;

/***********************************
* WoT structure handling: FILE
************************************/

libwot::WebOfTrust* loadFromFile(std::string file) {
  return libwot::readWoT(file);
}

void writeToFile(std::string file, libwot::WebOfTrust* wot) {
  return libwot::writeWoT(file, wot);
}

void doFreeMemory(libwot::WebOfTrust* wot) {
  libwot::freeWoT(wot);
}

void resetWoTFile(std::string file, libwot::WebOfTrust* wot) {
  remove((char *) file.c_str());
  libwot::createNewWoTIfNotExist(file);
}

/***********************************
* WoT structure handling: MEMORY
************************************/

libwot::WebOfTrust* loadFromMemory(std::string file) {
  return privateWoT;
}

void writeToMemory(std::string file, libwot::WebOfTrust* wot) {
  // Do nothing, memory is already written
}

void doNotFreeMemory(libwot::WebOfTrust* wot) {
  // Do nothing, memory must be left intact
}

void resetMemory(std::string file, libwot::WebOfTrust* wot) {
  libwot::freeWoT(wot);
  privateWoT = libwot::createRandomWoT(0, 0);
}

string getFileName(v8::Local<v8::Value> value) {
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(value).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  return string(*fileParam);
}

/***********************************
* INIT: DEFAULT TO MEMORY
************************************/

std::string fileName;

// Function pointers
libwot::WebOfTrust* (*loadWoT)(std::string file) = loadFromMemory;
void (*writeWoT)(std::string file, libwot::WebOfTrust* wot) = writeToMemory;
void (*freeMemory)(libwot::WebOfTrust* wot) = doNotFreeMemory;
void (*resetData)(std::string file, libwot::WebOfTrust* wot) = resetMemory;

/***********************************
* PUBLIC METHODS
************************************/

NAN_METHOD(plugOnFile) {
  doFreeMemory(privateWoT);
  fileName = getFileName(info[0]);
  loadWoT = loadFromFile;
  writeWoT = writeToFile;
  freeMemory = doFreeMemory;
  // Create file if not exist
  libwot::createNewWoTIfNotExist(fileName);
  libwot::WebOfTrust* wot = loadWoT(fileName);
  info.GetReturnValue().Set(New<Number>(wot->nbMembers));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(resetWoT) {
  resetData(fileName, privateWoT);
}

NAN_METHOD(getWoTSize) {
  libwot::WebOfTrust* wot = loadWoT(fileName);
  info.GetReturnValue().Set(New<Number>(wot->nbMembers));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(addNode) {
  libwot::WebOfTrust* wot = loadWoT(fileName);
  int32_t res = libwot::addNode(wot);
  info.GetReturnValue().Set(New<Number>(res));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(isEnabled) {
  int32_t member = Nan::To<int32_t>(info[0]).FromJust();
  libwot::WebOfTrust* wot = loadWoT(fileName);
  info.GetReturnValue().Set(New<Boolean>(libwot::isEnabled(member, wot)));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(setEnabled) {
  bool enabled = Nan::To<bool>(info[0]).FromJust();
  int32_t member = Nan::To<int32_t>(info[1]).FromJust();
  libwot::WebOfTrust* wot = loadWoT(fileName);
  info.GetReturnValue().Set(New<Boolean>(libwot::setEnabled(enabled, member, wot)));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(existsLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  libwot::WebOfTrust* wot = loadWoT(fileName);
  info.GetReturnValue().Set(New<Boolean>(libwot::existsLink(from, to, wot)));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(addLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  libwot::WebOfTrust* wot = loadWoT(fileName);
  int32_t res = libwot::addLink(from, to, wot);
  info.GetReturnValue().Set(New<Number>(res));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(removeLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  libwot::WebOfTrust* wot = loadWoT(fileName);
  info.GetReturnValue().Set(New<Number>(libwot::removeLink(from, to, wot)));
  writeWoT(fileName, wot);
  freeMemory(wot);
}

NAN_METHOD(isOutdistanced) {
  int32_t member = Nan::To<int32_t>(info[0]).FromJust();
  int32_t d_min = Nan::To<int32_t>(info[1]).FromJust();
  int32_t k_max = Nan::To<int32_t>(info[2]).FromJust();
  double x_percent = Nan::To<double>(info[3]).FromJust();
  libwot::WebOfTrust* wot = loadWoT(fileName);
  info.GetReturnValue().Set(New<Boolean>((libwot::isOutdistanced(member, d_min, k_max, x_percent, wot)).isOutdistanced));
  writeWoT(fileName, wot);
  freeMemory(wot);
}
