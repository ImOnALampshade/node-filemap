// -----------------------------------------------------------------------------
// Howard Hughes
// Windows named mutex wrapped object for node_filemap
// -----------------------------------------------------------------------------

#ifndef NODEJS_MUTEX_H
#define NODEJS_MUTEX_H

#pragma once

// -----------------------------------------------------------------------------

#include <node.h>
#include <node_object_wrap.h>
#include <node_api.h>
#include <windows.h>
#include <node_buffer.h>

// -----------------------------------------------------------------------------

namespace node_filemap
{
  // ---------------------------------------------------------------------------

  class mutex : public node::ObjectWrap
  {
  public:
    mutex();
    ~mutex();

    void create(const char *name, v8::Isolate *isolate);
    void open(const char *name, v8::Isolate *isolate);

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void Create(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void Open(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void Close(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void Wait(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void WaitMultiple(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void Release(const v8::FunctionCallbackInfo<v8::Value> &args);

    static v8::Persistent<v8::Function> constructor;

    static void Init(v8::Local<v8::Object> exports);

  private:
    HANDLE m_mutex;
  };

  // ---------------------------------------------------------------------------
}

// -----------------------------------------------------------------------------

#endif
