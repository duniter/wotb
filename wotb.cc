#include <node.h>
#include <nan.h>
#include "functions.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("newFileInstance").ToLocalChecked(), GetFunction(New<FunctionTemplate>(newFileInstance)).ToLocalChecked());
  Set(target, New<String>("newMemoryInstance").ToLocalChecked(), GetFunction(New<FunctionTemplate>(newMemoryInstance)).ToLocalChecked());
  Set(target, New<String>("setVerbose").ToLocalChecked(), GetFunction(New<FunctionTemplate>(setVerbose)).ToLocalChecked());
  Set(target, New<String>("showWoT").ToLocalChecked(), GetFunction(New<FunctionTemplate>(showWoT)).ToLocalChecked());
  Set(target, New<String>("showGraph").ToLocalChecked(), GetFunction(New<FunctionTemplate>(showGraph)).ToLocalChecked());
  Set(target, New<String>("resetWoT").ToLocalChecked(), GetFunction(New<FunctionTemplate>(resetWoT)).ToLocalChecked());
  Set(target, New<String>("getWoTSize").ToLocalChecked(), GetFunction(New<FunctionTemplate>(getWoTSize)).ToLocalChecked());
  Set(target, New<String>("addNode").ToLocalChecked(), GetFunction(New<FunctionTemplate>(addNode)).ToLocalChecked());
  Set(target, New<String>("removeNode").ToLocalChecked(), GetFunction(New<FunctionTemplate>(removeNode)).ToLocalChecked());
  Set(target, New<String>("setMaxCert").ToLocalChecked(), GetFunction(New<FunctionTemplate>(setMaxCert)).ToLocalChecked());
  Set(target, New<String>("getMaxCert").ToLocalChecked(), GetFunction(New<FunctionTemplate>(getMaxCert)).ToLocalChecked());
  Set(target, New<String>("isEnabled").ToLocalChecked(), GetFunction(New<FunctionTemplate>(isEnabled)).ToLocalChecked());
  Set(target, New<String>("setEnabled").ToLocalChecked(), GetFunction(New<FunctionTemplate>(setEnabled)).ToLocalChecked());
  Set(target, New<String>("existsLink").ToLocalChecked(), GetFunction(New<FunctionTemplate>(existsLink)).ToLocalChecked());
  Set(target, New<String>("addLink").ToLocalChecked(), GetFunction(New<FunctionTemplate>(addLink)).ToLocalChecked());
  Set(target, New<String>("removeLink").ToLocalChecked(), GetFunction(New<FunctionTemplate>(removeLink)).ToLocalChecked());
  Set(target, New<String>("isOutdistanced").ToLocalChecked(), GetFunction(New<FunctionTemplate>(isOutdistanced)).ToLocalChecked());
}

NODE_MODULE(nacl, InitAll)
