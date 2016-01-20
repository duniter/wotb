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

string getFileName(v8::Local<v8::Value> value) {
  v8::Local<v8::String> fileJSString = Nan::To<v8::String>(value).ToLocalChecked();
  v8::String::Utf8Value fileParam(fileJSString);
  return string(*fileParam);
}

NAN_METHOD(getWoTSize) {
  string file = getFileName(info[0]);
  info.GetReturnValue().Set(New<Number>(libwot::getWoTSize(file)));
}

NAN_METHOD(addNode) {
  string file = getFileName(info[0]);
  info.GetReturnValue().Set(New<Number>(libwot::addNode(file)));
}

NAN_METHOD(isEnabled) {
  int32_t member = Nan::To<int32_t>(info[0]).FromJust();
  string file = getFileName(info[1]);
  info.GetReturnValue().Set(New<Boolean>(libwot::isEnabled(member, file)));
}

NAN_METHOD(setEnabled) {
  bool enabled = Nan::To<bool>(info[0]).FromJust();
  int32_t member = Nan::To<int32_t>(info[1]).FromJust();
  string file = getFileName(info[2]);
  info.GetReturnValue().Set(New<Boolean>(libwot::setEnabled(enabled, member, file)));
}

NAN_METHOD(existsLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  string file = getFileName(info[2]);
  info.GetReturnValue().Set(New<Boolean>(libwot::existsLink(from, to, file)));
}

NAN_METHOD(addLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  string file = getFileName(info[2]);
  info.GetReturnValue().Set(New<Number>(libwot::addLink(from, to, file)));
}

NAN_METHOD(removeLink) {
  int32_t from = Nan::To<int32_t>(info[0]).FromJust();
  int32_t to = Nan::To<int32_t>(info[1]).FromJust();
  string file = getFileName(info[2]);
  info.GetReturnValue().Set(New<Number>(libwot::removeLink(from, to, file)));
}

NAN_METHOD(isOutdistanced) {
  int32_t member = Nan::To<int32_t>(info[0]).FromJust();
  int32_t d_min = Nan::To<int32_t>(info[1]).FromJust();
  int32_t k_max = Nan::To<int32_t>(info[2]).FromJust();
  double x_percent = Nan::To<double>(info[3]).FromJust();
  string file = getFileName(info[4]);
  info.GetReturnValue().Set(New<Boolean>((libwot::isOutdistanced(member, d_min, k_max, x_percent, file)).isOutdistanced));
}
