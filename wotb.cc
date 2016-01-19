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
  Set(target, New<String>("verify").ToLocalChecked(), GetFunction(New<FunctionTemplate>(verify)).ToLocalChecked());
  Set(target, New<String>("getWoTSize").ToLocalChecked(), GetFunction(New<FunctionTemplate>(getWoTSize)).ToLocalChecked());
  Set(target, New<String>("addNode").ToLocalChecked(), GetFunction(New<FunctionTemplate>(addNode)).ToLocalChecked());
  Set(target, New<String>("isEnabled").ToLocalChecked(), GetFunction(New<FunctionTemplate>(isEnabled)).ToLocalChecked());
  Set(target, New<String>("setEnabled").ToLocalChecked(), GetFunction(New<FunctionTemplate>(setEnabled)).ToLocalChecked());
}

NODE_MODULE(nacl, InitAll)
