
#include <node.h> 
#include <bangdb/database.h>
#include "database.h"

namespace bangdown {
static v8::Persistent<v8::FunctionTemplate> database_constructor;

Database::Database (char* name) : location(location) {
  db = NULL;
  currentIteratorId = 0;
  pendingCloseWorker = NULL;
};

Database::~Database () {
  if (db != NULL)
    db->closedatabase();
    delete db;
  delete[] bane;
};

database* Database::OpenDatabase (
      std::string location
    ) {
  database* db;
  db = new database((char*)location);
  printf("%s Created\n", db->getdbname());
  return db;
}
void Database::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Database::New);
  NanAssignPersistent(v8::FunctionTemplate, database_constructor, tpl);
  tpl->SetClassName(NanSymbol("Database"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(Database::New) {
  NanScope();

  if (args.Length() == 0)
    return NanThrowError("constructor requires at least a location argument");

  if (!args[0]->IsString())
    return NanThrowError("constructor requires a location string argument");

  char* name = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  database* obj = new database(name);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}
  NAN_METHOD(Bang){
    NanScope();

    v8::Local<v8::String> name;
    if (args.Length() != 0 && args[0]->IsString())
      name = args[0].As<v8::String>();
    Database::Init();
  }

}
