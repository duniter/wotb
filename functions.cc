#include <node.h>
#include <nan.h>
#include "wotc/wot.h"
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

class AbstractWoT {

public:
  virtual void reset();
  virtual void showWoT();
  virtual void showGraph();
  virtual int32_t getWoTSize();
  virtual int32_t addNode();
  virtual int32_t removeNode();
  virtual bool isEnabled(int32_t node);
  virtual bool setEnabled(bool enabled, int32_t node);
  virtual bool existsLink(int32_t from, int32_t to);
  virtual int32_t addLink(int32_t from, int32_t to);
  virtual int32_t removeLink(int32_t from, int32_t to);
  virtual bool isOutdistanced(int32_t member, int32_t d_min, int32_t k_max, double x_percent);
};

class MemoryWoT : public AbstractWoT {

public:
  MemoryWoT() {
    wot = libwot::createRandomWoT(0,0);
  }
  ~MemoryWoT() {
    libwot::freeWoT(wot);
  }

  void reset() {
    libwot::freeWoT(wot);
    wot = libwot::createRandomWoT(0,0);
  }

  void showWoT() {
    return libwot::showTable(wot);
  }

  void showGraph() {
    return libwot::showGraphviz(wot);
  }

  int32_t getWoTSize() {
    return wot->nbMembers;
  }

  int32_t addNode() {
    return libwot::addNode(wot);
  }

  int32_t removeNode() {
    return libwot::removeNode(wot);
  }

  bool isEnabled(int32_t node) {
    return libwot::isEnabled(node, wot);
  }

  bool setEnabled(bool enabled, int32_t node) {
    return libwot::setEnabled(enabled, node, wot);
  }

  bool existsLink(int32_t from, int32_t to) {
    return libwot::existsLink(from, to, wot);
  }

  int32_t addLink(int32_t from, int32_t to) {
    return libwot::addLink(from, to, wot);
  }

  int32_t removeLink(int32_t from, int32_t to) {
    return libwot::removeLink(from, to, wot);
  }

  bool isOutdistanced(int32_t member, int32_t d_min, int32_t k_max, double x_percent) {
    return libwot::isOutdistanced(member, d_min, k_max, x_percent, wot).isOutdistanced;
  }

private:
  libwot::WebOfTrust* wot;
};

class FileWoT : public AbstractWoT {

public:
  FileWoT(std::string f) : filename(f) {
    // Create file if not exist
    libwot::createNewWoTIfNotExist(filename);
  }
  ~FileWoT() {}

  void reset() {
    remove((char *) filename.c_str());
    // Create file if not exist
    libwot::createNewWoTIfNotExist(filename);
  }

  void showWoT() {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    libwot::showTable(wot);
    libwot::freeWoT(wot);
  }

  void showGraph() {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    libwot::showGraphviz(wot);
    libwot::freeWoT(wot);
  }

  int32_t getWoTSize() {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    int32_t result = wot->nbMembers;
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  int32_t addNode() {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    int32_t result = libwot::addNode(wot);
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  int32_t removeNode() {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    int32_t result = libwot::removeNode(wot);
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  bool isEnabled(int32_t node) {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    bool result = libwot::isEnabled(node, wot);
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  bool setEnabled(bool enabled, int32_t node) {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    bool result = libwot::setEnabled(enabled, node, wot);
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  bool existsLink(int32_t from, int32_t to) {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    bool result = libwot::existsLink(from, to, wot);
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  int32_t addLink(int32_t from, int32_t to) {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    int32_t result = libwot::addLink(from, to, wot);
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  int32_t removeLink(int32_t from, int32_t to) {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    int32_t result = libwot::removeLink(from, to, wot);
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

  bool isOutdistanced(int32_t member, int32_t d_min, int32_t k_max, double x_percent) {
    libwot::WebOfTrust* wot = libwot::readWoT(filename);
    bool result = libwot::isOutdistanced(member, d_min, k_max, x_percent, wot).isOutdistanced;
    libwot::writeWoT(filename, wot);
    libwot::freeWoT(wot);
    return result;
  }

private:
  std::string filename;
};

std::vector<AbstractWoT*> wots;

/***********************************
* PUBLIC METHODS
************************************/

NAN_METHOD(setVerbose) {
  bool verbose = Nan::To<bool>(info[0]).FromJust();
  libwot::setVerbose(verbose);
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
