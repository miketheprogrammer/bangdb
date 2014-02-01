
#include <node.h> 
#include <node_buffer.h>
#include "database.h"

namespace bangdown {
static v8::Persistent<v8::FunctionTemplate> database_constructor;

Database::Database (char* name) : name(name) {
  this->OpenDatabase(name);
};

Database::~Database () {
  if (bangdb != NULL)
    bangdb->closedatabase();
    delete bangdb;
  delete[] name;
};

database* Database::OpenDatabase (
  char* location
    ) {
  //database* db;
  bangdb = new database((char*)location);
  printf("%s Created\n", bangdb->getdbname());
  return bangdb;
}
void Database::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Database::New);
  NanAssignPersistent(v8::FunctionTemplate, database_constructor, tpl);
  tpl->SetClassName(NanSymbol("Database"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Database::Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "new", Database::New);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", Database::Put);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Database::Get);
}

int Database::OpenTable (char* tablename) {
  bangtable = bangdb->gettable(tablename);
  bangconnection = bangtable->getconnection();
  return 1;
}

int Database::PutValue(char* key, char* val) {

  FDT ikey, ival;

  ikey.data = key;
  ikey.length = strlen(key);
  ival.data = (void*)val;
  ival.length = strlen(val);
  
  printf("PUTTING data\n");  
  
  int flag = bangconnection->put(&ikey, &ival, INSERT_UPDATE);

  printf("Put has flag %d\n",flag);

  return flag;
}

FDT* Database::GetValue(char* key) { 

  FDT ikey;

  ikey.data = key;
  ikey.length = strlen(key);

  FDT* result = bangconnection->get(&ikey);
 
  printf("result: (%s) \n", (char*)result->data);

  return result;
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
NAN_METHOD(Database::Open) {
  NanScope();
 
  Database* _db = ObjectWrap::Unwrap<Database>(args.This());
 
  char* tablename = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  _db->OpenTable(tablename);

  
  NanReturnUndefined();
}
NAN_METHOD(Database::Put) {
    NanScope();
 
    Database* _db = ObjectWrap::Unwrap<Database>(args.This());
 
    char* tablename = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
    char* key = NanFromV8String(args[1].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
    char* val = NanFromV8String(args[2].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

    printf("PUT ARGS %s %s %s", tablename, key, val);
    _db->OpenTable(tablename);
    _db->PutValue(key, val);

    printf("Done putting \n");

    NanReturnUndefined();
}

NAN_METHOD(Database::Get) {
  NanScope();

  Database* _db = ObjectWrap::Unwrap<Database>(args.This());

  char* tablename = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
  char* key = NanFromV8String(args[1].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  _db->OpenTable(tablename);
  printf("GET ARGS: %s %s", tablename, key);

  FDT* result = _db->GetValue(key);

  NanReturnValue(v8::String::New((char*)result->data));
  result->free();
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
