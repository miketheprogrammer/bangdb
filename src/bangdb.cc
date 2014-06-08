#include <node.h>
#include <nan.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bangdb.h"
#include "database.h"
#include "scan.h"
#include "transaction.h"
namespace bangdb {

  void Init(v8::Handle<v8::Object> exports) { 

    Database::Init();
    bangdb::Scan::Init();
    bangdb::Transaction::Init();
    v8::Local<v8::Function> bangdb = v8::FunctionTemplate::New(Bang)->GetFunction();

    exports->Set(NanSymbol("bangdb"), bangdb);
  }
}

NODE_MODULE(bangdb_embedded, bangdb::Init);

