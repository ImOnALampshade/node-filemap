// -----------------------------------------------------------------------------
// Howard Hughes
// file_mapping wrapped object for node_filemap
// -----------------------------------------------------------------------------

#ifndef NODEJS_FILEMAP_H
#define NODEJS_FILEMAP_H

#pragma once

// -----------------------------------------------------------------------------

#include <node.h>
#include <node_object_wrap.h>
#include <windows.h>
#include <node_buffer.h>

// -----------------------------------------------------------------------------

namespace node_filemap
{
  // ---------------------------------------------------------------------------

  class file_mapping : public node::ObjectWrap
  {
  public:
    file_mapping();
    ~file_mapping();

    void create_mapping(const char *filename, const char *mappingName, unsigned mappingSize, v8::Isolate *isolate);
    void open_mapping(const char *mappingName, unsigned mappingSize, v8::Isolate *isolate);
    void close_mapping();

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void CreateMapping(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void OpenMapping(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void CloseMapping(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void WriteBuffer(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ReadInto(const v8::FunctionCallbackInfo<v8::Value> &args);

    static v8::Persistent<v8::Function> constructor;

    static void Init(v8::Local<v8::Object> exports);

  private:
    HANDLE m_fileHandle;
    HANDLE m_mappingHandle;
    void *m_ptr;
  };

  // ---------------------------------------------------------------------------
}

// -----------------------------------------------------------------------------

#endif
