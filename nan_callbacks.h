/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2015 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#ifndef NAN_CALLBACKS_H_
#define NAN_CALLBACKS_H_

namespace Nan { namespace imp {
typedef v8::Handle<v8::AccessorSignature> Sig;
}  // end of namespace imp
}  // end of namespace Nan

#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
template<typename T>
class NanFunctionCallbackInfo : public v8::FunctionCallbackInfo<T> {
 public:
  NanFunctionCallbackInfo(  // NOLINT(runtime/explicit)
      const v8::FunctionCallbackInfo<T> &arg) :
          v8::FunctionCallbackInfo<T>(arg) {}
};

template<typename T>
class NanPropertyCallbackInfo : public v8::PropertyCallbackInfo<T> {
 public:
  NanPropertyCallbackInfo(  // NOLINT(runtime/explicit)
      const v8::PropertyCallbackInfo<T> &info) :
          v8::PropertyCallbackInfo<T>(info) {}
};

template<typename T>
class NanReturnValue : public v8::ReturnValue<T> {
 public:
  NanReturnValue(const v8::ReturnValue<T> val) :  // NOLINT(runtime/explicit)
      v8::ReturnValue<T>(val) {}
};

typedef void(*NanFunctionCallback)(const v8::FunctionCallbackInfo<v8::Value>&);
typedef void(*NanGetterCallback)
    (v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>&);
typedef void(*NanSetterCallback)(
    v8::Local<v8::String>
  , v8::Local<v8::Value>
  , const v8::PropertyCallbackInfo<void>&);
typedef void(*NanPropertyGetterCallback)(
    v8::Local<v8::String>,
    const v8::PropertyCallbackInfo<v8::Value>&);
typedef void(*NanPropertySetterCallback)(
    v8::Local<v8::String>,
    v8::Local<v8::Value>,
    const v8::PropertyCallbackInfo<v8::Value>&);
typedef void(*NanPropertyEnumeratorCallback)
    (const v8::PropertyCallbackInfo<v8::Array>&);
typedef void(*NanPropertyDeleterCallback)(
    v8::Local<v8::String>,
    const v8::PropertyCallbackInfo<v8::Boolean>&);
typedef void(*NanPropertyQueryCallback)(
    v8::Local<v8::String>,
    const v8::PropertyCallbackInfo<v8::Integer>&);
typedef void(*NanIndexGetterCallback)(
    uint32_t,
    const v8::PropertyCallbackInfo<v8::Value>&);
typedef void(*NanIndexSetterCallback)(
    uint32_t,
    v8::Local<v8::Value>,
    const v8::PropertyCallbackInfo<v8::Value>&);
typedef void(*NanIndexEnumeratorCallback)
    (const v8::PropertyCallbackInfo<v8::Array>&);
typedef void(*NanIndexDeleterCallback)(
    uint32_t,
    const v8::PropertyCallbackInfo<v8::Boolean>&);
typedef void(*NanIndexQueryCallback)(
    uint32_t,
    const v8::PropertyCallbackInfo<v8::Integer>&);

inline void NanSetAccessor(
    v8::Handle<v8::ObjectTemplate> tpl
  , v8::Handle<v8::String> name
  , NanGetterCallback getter
  , NanSetterCallback setter = 0
  , v8::Handle<v8::Value> data = v8::Handle<v8::Value>()
  , v8::AccessControl settings = v8::DEFAULT
  , v8::PropertyAttribute attribute = v8::None
  , Nan::imp::Sig signature = Nan::imp::Sig()) {
    tpl->SetAccessor(
        name, getter, setter, data, settings, attribute, signature);
}

inline void NanSetNamedPropertyHandler(
    v8::Handle<v8::ObjectTemplate> tpl
  , NanPropertyGetterCallback getter
  , NanPropertySetterCallback setter = 0
  , NanPropertyQueryCallback query = 0
  , NanPropertyDeleterCallback deleter = 0
  , NanPropertyEnumeratorCallback enumerator = 0
  , v8::Handle<v8::Value> data = v8::Handle<v8::Value>()) {
  tpl->SetNamedPropertyHandler(
      getter, setter, query, deleter, enumerator, data);
}

inline void NanSetIndexedPropertyHandler(
    v8::Handle<v8::ObjectTemplate> tpl
  , NanIndexGetterCallback getter
  , NanIndexSetterCallback setter = 0
  , NanIndexQueryCallback query = 0
  , NanIndexDeleterCallback deleter = 0
  , NanIndexEnumeratorCallback enumerator = 0
  , v8::Handle<v8::Value> data = v8::Handle<v8::Value>()) {
  tpl->SetIndexedPropertyHandler(
      getter, setter, query, deleter, enumerator, data);
}

#else

#if defined(_MSC_VER)
# pragma warning( push )
# pragma warning( disable : 4530 )
# include <map>
# pragma warning( pop )
#else
# include <map>
#endif


namespace Nan { namespace imp {
template<typename T> class NanReturnValueImp;
}  // end of namespace imp
}  // end of namespace Nan

template<typename T>
class NanReturnValue {
  v8::Isolate *isolate_;
  v8::Persistent<T> *value_;
  friend class Nan::imp::NanReturnValueImp<T>;

 public:
  template <class S>
  explicit inline NanReturnValue(v8::Isolate *isolate, v8::Persistent<S> *p) :
      isolate_(isolate), value_(p) {}
  template <class S>
  explicit inline NanReturnValue(const NanReturnValue<S>& that)
      : isolate_(that.isolate_), value_(that.value_) {
    TYPE_CHECK(T, S);
  }

  // Handle setters
  template <typename S> inline void Set(v8::Handle<S> handle) {
    TYPE_CHECK(T, S);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(handle);
  }

  // Fast primitive setters
  inline void Set(bool value) {
    TYPE_CHECK(T, v8::Boolean);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(v8::Boolean::New(value));
  }

  inline void Set(double i) {
    TYPE_CHECK(T, v8::Number);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(v8::Number::New(i));
  }

  inline void Set(int32_t i) {
    TYPE_CHECK(T, v8::Integer);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(v8::Int32::New(i));
  }

  inline void Set(uint32_t i) {
    TYPE_CHECK(T, v8::Integer);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(v8::Uint32::New(i));
  }

  // Fast JS primitive setters
  inline void SetNull() {
    TYPE_CHECK(T, v8::Primitive);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(v8::Null());
  }

  inline void SetUndefined() {
    TYPE_CHECK(T, v8::Primitive);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(v8::Undefined());
  }

  inline void SetEmptyString() {
    TYPE_CHECK(T, v8::String);
    value_->Dispose();
    *value_ = v8::Persistent<T>::New(v8::String::Empty());
  }

  // Convenience getter for isolate
  inline v8::Isolate *GetIsolate() const {
    return isolate_;
  }

  // Pointer setter: Uncompilable to prevent inadvertent misuse.
  template<typename S>
  inline void Set(S *whatever) { TYPE_CHECK(S*, v8::Primitive); }
};

template<typename T>
class NanFunctionCallbackInfo {
  const v8::Arguments &args_;
  v8::Local<v8::Value> data_;
  NanReturnValue<T> return_value_;
  v8::Persistent<T> retval_;

 public:
  explicit inline NanFunctionCallbackInfo(
      const v8::Arguments &args
    , v8::Local<v8::Value> data) :
          args_(args)
        , data_(data)
        , return_value_(args.GetIsolate(), &retval_)
        , retval_(v8::Persistent<T>::New(v8::Undefined())) {}

  inline NanReturnValue<T> GetReturnValue() const {
    return NanReturnValue<T>(return_value_);
  }

  inline v8::Local<v8::Function> Callee() const { return args_.Callee(); }
  inline v8::Local<v8::Value> Data() const { return data_; }
  inline v8::Local<v8::Object> Holder() const { return args_.Holder(); }
  inline bool IsConstructCall() const { return args_.IsConstructCall(); }
  inline int Length() const { return args_.Length(); }
  inline v8::Local<v8::Value> operator[](int i) const { return args_[i]; }
  inline v8::Local<v8::Object> This() const { return args_.This(); }
  inline v8::Isolate *GetIsolate() const { return args_.GetIsolate(); }


 protected:
  static const int kHolderIndex = 0;
  static const int kIsolateIndex = 1;
  static const int kReturnValueDefaultValueIndex = 2;
  static const int kReturnValueIndex = 3;
  static const int kDataIndex = 4;
  static const int kCalleeIndex = 5;
  static const int kContextSaveIndex = 6;
  static const int kArgsLength = 7;
};

template<typename T>
class NanPropertyCallbackInfoBase {
  const v8::AccessorInfo &info_;
  const v8::Local<v8::Value> &data_;

 public:
  explicit inline NanPropertyCallbackInfoBase(
      const v8::AccessorInfo &info
    , const v8::Local<v8::Value> &data) :
          info_(info)
        , data_(data) {}

  inline v8::Isolate* GetIsolate() const { return info_.GetIsolate(); }
  inline v8::Local<v8::Value> Data() const { return data_; }
  inline v8::Local<v8::Object> This() const { return info_.This(); }
  inline v8::Local<v8::Object> Holder() const { return info_.Holder(); }

 protected:
  static const int kHolderIndex = 0;
  static const int kIsolateIndex = 1;
  static const int kReturnValueDefaultValueIndex = 2;
  static const int kReturnValueIndex = 3;
  static const int kDataIndex = 4;
  static const int kThisIndex = 5;
  static const int kArgsLength = 6;
};

template<typename T>
class NanPropertyCallbackInfo : public NanPropertyCallbackInfoBase<T> {
  NanReturnValue<T> return_value_;
  v8::Persistent<T> retval_;

 public:
  explicit inline NanPropertyCallbackInfo(
      const v8::AccessorInfo &info
    , const v8::Local<v8::Value> &data) :
          NanPropertyCallbackInfoBase<T>(info, data)
        , return_value_(info.GetIsolate(), &retval_)
        , retval_(v8::Persistent<T>::New(v8::Undefined())) {}
  inline NanReturnValue<T> GetReturnValue() const { return return_value_; }
};

template<>
class NanPropertyCallbackInfo<v8::Array> :
    public NanPropertyCallbackInfoBase<v8::Array> {
  NanReturnValue<v8::Array> return_value_;
  v8::Persistent<v8::Array> retval_;

 public:
  explicit inline NanPropertyCallbackInfo(
      const v8::AccessorInfo &info
    , const v8::Local<v8::Value> &data) :
          NanPropertyCallbackInfoBase<v8::Array>(info, data)
        , return_value_(info.GetIsolate(), &retval_)
        , retval_(v8::Persistent<v8::Array>::New(v8::Local<v8::Array>())) {}
  inline NanReturnValue<v8::Array> GetReturnValue() const {
    return return_value_;
  }
};

template<>
class NanPropertyCallbackInfo<v8::Boolean> :
    public NanPropertyCallbackInfoBase<v8::Boolean> {
  NanReturnValue<v8::Boolean> return_value_;
  v8::Persistent<v8::Boolean> retval_;

 public:
  explicit inline NanPropertyCallbackInfo(
      const v8::AccessorInfo &info
    , const v8::Local<v8::Value> &data) :
          NanPropertyCallbackInfoBase<v8::Boolean>(info, data)
        , return_value_(info.GetIsolate(), &retval_)
        , retval_(v8::Persistent<v8::Boolean>::New(v8::Local<v8::Boolean>())) {}
  inline NanReturnValue<v8::Boolean> GetReturnValue() const {
    return return_value_;
  }
};

template<>
class NanPropertyCallbackInfo<v8::Integer> :
    public NanPropertyCallbackInfoBase<v8::Integer> {
  NanReturnValue<v8::Integer> return_value_;
  v8::Persistent<v8::Integer> retval_;

 public:
  explicit inline NanPropertyCallbackInfo(
      const v8::AccessorInfo &info
    , const v8::Local<v8::Value> &data) :
          NanPropertyCallbackInfoBase<v8::Integer>(info, data)
        , return_value_(info.GetIsolate(), &retval_)
        , retval_(v8::Persistent<v8::Integer>::New(v8::Local<v8::Integer>())) {}
  inline NanReturnValue<v8::Integer> GetReturnValue() const {
    return return_value_;
  }
};

typedef void(*NanFunctionCallback)(const NanFunctionCallbackInfo<v8::Value>&);
typedef void(*NanGetterCallback)
    (v8::Local<v8::String>, const NanPropertyCallbackInfo<v8::Value>&);
typedef void(*NanSetterCallback)(
    v8::Local<v8::String>,
    v8::Local<v8::Value>,
    const NanPropertyCallbackInfo<void>&);
typedef void(*NanPropertyGetterCallback)(
    v8::Local<v8::String>,
    const NanPropertyCallbackInfo<v8::Value>&);
typedef void(*NanPropertySetterCallback)(
    v8::Local<v8::String>,
    v8::Local<v8::Value>,
    const NanPropertyCallbackInfo<v8::Value>&);
typedef void(*NanPropertyEnumeratorCallback)
    (const NanPropertyCallbackInfo<v8::Array>&);
typedef void(*NanPropertyDeleterCallback)(
    v8::Local<v8::String>,
    const NanPropertyCallbackInfo<v8::Boolean>&);
typedef void(*NanPropertyQueryCallback)(
    v8::Local<v8::String>,
    const NanPropertyCallbackInfo<v8::Integer>&);
typedef void(*NanIndexGetterCallback)(
    uint32_t,
    const NanPropertyCallbackInfo<v8::Value>&);
typedef void(*NanIndexSetterCallback)(
    uint32_t,
    v8::Local<v8::Value>,
    const NanPropertyCallbackInfo<v8::Value>&);
typedef void(*NanIndexEnumeratorCallback)
    (const NanPropertyCallbackInfo<v8::Array>&);
typedef void(*NanIndexDeleterCallback)(
    uint32_t,
    const NanPropertyCallbackInfo<v8::Boolean>&);
typedef void(*NanIndexQueryCallback)(
    uint32_t,
    const NanPropertyCallbackInfo<v8::Integer>&);

namespace Nan { namespace imp {
template<typename T>
class NanReturnValueImp : public NanReturnValue<T> {
 public:
  explicit NanReturnValueImp(NanReturnValue<T> that) :
      NanReturnValue<T>(that) {}
  NAN_INLINE v8::Handle<T> Value() {
      return *NanReturnValue<T>::value_;
  }
};

#define X(NAME)                                                                \
struct NAME ## Wrapper {                                                       \
  explicit NAME ## Wrapper(Nan ## NAME ## Callback callback_) :                \
      callback(callback_) {}                                                   \
  Nan ## NAME ## Callback callback;                                            \
};

X(Function)
X(Getter)
X(Setter)
X(PropertyGetter)
X(PropertySetter)
X(PropertyEnumerator)
X(PropertyDeleter)
X(PropertyQuery)
X(IndexGetter)
X(IndexSetter)
X(IndexEnumerator)
X(IndexDeleter)
X(IndexQuery)

#undef X

static const int kDataIndex =                    0;

static const int kFunctionIndex =                1;
static const int kFunctionFieldCount =           2;

static const int kGetterIndex =                  1;
static const int kSetterIndex =                  2;
static const int kAccessorFieldCount =           3;

static const int kPropertyGetterIndex =          1;
static const int kPropertySetterIndex =          2;
static const int kPropertyEnumeratorIndex =      3;
static const int kPropertyDeleterIndex =         4;
static const int kPropertyQueryIndex =           5;
static const int kPropertyFieldCount =           6;

static const int kIndexPropertyGetterIndex =     1;
static const int kIndexPropertySetterIndex =     2;
static const int kIndexPropertyEnumeratorIndex = 3;
static const int kIndexPropertyDeleterIndex =    4;
static const int kIndexPropertyQueryIndex =      5;
static const int kIndexPropertyFieldCount =      6;

static
v8::Handle<v8::Value> FunctionCallbackWrapper(const v8::Arguments &args) {
  v8::Local<v8::Object> obj = args.Data().As<v8::Object>();
  FunctionWrapper *wrapper = static_cast<FunctionWrapper*>(
      obj->GetPointerFromInternalField(kFunctionIndex));
  NanFunctionCallbackInfo<v8::Value>
      cbinfo(args, obj->GetInternalField(kDataIndex));
  wrapper->callback(cbinfo);
  return NanReturnValueImp<v8::Value>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Value> (*NativeFunction)(const v8::Arguments &);

static
v8::Handle<v8::Value> GetterCallbackWrapper(
    v8::Local<v8::String> property, const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Value>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  GetterWrapper *wrapper = static_cast<GetterWrapper*>(
      obj->GetPointerFromInternalField(kGetterIndex));
  wrapper->callback(property, cbinfo);
  return NanReturnValueImp<v8::Value>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Value> (*NativeGetter)
    (v8::Local<v8::String>, const v8::AccessorInfo &);

static
void SetterCallbackWrapper(
    v8::Local<v8::String> property
  , v8::Local<v8::Value> value
  , const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<void>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  SetterWrapper *wrapper = static_cast<SetterWrapper*>(
      obj->GetPointerFromInternalField(kSetterIndex));
  wrapper->callback(property, value, cbinfo);
}

typedef void (*NativeSetter)
    (v8::Local<v8::String>, v8::Local<v8::Value>, const v8::AccessorInfo &);

static
v8::Handle<v8::Value> PropertyGetterCallbackWrapper(
    v8::Local<v8::String> property, const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Value>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  PropertyGetterWrapper *wrapper = static_cast<PropertyGetterWrapper*>(
      obj->GetPointerFromInternalField(kPropertyGetterIndex));
  wrapper->callback(property, cbinfo);
  return NanReturnValueImp<v8::Value>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Value> (*NativePropertyGetter)
    (v8::Local<v8::String>, const v8::AccessorInfo &);

static
v8::Handle<v8::Value> PropertySetterCallbackWrapper(
    v8::Local<v8::String> property
  , v8::Local<v8::Value> value
  , const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Value>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  PropertySetterWrapper *wrapper = static_cast<PropertySetterWrapper*>(
      obj->GetPointerFromInternalField(kPropertySetterIndex));
  wrapper->callback(property, value, cbinfo);
  return NanReturnValueImp<v8::Value>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Value> (*NativePropertySetter)
    (v8::Local<v8::String>, v8::Local<v8::Value>, const v8::AccessorInfo &);

static
v8::Handle<v8::Array> PropertyEnumeratorCallbackWrapper(
    const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Array>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  PropertyEnumeratorWrapper *wrapper = static_cast<PropertyEnumeratorWrapper*>(
      obj->GetPointerFromInternalField(kPropertyEnumeratorIndex));
  wrapper->callback(cbinfo);
  return NanReturnValueImp<v8::Array>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Array> (*NativePropertyEnumerator)
    (const v8::AccessorInfo &);

static
v8::Handle<v8::Boolean> PropertyDeleterCallbackWrapper(
    v8::Local<v8::String> property
  , const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Boolean>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  PropertyDeleterWrapper *wrapper = static_cast<PropertyDeleterWrapper*>(
      obj->GetPointerFromInternalField(kPropertyDeleterIndex));
  wrapper->callback(property, cbinfo);
  return NanReturnValueImp<v8::Boolean>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Boolean> (NativePropertyDeleter)
    (v8::Local<v8::String>, const v8::AccessorInfo &);

static
v8::Handle<v8::Integer> PropertyQueryCallbackWrapper(
    v8::Local<v8::String> property, const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Integer>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  PropertyQueryWrapper *wrapper = static_cast<PropertyQueryWrapper*>(
      obj->GetPointerFromInternalField(kPropertyQueryIndex));
  wrapper->callback(property, cbinfo);
  return NanReturnValueImp<v8::Integer>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Integer> (*NativePropertyQuery)
    (v8::Local<v8::String>, const v8::AccessorInfo &);

static
v8::Handle<v8::Value> IndexGetterCallbackWrapper(
    uint32_t index, const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Value>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  IndexGetterWrapper *wrapper = static_cast<IndexGetterWrapper*>(
      obj->GetPointerFromInternalField(kIndexPropertyGetterIndex));
  wrapper->callback(index, cbinfo);
  return NanReturnValueImp<v8::Value>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Value> (*NativeIndexGetter)
    (uint32_t, const v8::AccessorInfo &);

static
v8::Handle<v8::Value> IndexSetterCallbackWrapper(
    uint32_t index
  , v8::Local<v8::Value> value
  , const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Value>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  IndexSetterWrapper *wrapper = static_cast<IndexSetterWrapper*>(
      obj->GetPointerFromInternalField(kIndexPropertySetterIndex));
  wrapper->callback(index, value, cbinfo);
  return NanReturnValueImp<v8::Value>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Value> (*NativeIndexSetter)
    (uint32_t, v8::Local<v8::Value>, const v8::AccessorInfo &);

static
v8::Handle<v8::Array> IndexEnumeratorCallbackWrapper(
    const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Array>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  IndexEnumeratorWrapper *wrapper = static_cast<IndexEnumeratorWrapper*>(
      obj->GetPointerFromInternalField(kIndexPropertyEnumeratorIndex));
  wrapper->callback(cbinfo);
  return NanReturnValueImp<v8::Array>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Array> (*NativeIndexEnumerator)
    (const v8::AccessorInfo &);

static
v8::Handle<v8::Boolean> IndexDeleterCallbackWrapper(
    uint32_t index, const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Boolean>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  IndexDeleterWrapper *wrapper = static_cast<IndexDeleterWrapper*>(
      obj->GetPointerFromInternalField(kIndexPropertyDeleterIndex));
  wrapper->callback(index, cbinfo);
  return NanReturnValueImp<v8::Boolean>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Boolean> (*NativeIndexDeleter)
    (uint32_t, const v8::AccessorInfo &);

static
v8::Handle<v8::Integer> IndexQueryCallbackWrapper(
    uint32_t index, const v8::AccessorInfo &info) {
  v8::Local<v8::Object> obj = info.Data().As<v8::Object>();
  NanPropertyCallbackInfo<v8::Integer>
      cbinfo(info, obj->GetInternalField(kDataIndex));
  IndexQueryWrapper *wrapper = static_cast<IndexQueryWrapper*>(
      obj->GetPointerFromInternalField(kIndexPropertyQueryIndex));
  wrapper->callback(index, cbinfo);
  return NanReturnValueImp<v8::Integer>(cbinfo.GetReturnValue()).Value();
}

typedef v8::Handle<v8::Integer> (*NativeIndexQuery)
    (uint32_t, const v8::AccessorInfo &);

template<typename T, typename P>
P *GetWrapper(T needle) {
  static std::map<T, P*> haystack;
  typename std::map<T, P*>::iterator it =
      haystack.find(needle);

  if (it == haystack.end()) {
    return haystack.insert(it, std::make_pair(needle, new P(needle)))->second;
  } else {
    return it->second;
  }
}
}  // end of namespace imp
}  // end of namespace Nan

inline void NanSetAccessor(
    v8::Handle<v8::ObjectTemplate> tpl
  , v8::Handle<v8::String> name
  , NanGetterCallback getter
  , NanSetterCallback setter = 0
  , v8::Handle<v8::Value> data = v8::Handle<v8::Value>()
  , v8::AccessControl settings = v8::DEFAULT
  , v8::PropertyAttribute attribute = v8::None
  , Nan::imp::Sig signature = Nan::imp::Sig()) {
  v8::HandleScope scope;

  v8::Local<v8::ObjectTemplate> otpl = v8::ObjectTemplate::New();
  otpl->SetInternalFieldCount(Nan::imp::kAccessorFieldCount);
  v8::Local<v8::Object> obj = otpl->NewInstance();
  obj->SetPointerInInternalField(
      Nan::imp::kGetterIndex
    , Nan::imp::GetWrapper<NanGetterCallback, Nan::imp::GetterWrapper>(getter));
  v8::Local<v8::Value> val = v8::Local<v8::Value>::New(data);

  if (setter != 0) {
    obj->SetPointerInInternalField(
        Nan::imp::kSetterIndex
      , Nan::imp::GetWrapper<NanSetterCallback,
            Nan::imp::SetterWrapper>(setter));
    tpl->SetAccessor(
        name
      , Nan::imp::GetterCallbackWrapper
      , Nan::imp::SetterCallbackWrapper
      , obj
      , settings
      , attribute
      , signature);
  } else {
    tpl->SetAccessor(
        name
      , Nan::imp::GetterCallbackWrapper
      , 0
      , obj
      , settings
      , attribute
      , signature);
  }

  if (!val.IsEmpty()) {
    obj->SetInternalField(Nan::imp::kDataIndex, val);
  }
}

inline void NanSetNamedPropertyHandler(
    v8::Handle<v8::ObjectTemplate> tpl
  , NanPropertyGetterCallback getter
  , NanPropertySetterCallback setter = 0
  , NanPropertyQueryCallback query = 0
  , NanPropertyDeleterCallback deleter = 0
  , NanPropertyEnumeratorCallback enumerator = 0
  , v8::Handle<v8::Value> data = v8::Handle<v8::Value>()) {
  v8::HandleScope scope;

  Nan::imp::NativePropertyGetter getter_ =
      Nan::imp::PropertyGetterCallbackWrapper;
  Nan::imp::NativePropertySetter setter_ =
      setter ? Nan::imp::PropertySetterCallbackWrapper : 0;
  Nan::imp::NativePropertyQuery query_ =
      query ? Nan::imp::PropertyQueryCallbackWrapper : 0;
  Nan::imp::NativePropertyDeleter *deleter_ =
      deleter ? Nan::imp::PropertyDeleterCallbackWrapper : 0;
  Nan::imp::NativePropertyEnumerator enumerator_ =
      enumerator ? Nan::imp::PropertyEnumeratorCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = v8::ObjectTemplate::New();
  otpl->SetInternalFieldCount(Nan::imp::kPropertyFieldCount);
  v8::Local<v8::Object> obj = otpl->NewInstance();
  obj->SetPointerInInternalField(
      Nan::imp::kPropertyGetterIndex
    , Nan::imp::GetWrapper<NanPropertyGetterCallback,
          Nan::imp::PropertyGetterWrapper>(getter));
  v8::Local<v8::Value> val = v8::Local<v8::Value>::New(data);

  if (setter) {
    obj->SetPointerInInternalField(
        Nan::imp::kPropertySetterIndex
      , Nan::imp::GetWrapper<NanPropertySetterCallback,
            Nan::imp::PropertySetterWrapper>(setter));
  }

  if (query) {
    obj->SetPointerInInternalField(
        Nan::imp::kPropertyQueryIndex
      , Nan::imp::GetWrapper<NanPropertyQueryCallback,
            Nan::imp::PropertyQueryWrapper>(query));
  }

  if (deleter) {
    obj->SetPointerInInternalField(
        Nan::imp::kPropertyDeleterIndex
      , Nan::imp::GetWrapper<NanPropertyDeleterCallback,
            Nan::imp::PropertyDeleterWrapper>(deleter));
  }

  if (enumerator) {
    obj->SetPointerInInternalField(
        Nan::imp::kPropertyEnumeratorIndex
      , Nan::imp::GetWrapper<NanPropertyEnumeratorCallback,
            Nan::imp::PropertyEnumeratorWrapper>(enumerator));
  }

  if (!val.IsEmpty()) {
    obj->SetInternalField(Nan::imp::kDataIndex, val);
  }

  tpl->SetNamedPropertyHandler(
      getter_
    , setter_
    , query_
    , deleter_
    , enumerator_
    , obj);
}

inline void NanSetIndexedPropertyHandler(
    v8::Handle<v8::ObjectTemplate> tpl
  , NanIndexGetterCallback getter
  , NanIndexSetterCallback setter = 0
  , NanIndexQueryCallback query = 0
  , NanIndexDeleterCallback deleter = 0
  , NanIndexEnumeratorCallback enumerator = 0
  , v8::Handle<v8::Value> data = v8::Handle<v8::Value>()) {
  v8::HandleScope scope;

  Nan::imp::NativeIndexGetter getter_ =
      Nan::imp::IndexGetterCallbackWrapper;
  Nan::imp::NativeIndexSetter setter_ =
      setter ? Nan::imp::IndexSetterCallbackWrapper : 0;
  Nan::imp::NativeIndexQuery query_ =
      query ? Nan::imp::IndexQueryCallbackWrapper : 0;
  Nan::imp::NativeIndexDeleter deleter_ =
      deleter ? Nan::imp::IndexDeleterCallbackWrapper : 0;
  Nan::imp::NativeIndexEnumerator enumerator_ =
      enumerator ? Nan::imp::IndexEnumeratorCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = v8::ObjectTemplate::New();
  otpl->SetInternalFieldCount(Nan::imp::kIndexPropertyFieldCount);
  v8::Local<v8::Object> obj = otpl->NewInstance();
  obj->SetPointerInInternalField(
      Nan::imp::kIndexPropertyGetterIndex
    , Nan::imp::GetWrapper<NanIndexGetterCallback,
          Nan::imp::IndexGetterWrapper>(getter));
  v8::Local<v8::Value> val = v8::Local<v8::Value>::New(data);

  if (setter) {
    obj->SetPointerInInternalField(
        Nan::imp::kIndexPropertySetterIndex
      , Nan::imp::GetWrapper<NanIndexSetterCallback,
            Nan::imp::IndexSetterWrapper>(setter));
  }

  if (query) {
    obj->SetPointerInInternalField(
        Nan::imp::kIndexPropertyQueryIndex
      , Nan::imp::GetWrapper<NanIndexQueryCallback,
            Nan::imp::IndexQueryWrapper>(query));
  }

  if (deleter) {
    obj->SetPointerInInternalField(
        Nan::imp::kIndexPropertyDeleterIndex
      , Nan::imp::GetWrapper<NanIndexDeleterCallback,
            Nan::imp::IndexDeleterWrapper>(deleter));
  }

  if (enumerator) {
    obj->SetPointerInInternalField(
        Nan::imp::kIndexPropertyEnumeratorIndex
      , Nan::imp::GetWrapper<NanIndexEnumeratorCallback,
            Nan::imp::IndexEnumeratorWrapper>(enumerator));
  }

  if (!val.IsEmpty()) {
    obj->SetInternalField(Nan::imp::kDataIndex, val);
  }

  tpl->SetIndexedPropertyHandler(
      getter_
    , setter_
    , query_
    , deleter_
    , enumerator_
    , obj);
}

#endif
#endif  // NAN_CALLBACKS_H_

