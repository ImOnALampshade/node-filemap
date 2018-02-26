// -----------------------------------------------------------------------------
// Howard Hughes
// NodeJS addon initializer for node_filemap
// -----------------------------------------------------------------------------

#include <node.h>
#include "filemap.h"
#include "mutex.h"

// -----------------------------------------------------------------------------

namespace node_filemap
{
  // ---------------------------------------------------------------------------

  using namespace v8;

  void init(Local<Object> exports)
  {
    file_mapping::Init(exports);
    mutex::Init(exports);

    exports->Set(String::NewFromUtf8(exports->GetIsolate(), "INFINITE"), Integer::New(exports->GetIsolate(), INFINITE));
    exports->Set(String::NewFromUtf8(exports->GetIsolate(), "WAIT_ABANDONED"), Integer::New(exports->GetIsolate(), WAIT_ABANDONED));
    exports->Set(String::NewFromUtf8(exports->GetIsolate(), "WAIT_ABANDONED_0"), Integer::New(exports->GetIsolate(), WAIT_ABANDONED_0));
    exports->Set(String::NewFromUtf8(exports->GetIsolate(), "WAIT_OBJECT_0"), Integer::New(exports->GetIsolate(), WAIT_OBJECT_0));
    exports->Set(String::NewFromUtf8(exports->GetIsolate(), "WAIT_TIMEOUT"), Integer::New(exports->GetIsolate(), WAIT_TIMEOUT));
  }

  NODE_MODULE(NODE_GYP_MODULE_NAME, init)

  // ---------------------------------------------------------------------------
}

// -----------------------------------------------------------------------------
