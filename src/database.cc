
#include <node.h> 
#include <node_buffer.h>
#include "database.h"

namespace bangdown {
static v8::Persistent<v8::FunctionTemplate> database_constructor;

Database::Database (char* name) : name(name) {
  db = NULL;
};

Database::~Database () {
  if (db != NULL)
    db->closedatabase();
    delete db;
  delete[] name;
};

database* Database::OpenDatabase (
  char* location
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
  NODE_SET_PROTOTYPE_METHOD(tpl, "new", Database::New);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", Database::Put);
}

NAN_METHOD(Database::New) {
  NanScope();

  if (args.Length() == 0)
    return NanThrowError("constructor requires at least a location argument");

  if (!args[0]->IsString())
    return NanThrowError("constructor requires a location string argument");

  char* name = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  Database* obj = new Database(name);
  obj->Wrap(args.This());
  printf("NAME: %s \n", name);
//  NanReturnUndefined();
  NanReturnValue(args.This());
}

NAN_METHOD(Database::Put) {
    NanScope();

    char* location = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
    printf("Creating new db\n");
    database* db = new database((char*)location);
    printf("%s Created\n", db->getdbname());

    table *tbl = db->gettable((char*)"mytable2");
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

  NAN_METHOD(Bang){
    NanScope();

    v8::Local<v8::String> name;
    if (args.Length() != 0 && args[0]->IsString())
      name = args[0].As<v8::String>();

    NanReturnValue(Database::NewInstance(name));
  }
v8::Handle<v8::Value> Database::NewInstance (v8::Local<v8::String> &name) {
  NanScope();

  v8::Local<v8::Object> instance;

  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanPersistentToLocal(database_constructor);

  if (name.IsEmpty()) {
    instance = constructorHandle->GetFunction()->NewInstance(0, NULL);
  } else {
    v8::Handle<v8::Value> argv[] = { name };
    instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  }

  return instance;
}
}
