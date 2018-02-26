// -----------------------------------------------------------------------------
// Howard Hughes
// file_mapping wrapped object for node_filemap
// -----------------------------------------------------------------------------

#include "filemap.h"

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

  Persistent<Function> file_mapping::constructor;

  // ---------------------------------------------------------------------------

  file_mapping::file_mapping() :
    m_fileHandle(INVALID_HANDLE_VALUE),
    m_mappingHandle(INVALID_HANDLE_VALUE)
  {
  }

  file_mapping::~file_mapping()
  {
    if (m_fileHandle != INVALID_HANDLE_VALUE)
      CloseHandle(m_fileHandle);
    if (m_mappingHandle != INVALID_HANDLE_VALUE)
      CloseHandle(m_mappingHandle);
  }

  // ---------------------------------------------------------------------------

  void file_mapping::create_mapping(const char *fileName, const char *mappingName, unsigned mappingSize, Isolate *isolate)
  {
    if (fileName == nullptr)
    {
      m_fileHandle = INVALID_HANDLE_VALUE; // Used for plain shared memory, with no backing file
    }
    else
    {
      m_fileHandle = CreateFile(
        fileName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

      if (m_fileHandle == INVALID_HANDLE_VALUE)
      {
        int lastErr = GetLastError();
        auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to create file, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

        isolate->ThrowException(Exception::Error(errStr));
        return;
      }
    }

    m_mappingHandle = CreateFileMapping(
      m_fileHandle,
      nullptr,
      PAGE_READWRITE,
      0,
      mappingSize,
      mappingName);

    if (m_mappingHandle == INVALID_HANDLE_VALUE)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to create file mapping, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }

    m_ptr = MapViewOfFile(
      m_mappingHandle,
      FILE_MAP_ALL_ACCESS,
      0,
      0,
      mappingSize);

    if (m_ptr == nullptr)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to map view of file, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }
  }

  // ---------------------------------------------------------------------------

  void file_mapping::open_mapping(const char *mappingName, unsigned mappingSize, Isolate *isolate)
  {
    m_mappingHandle = OpenFileMapping(
      FILE_MAP_ALL_ACCESS,
      FALSE,
      mappingName);

    if (m_mappingHandle == INVALID_HANDLE_VALUE)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to open file mapping, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }

    m_ptr = MapViewOfFile(
      m_mappingHandle,
      FILE_MAP_ALL_ACCESS,
      0,
      0,
      mappingSize);

    if (m_ptr == nullptr)
    {
      int lastErr = GetLastError();
      auto errStr = String::Concat(String::NewFromUtf8(isolate, "Failed to map view of file, error code: "), Integer::New(isolate, lastErr)->ToString(isolate));

      isolate->ThrowException(Exception::Error(errStr));
      return;
    }
  }

  // ---------------------------------------------------------------------------

  void file_mapping::close_mapping()
  {
    if (m_fileHandle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(m_fileHandle);
      m_fileHandle = INVALID_HANDLE_VALUE;
    }
    if (m_mappingHandle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(m_mappingHandle);
      m_mappingHandle = INVALID_HANDLE_VALUE;
    }
  }

  // ---------------------------------------------------------------------------

  void file_mapping::New(const FunctionCallbackInfo<Value>& args)
  {
    auto isolate = args.GetIsolate();

    if (args.IsConstructCall())
    {
      auto obj = new file_mapping();
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    }
    else
    {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Must create FileMapping with new")));
      return;
    }
  }

  // ---------------------------------------------------------------------------

  void file_mapping::CreateMapping(const FunctionCallbackInfo<Value>& args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<file_mapping>(args.Holder());

    if (args.Length() < 3)
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to FileMapping.createMapping")));
      return;
    }

    if (!((args[0]->IsNull() || args[0]->IsString()) &&
           args[1]->IsString() &&
           args[2]->IsNumber()))
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type arguments to FileMapping.createMapping")));
      return;
    }

    const char *filename = args[0]->IsNull() ? nullptr : ToCString(args[0]->ToString());
    const char *mappingName = ToCString(args[1]->ToString());
    auto mappingSize = args[2]->Uint32Value();

    obj->create_mapping(filename, mappingName, mappingSize, isolate);
  }
  
  void file_mapping::OpenMapping(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<file_mapping>(args.Holder());

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to FileMapping.openMapping")));
      return;
    }

    if (!(args[0]->IsString() && args[1]->IsNumber()))
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type arguments to FileMapping.openMapping")));
      return;
    }

    const char *mappingName = ToCString(args[0]->ToString());
    auto mappingSize = args[1]->Uint32Value();

    obj->open_mapping(mappingName, mappingSize, isolate);
  }

  void file_mapping::CloseMapping(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<file_mapping>(args.Holder());

    obj->close_mapping();
  }

  void file_mapping::WriteBuffer(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<file_mapping>(args.Holder());

    if (args.Length() < 3)
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to FileMapping.writeBuffer")));
      return;
    }

    if (!(node::Buffer::HasInstance(args[0]) && args[1]->IsNumber() && args[2]->IsNumber()))
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to FileMapping.writeBuffer")));
      return;
    }

    void *bufferData = node::Buffer::Data(args[0]);
    auto destOffest  = args[1]->Uint32Value();
    auto srcOffset   = args[2]->Uint32Value();
    auto length      = args[3]->Uint32Value();

    memcpy(reinterpret_cast<char *>(obj->m_ptr) + destOffest, reinterpret_cast<char *>(bufferData) + srcOffset, length);
  }

  // ---------------------------------------------------------------------------

  void file_mapping::ReadInto(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<file_mapping>(args.Holder());

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to FileMapping.writeBuffer")));
      return;
    }

    if (!(args[0]->IsNumber() && args[1]->IsNumber() && node::Buffer::HasInstance(args[2])))
    {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Not enough arguments to FileMapping.writeBuffer")));
      return;
    }

    auto offset = args[0]->Uint32Value();
    auto length = args[1]->Uint32Value();
    char *bufferData = node::Buffer::Data(args[2]);
    
    bufferData += offset;
    memcpy(bufferData, obj->m_ptr, length);
  }

  // ---------------------------------------------------------------------------

  void file_mapping::Init(v8::Local<v8::Object> exports)
  {
    auto isolate = exports->GetIsolate();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "FileMapping"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "createMapping", CreateMapping);
    NODE_SET_PROTOTYPE_METHOD(tpl, "openMapping", OpenMapping);
    NODE_SET_PROTOTYPE_METHOD(tpl, "closeMapping", CloseMapping);
    NODE_SET_PROTOTYPE_METHOD(tpl, "writeBuffer", WriteBuffer);
    NODE_SET_PROTOTYPE_METHOD(tpl, "readInto", ReadInto);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(
      String::NewFromUtf8(isolate, "FileMapping"), 
      tpl->GetFunction());
  }

  // ---------------------------------------------------------------------------
}

// -----------------------------------------------------------------------------
