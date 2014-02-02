#include <node.h>
#include <nan.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bangdown.h"
#include "database.h"
namespace bangdown {

  void Init(v8::Handle<v8::Object> exports) { 

    Database::Init();
    v8::Local<v8::Function> bangdown = v8::FunctionTemplate::New(Bang)->GetFunction();

    exports->Set(NanSymbol("bangdown"), bangdown);
  }
}

NODE_MODULE(bangdown, bangdown::Init);

