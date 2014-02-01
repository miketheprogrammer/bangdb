#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <nan.h>
#include <bangdb/database.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bangdown.h"
#include "database.h"
namespace bangdown {


  NAN_METHOD(CreateDb) {
    NanScope();

    database* db;
    char* location = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
    printf("Creating new db\n");
    db = new database((char*)location);
    printf("%s Created\n", db->getdbname());

    table *tbl = db->gettable((char*)"mytable1");
    connection *conn = tbl->getconnection();

    FDT ikey, ival, *out;

//    char* key = location;
//    char* val = location;


    ikey.data = location;
    ikey.length = strlen(location);
    ival.data = (void*)location;
    ival.length = strlen(location);
    int ret;
    ret=conn->put(&ikey, &ival, INSERT_UPDATE);
    printf("PUT Finished with %d\n",ret);   
    out = conn->get(&ikey);
    printf("GET Finished\n");
    printf("GET ReturnValue(%s)\n", (char*)out->data);

//    key = null;
//    val = null;
    db->closedatabase();
    delete db;
    NanReturnUndefined();
  }

  NAN_METHOD(TestPut) {
    NanScope();
    database *db;

    char* location = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

    db = new database((char*)location);

  //printf("could not insert %d keys\n", 10);
  db->closedatabase();
  delete db;

 // NanReturnValue(v8::Number::New(successflag)); 
  NanReturnUndefined();
 }

  void Init(v8::Handle<v8::Object> exports) { 
    Database::Init();
    v8::Local<v8::Function> bangdown = v8::FunctionTemplate::New(Bang)->GetFunction();

    bangdown->Set(NanSymbol("create"), v8::FunctionTemplate::New(CreateDb)->GetFunction());
    bangdown->Set(NanSymbol("testput"), v8::FunctionTemplate::New(TestPut)->GetFunction());
    exports->Set(NanSymbol("bangdown"), bangdown);
  }
}

NODE_MODULE(bangdown, bangdown::Init);

using namespace v8;


Handle<Value> CreateObject(const Arguments& args) {
  HandleScope scope;

  Local<Object> obj = Object::New();
  obj->Set(String::NewSymbol("msg"), args[0]->ToString());

  return scope.Close(obj);
}

void Init(Handle<Object> exports, Handle<Object> module) {
  Database::Init();
  module->Set(String::NewSymbol("exports"),
      FunctionTemplate::New(CreateObject)->GetFunction());
}

//NODE_MODULE(addon, Init)
