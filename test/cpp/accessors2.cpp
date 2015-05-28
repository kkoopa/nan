/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2015 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#include <nan.h>
#include <cstring>

using namespace Nan;  // NOLINT(build/namespaces)

class SetterGetter : public NanObjectWrap {
 public:
  static void Init (v8::Handle<v8::Object> target);
  static v8::Handle<v8::Value> NewInstance ();
  static NAN_METHOD(New);
  static NAN_METHOD(Log);
  static NAN_GETTER(GetProp1);
  static NAN_GETTER(GetProp2);
  static NAN_SETTER(SetProp2);

  SetterGetter();

  char log[1024];
  char prop1[256];
  char prop2[256];
};

static NanPersistent<v8::FunctionTemplate> settergetter_constructor;

NAN_METHOD(CreateNew) {
  info.GetReturnValue().Set(SetterGetter::NewInstance());
}

SetterGetter::SetterGetter() {
  log[0] = '\0';
  strncpy(prop1, "this is property 1", sizeof (prop1) - 1);
  prop1[sizeof (prop1) - 1] = '\0';
  prop2[0] = '\0';
}

void SetterGetter::Init(v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl =
    NanNew<v8::FunctionTemplate>(SetterGetter::New);
  settergetter_constructor.Reset(tpl);
  tpl->SetClassName(NanNew<v8::String>("SetterGetter").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NanSetPrototypeMethod(tpl, "log", SetterGetter::Log);
  v8::Local<v8::Function> createnew =
    NanNew<v8::FunctionTemplate>(CreateNew)->GetFunction();
  NanSet(target, NanNew<v8::String>("create").ToLocalChecked(), createnew);
}

v8::Handle<v8::Value> SetterGetter::NewInstance () {
  NanEscapableScope scope;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanNew(settergetter_constructor);
  v8::Local<v8::Object> instance =
    constructorHandle->GetFunction()->NewInstance(0, NULL);
  NanSetAccessor(
      instance
    , NanNew("prop1").ToLocalChecked()
    , SetterGetter::GetProp1);
  NanSetAccessor(
      instance
    , NanNew<v8::String>("prop2").ToLocalChecked()
    , SetterGetter::GetProp2
    , SetterGetter::SetProp2
  );
  return scope.Escape(instance);
}

NAN_METHOD(SetterGetter::New) {
  SetterGetter* settergetter = new SetterGetter();
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , "New()\n"
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
  settergetter->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_GETTER(SetterGetter::GetProp1) {
  SetterGetter* settergetter =
    NanObjectWrap::Unwrap<SetterGetter>(info.This());
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , "Prop1:GETTER("
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , settergetter->prop1
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , ")\n"
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));

  info.GetReturnValue().Set(NanNew(settergetter->prop1).ToLocalChecked());
}

NAN_GETTER(SetterGetter::GetProp2) {
  SetterGetter* settergetter =
    NanObjectWrap::Unwrap<SetterGetter>(info.This());
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , "Prop2:GETTER("
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , settergetter->prop2
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , ")\n"
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));

  info.GetReturnValue().Set(NanNew(settergetter->prop2).ToLocalChecked());
}

NAN_SETTER(SetterGetter::SetProp2) {
  SetterGetter* settergetter =
      NanObjectWrap::Unwrap<SetterGetter>(info.This());
  strncpy(
      settergetter->prop2
    , *v8::String::Utf8Value(value)
    , sizeof (settergetter->prop2));
  settergetter->prop2[sizeof (settergetter->prop2) - 1] = '\0';
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , "Prop2:SETTER("
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , settergetter->prop2
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
  assert(strlen(settergetter->log) < sizeof (settergetter->log));
  strncat(
      settergetter->log
    , ")\n"
    , sizeof (settergetter->log) - 1 - strlen(settergetter->log));
}

NAN_METHOD(SetterGetter::Log) {
  SetterGetter* settergetter =
    NanObjectWrap::Unwrap<SetterGetter>(info.This());

  info.GetReturnValue().Set(NanNew(settergetter->log).ToLocalChecked());
}

NODE_MODULE(accessors2, SetterGetter::Init)
