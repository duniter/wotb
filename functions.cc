#include <node.h>
#include <nan.h>
#include "wotc/wot.h"
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

NAN_METHOD(isEnabled) {
  int32_t member = Nan::To<int32_t>(info[0]).FromJust();
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[1]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  bool enabled = libwot::isEnabled(member, file);
  info.GetReturnValue().Set(New<Boolean>(enabled));
}

NAN_METHOD(setEnabled) {
  bool enabled = Nan::To<bool>(info[0]).FromJust();
  int32_t member = Nan::To<int32_t>(info[1]).FromJust();
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[2]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  enabled = libwot::setEnabled(enabled, member, file);
  info.GetReturnValue().Set(New<Boolean>(enabled));
}

NAN_METHOD(existsLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[2]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  info.GetReturnValue().Set(New<Boolean>(libwot::existsLink(from, to, file)));
}

NAN_METHOD(addLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[2]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  info.GetReturnValue().Set(New<Number>(libwot::addLink(from, to, file)));
}

NAN_METHOD(removeLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[2]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  info.GetReturnValue().Set(New<Number>(libwot::removeLink(from, to, file)));
}

NAN_METHOD(isOutdistanced) {
  int32_t member = Nan::To<int32_t>(info[0]).FromJust();
  int32_t d_min = Nan::To<int32_t>(info[1]).FromJust();
  int32_t k_max = Nan::To<int32_t>(info[2]).FromJust();
  double x_percent = Nan::To<double>(info[3]).FromJust();
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(info[4]).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  string file = string(*fileParam);
  info.GetReturnValue().Set(New<Boolean>((libwot::isOutdistanced(member, d_min, k_max, x_percent, file)).isOutdistanced));
}
