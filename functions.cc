#include <node.h>
#include <nan.h>
#include "wot.h"
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

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

//string getFileName(v8::Value value) {
//  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(value).ToLocalChecked();
//  v8::String::Utf8Value fileParam(fileJSString);
//  return string(*fileParam);
//}

NAN_METHOD(verify) {

  int32_t nbMembers = Nan::To<int32_t>(info[0]).FromJust();
  int32_t maxCertStock = Nan::To<int32_t>(info[1]).FromJust();
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[2]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);

  string file = string(*fileParam);

  libwot::WebOfTrust* wot = libwot::createRandomWoT(nbMembers, maxCertStock);

  libwot::writeWoT(file, wot);

  info.GetReturnValue().Set(New<Number>(wot->nbMembers));
}

NAN_METHOD(getWoTSize) {
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[0]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  int32_t wotSize = libwot::getWoTSize(file);
  info.GetReturnValue().Set(New<Number>(wotSize));
}


NAN_METHOD(addNode) {
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[0]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  int32_t wotSize = libwot::addNode(file);
  info.GetReturnValue().Set(New<Number>(wotSize));
}
