// -----------------------------------------------------------------------------
// Howard Hughes
// Windows named mutex wrapped object for node_filemap
// -----------------------------------------------------------------------------

#include "mutex.h"
#include <malloc.h> // alloca

// -----------------------------------------------------------------------------

namespace node_filemap
{
  using namespace v8;

  // ---------------------------------------------------------------------------

  namespace
  {
    const char *ToCString(Local<String> str)
    {
      String::Utf8Value value(str);
      return *value ? *value : "<string conversion failed>";
    }
  }

  // ---------------------------------------------------------------------------

  mutex::mutex() :
    m_mutex(INVALID_HANDLE_VALUE)
  {
  }

  mutex::~mutex()
  {
    if (m_mutex != INVALID_HANDLE_VALUE)
      CloseHandle(m_mutex);
  }

  // ---------------------------------------------------------------------------

  void mutex::create(const char *name, Isolate *isolate)
  {
    m_mutex = CreateMutex(
      nullptr,
      FALSE,
      name);

    if (m_mutex == INVALID_HANDLE_VALUE)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to create mutex, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }
  }

  void mutex::open(const char *name, Isolate *isolate)
  {
    m_mutex = OpenMutex(
      SYNCHRONIZE,
      FALSE,
      name);

    if (m_mutex == INVALID_HANDLE_VALUE)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to create mutex, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }
  }

  // ---------------------------------------------------------------------------

  void mutex::New(const v8::FunctionCallbackInfo<v8::Value>& args)
  {
    auto isolate = args.GetIsolate();

    if (args.IsConstructCall())
    {
      auto obj = new mutex();
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    }
    else
    {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Must create Mutex with new")));
      return;
    }
  }

  // ---------------------------------------------------------------------------

  void mutex::Create(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<mutex>(args.Holder());

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to Mutex.create")));
      return;
    }

    if (!args[0]->IsString())
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type arguments to Mutex.create")));
      return;
    }

    const char *name = ToCString(args[0]->ToString());

    obj->create(name, isolate);
  }

  void mutex::Open(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<mutex>(args.Holder());

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to Mutex.open")));
      return;
    }

    if (!args[0]->IsString())
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type arguments to Mutex.open")));
      return;
    }

    const char *name = ToCString(args[0]->ToString());

    obj->open(name, isolate);
  }

  void mutex::Close(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<mutex>(args.Holder());

    if (obj->m_mutex != INVALID_HANDLE_VALUE)
      CloseHandle(obj->m_mutex);
  }

  void mutex::Wait(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<mutex>(args.Holder());

    DWORD ms;

    if (args.Length() < 1)
    {
      ms = INFINITE;
    }
    else
    {
      if (!args[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type arguments to Mutex.wait")));
        return;
      }

      ms = args[0]->Uint32Value();
    }

    DWORD waitResult = WaitForSingleObject(obj->m_mutex, ms);

    if (waitResult == WAIT_FAILED)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to wait on mutex, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }

    args.GetReturnValue().Set(Integer::New(isolate, waitResult));
  }

  void mutex::WaitMultiple(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<mutex>(args.Holder());

    if (args.Length() < 3)
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to Mutex.waitMultiple")));
      return;
    }

    if (!(
      args[0]->IsArray() &&
      args[1]->IsBoolean() &&
      args[2]->IsNumber()))
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type arguments to Mutex.waitMultiple")));
      return;
    }

    auto mutexList = Local<Array>::Cast(args[0]);
    bool waitAll = args[1]->BooleanValue();
    auto waitFor = args[2]->Uint32Value();

    auto len = mutexList->Length();
    HANDLE *mutexArr = reinterpret_cast<HANDLE *>(alloca(len * sizeof(HANDLE)));
    for (unsigned i = 0; i < mutexList->Length(); ++i)
    {
      auto it = mutexList->Get(i);
      mutexArr[i] = ObjectWrap::Unwrap<mutex>(it->ToObject())->m_mutex;
    }
    
    DWORD waitResult = WaitForMultipleObjects(
      mutexList->Length(),
      mutexArr,
      waitAll,
      waitFor);

    if (waitResult == WAIT_FAILED)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to wait on mutex, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }

    args.GetReturnValue().Set(Integer::New(isolate, waitResult));
  }
  
  void mutex::Release(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<mutex>(args.Holder());

    ReleaseMutex(obj->m_mutex);
  }

  // ---------------------------------------------------------------------------

  v8::Persistent<v8::Function> mutex::constructor;

  // ---------------------------------------------------------------------------

  void mutex::Init(v8::Local<v8::Object> exports)
  {
    auto isolate = exports->GetIsolate();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Mutex"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "create", Create);
    NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "wait", Wait);
    NODE_SET_PROTOTYPE_METHOD(tpl, "waitMultiple", WaitMultiple);
    NODE_SET_PROTOTYPE_METHOD(tpl, "release", Release);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(
      String::NewFromUtf8(isolate, "Mutex"),
      tpl->GetFunction());
  }

  // ---------------------------------------------------------------------------
}

// -----------------------------------------------------------------------------
