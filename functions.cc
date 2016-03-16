#include <node.h>
#include <nan.h>
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

class AbstractWoT {

public:
  virtual void reset();
  virtual void showWoT();
  virtual void showGraph();
  virtual uint32_t getWoTSize();
  virtual uint32_t addNode();
  virtual uint32_t removeNode();
  virtual bool isEnabled(uint32_t nodeIndex);
  virtual bool setEnabled(bool enabled, uint32_t nodeIndex);
  virtual bool existsLink(uint32_t from, uint32_t to);
  virtual uint32_t addLink(uint32_t from, uint32_t to);
  virtual uint32_t removeLink(uint32_t from, uint32_t to);
  virtual bool isOutdistanced(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent);
};

class MemoryWoT : public AbstractWoT {

  public:

  MemoryWoT() {
    wot = new WebOfTrust(3);
  }

  ~MemoryWoT() {
    delete wot;
  }

  void reset() {
    wot->reset();
  }

  void showWoT() {
    wot->showTable();
  }

  void showGraph() {
    wot->showTable(); 
  }

  uint32_t getWoTSize() {
    return wot->getSize();
  }

  uint32_t addNode() {
    wot->addNode();
    return wot->getSize() - 1;
  }

  uint32_t removeNode() {
    wot->removeNode();
    return wot->getSize() - 1;
  }

  bool isEnabled(uint32_t nodeIndex) {
    return wot->getNodeAt(nodeIndex)->isEnabled();
  }

  bool setEnabled(bool enabled, uint32_t nodeIndex) {
    return wot->getNodeAt(nodeIndex)->setEnabled(enabled)->isEnabled();
  }

  bool existsLink(uint32_t from, uint32_t to) {
    return wot->getNodeAt(from)->hasLinkTo(to);
  }

  uint32_t addLink(uint32_t from, uint32_t to) {
    wot->getNodeAt(from)->addLinkTo(to);
    return wot->getNodeAt(to)->getNbLinks();
  }

  uint32_t removeLink(uint32_t from, uint32_t to) {
    wot->getNodeAt(from)->removeLinkTo(to);
    return wot->getNodeAt(to)->getNbLinks();
  }

  bool isOutdistanced(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent) {
    DistanceResult result = wot->computeDistance(member, d_min, k_max, x_percent);
    return result.isOutdistanced;
  }

  private:

    libwot::WebOfTrust* wot;
};



class FileWoT : public AbstractWoT {

  public:

    FileWoT(std::string f) : filename(f) {
      WebOfTrust *wot = WebOfTrust::readFromDisk(f);
      if (wot == NULL) {
        wot = new WebOfTrust(3);		
        wot->writeToDisk(f);
      }

      delete wot;
    }

    ~FileWoT() {
    }

    void reset() {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      wot->reset();
      wot->writeToDisk(filename);
      delete wot;
    }

    void showWoT() {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      wot->showTable();
      delete wot;
    }

    void showGraph() {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      wot->showTable(); 
      delete wot;
    }

    uint32_t getWoTSize() {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      uint32_t size = wot->getSize();
      delete wot;
      return size;
    }

    uint32_t addNode() {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      wot->addNode();
      uint32_t size = wot->getSize() - 1;
      wot->writeToDisk(filename);
      delete wot;
      return size;
    }

    uint32_t removeNode() {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      wot->removeNode();
      uint32_t size = wot->getSize() - 1;
      wot->writeToDisk(filename);
      delete wot;
      return size;
    }

    bool isEnabled(uint32_t nodeIndex) {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      bool enabled = wot->getNodeAt(nodeIndex)->isEnabled();
      delete wot;
      return enabled;
    }

    bool setEnabled(bool enabled, uint32_t nodeIndex) {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      Node* node = wot->getNodeAt(nodeIndex);
      node->setEnabled(enabled);
      bool isEnabled = node->isEnabled();
      wot->writeToDisk(filename);
      delete wot;
      return isEnabled;
    }

    bool existsLink(uint32_t from, uint32_t to) {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      bool exists = wot->getNodeAt(from)->hasLinkTo(to);
      delete wot;
      return exists;
    }

    uint32_t addLink(uint32_t from, uint32_t to) {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      Node* node = wot->getNodeAt(from);
      node->addLinkTo(to);
      uint32_t nbLinks = wot->getNodeAt(to)->getNbLinks();
      wot->writeToDisk(filename);
      delete wot;
      return nbLinks;
    }

    uint32_t removeLink(uint32_t from, uint32_t to) {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      Node* node = wot->getNodeAt(from);
      node->removeLinkTo(to);
      uint32_t nbLinks = wot->getNodeAt(to)->getNbLinks();
      wot->writeToDisk(filename);
      delete wot;
      return nbLinks;
    }

    bool isOutdistanced(uint32_t member, uint32_t d_min, uint32_t k_max, double x_percent) {
      WebOfTrust* wot = WebOfTrust::readFromDisk(filename);
      DistanceResult result = wot->computeDistance(member, d_min, k_max, x_percent);
      delete wot;
      return result.isOutdistanced;
    }

  private:

    string filename;
};


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
