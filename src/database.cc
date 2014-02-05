
#include <node.h> 
#include <node_buffer.h>
#include "database.h"
#include "iterator.h"
#include <bangdb/database.h>
#include <bangdb/resultset.h>
#include "autodestroy.h"
#include "database_async.h"
using node::AtExit;

namespace bangdb {
static v8::Persistent<v8::FunctionTemplate> database_constructor;

static void at_exit_destroy_db(void* arg) {
  Database* obj = (Database*)arg;

  obj->CloseTable((char*)"anyname");
  obj->CloseDatabase();
}

Database::Database (char* name) : name(name) {
  this->OpenDatabase(name);
};

Database::~Database () {
  //printf("Destroying DB Handle");
  
  if (bangconnection != NULL) {
    bangconnection->closeconnection();
    //delete bangconnection;
  }

  if (bangtable != NULL) {
    bangtable->closetable();
    //delete bangtable;
  }

  if (bangdb != NULL) {
    bangdb->closedatabase();
    //delete bangdb;
  }    
  //delete[] name;
};

const char* Database::Name() const { return name; }

database* Database::OpenDatabase (
  char* location
    ) {
  bangdb = new database((char*)location);
  return bangdb;
}

int Database::CloseDatabase () {
  bangdb->closedatabase();
  bangdb = NULL; 
  return 1;
}

void Database::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Database::New);
  NanAssignPersistent(v8::FunctionTemplate, database_constructor, tpl);
  tpl->SetClassName(NanSymbol("Database"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Database::Open);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "new", Database::New);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", Database::Put);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Database::Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Database::Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "free", Database::Free);
  NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", Database::Iterator);
}

/*
TODO: Convert from using single table
To a list of tables.
*/
int Database::OpenTable (char* tablename) {
  bangtable = bangdb->gettable(tablename);
  bangconnection = bangtable->getconnection();
  return 1;
}

int Database::CloseTable (char* tablename) {
  if (bangconnection != NULL) {
    bangconnection->closeconnection();
    bangconnection = NULL;
  }

  if (bangtable != NULL) {
    bangtable->closetable();
    bangtable = NULL;
  }
  return 1;
}

int Database::PutValue(char* key, char* val) {

  FDT ikey, ival;

  ikey.data = key;
  ikey.length = strlen(key);
  ival.data = (void*)val;
  ival.length = strlen(val);

  int flag = bangconnection->put(&ikey, &ival, INSERT_UPDATE); 
  return flag;
}

FDT* Database::GetValue(char* key, std::string& value) { 

  FDT ikey;

  ikey.data = key;
  ikey.length = strlen(key);

  FDT* result = bangconnection->get(&ikey);
 
  value.assign((char*)result->data, result->length);
  return result;
}

resultset* Database::NewIterator (char* skey, char* ekey) {

  resultset* rs = bangconnection->scan(skey, ekey);

  return rs;
}

NAN_METHOD(Database::New) {
  NanScope();

  if (args.Length() == 0)
    return NanThrowError("constructor requires at least a location argument");

  if (!args[0]->IsString())
    return NanThrowError("constructor requires a location string argument");

  char* name = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  Database* obj = new Database(name);
  //Shell::autoDestroy(obj);
  AtExit(at_exit_destroy_db, obj);
  obj->Wrap(args.This());
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

NAN_METHOD(Database::Close) {
  NanScope();
 
  Database* _db = ObjectWrap::Unwrap<Database>(args.This());
 
  char* tablename = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  _db->CloseTable(tablename);

  NanReturnUndefined();
}

NAN_METHOD(Database::Free) {
  NanScope();
 
  Database* _db = ObjectWrap::Unwrap<Database>(args.This());

  _db->CloseDatabase(); 

  NanReturnUndefined();
}

NAN_METHOD(Database::Put) {
  NanScope();

  Database* _db = ObjectWrap::Unwrap<Database>(args.This());

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();
  v8::Local<v8::Object> valueHandle = args[1].As<v8::Object>();
  char* key = NanFromV8String(keyHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
  char* val = NanFromV8String(valueHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  v8::Local<v8::Function> callback = args[2].As<v8::Function>();
  WriteWorker* worker  = new WriteWorker(
      _db
    , new NanCallback(callback)
    , key
    , val
    , keyHandle
    , valueHandle
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SavePersistent("database", _this);
  NanAsyncQueueWorker(worker);

  NanReturnUndefined();
}

NAN_METHOD(Database::Get) {
  NanScope();

  Database* _db = ObjectWrap::Unwrap<Database>(args.This());
  v8::Local<v8::Function> callback;

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();
  callback = args[1].As<v8::Function>();   


  ReadWorker* worker = new ReadWorker(
      _db
    , new NanCallback(callback)
    , (char*)NanFromV8String(keyHandle.As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS)
    , keyHandle
  );

  v8::Local<v8::Object> _this = args.This();
  worker->SavePersistent("database", _this);
  NanAsyncQueueWorker(worker);
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
NAN_METHOD(Database::Iterator) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args.This());

  //v8::Local<v8::String> skey = NanFromV8String(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
  //v8::Local<v8::String> ekey = NanFromV8String(args[1].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  v8::Local<v8::String> skey = args[0].As<v8::String>();
  v8::Local<v8::String> ekey = args[1].As<v8::String>();
  v8::TryCatch try_catch;
  v8::Local<v8::Object> iteratorHandle = Iterator::NewInstance(
      args.This()
    , skey
    , ekey
  );

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }

  NanReturnValue(iteratorHandle);
}
}
