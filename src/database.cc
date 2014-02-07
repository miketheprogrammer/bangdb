
#include <node.h> 
#include <node_buffer.h>
#include "database.h"
#include "iterator.h"
#include <bangdb/database.h>
#include <bangdb/resultset.h>
#include "autodestroy.h"
#include "database_async.h"
#include "batch.h"
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
database* Database::GetDatabase() { return bangdb; }

database* Database::OpenDatabase (
  char* location
    ) {
  bangdb = new database((char*)location, NULL, DB_OPTIMISTIC_TRANSACTION);
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
  NODE_SET_PROTOTYPE_METHOD(tpl, "del", Database::Delete);

  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Database::Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "free", Database::Free);
  NODE_SET_PROTOTYPE_METHOD(tpl, "batch", Database::Batch);
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

FILEOFF_T Database::PutValue(char* key, char* val, void* txn_handle) {

  FDT ikey, ival;

  ikey.data = key;
  ikey.length = strlen(key);
  ival.data = (void*)val;
  ival.length = strlen(val);

  FILEOFF_T flag;
  if (txn_handle == NULL) {
    flag = bangconnection->put(&ikey, &ival, INSERT_UPDATE); 
  } else {
    flag = bangconnection->put(&ikey, &ival, INSERT_UPDATE, txn_handle); 
  }
  return flag;
}

FDT* Database::GetValue(char* key, std::string& value, void* txn_handle) { 

  FDT ikey;

  ikey.data = key;
  ikey.length = strlen(key);

  FDT* result;
  if (txn_handle == NULL) {
    result = bangconnection->get(&ikey);
  } else {
    result = bangconnection->get(&ikey, txn_handle);
  }
 
  if (result != NULL) {
    value.assign((char*)result->data, result->length);
  } else {
    value.assign("!ERROR!", 7);
  } 
  return result;
}

FILEOFF_T Database::DeleteValue(char* key, void* txn_handle) {

  FDT ikey;

  ikey.data = key;
  ikey.length = strlen(key);

  FILEOFF_T flag;
  if (txn_handle == NULL) {
    flag = bangconnection->del(&ikey);
  } else {
    flag = bangconnection->del(&ikey, txn_handle);
  }
  return flag;
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

  //If no callback execute sync
  if (callback->IsNull() || callback->IsUndefined()) {
    _db->PutValue(key, val);
  } else {
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
  }
  NanReturnUndefined();
}

NAN_METHOD(Database::Get) {
  NanScope();

  BD_METHOD_SETUP_COMMON(get, 1, 2);
  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();

  char* key = NanFromV8String(keyHandle.As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
  if (callback->IsNull() || callback->IsUndefined()) {
    std::string out;
    FDT* result = _db->GetValue(key, out);
    if (result != NULL) {
      //result->free();
    }
    //NanReturnValue(NanNewBufferHandle((char*)result->data, result->length));
    NanReturnValue(v8::String::New(out.c_str()));

  } else {
    bool asBuffer = NanBooleanOptionValue(optionsObj, NanSymbol("asBuffer"), true);
    bool fillCache = NanBooleanOptionValue(optionsObj, NanSymbol("fillCache"), true);
    ReadWorker* worker = new ReadWorker(
        _db
      , new NanCallback(callback)
      , key
      , asBuffer
      , fillCache
      , keyHandle
    );

    v8::Local<v8::Object> _this = args.This();
    worker->SavePersistent("database", _this);
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

}

NAN_METHOD(Database::Delete) {
  NanScope();

  Database* _db = ObjectWrap::Unwrap<Database>(args.This());

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();
  v8::Local<v8::Object> valueHandle = args[1].As<v8::Object>();
  char* key = NanFromV8String(keyHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  v8::Local<v8::Function> callback = args[2].As<v8::Function>();

  //If no callback execute sync
  if (callback->IsNull() || callback->IsUndefined()) {
    _db->DeleteValue(key);
  } else {
    DeleteWorker* worker  = new DeleteWorker(
        _db
      , new NanCallback(callback)
      , key
      , keyHandle
    );
    // persist to prevent accidental GC
    v8::Local<v8::Object> _this = args.This();
    worker->SavePersistent("database", _this);
    NanAsyncQueueWorker(worker);
  }
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

NAN_METHOD(Database::Batch) {
  NanScope();
  if ((args.Length() == 0 || args.Length() == 1) && !args[0]->IsArray()) { 
    NanReturnValue(Batch::NewInstance(args.This()));
  }

  v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(args[0]);

  Database* _db = ObjectWrap::Unwrap<Database>(args.This());

  database* bdb = _db->GetDatabase();

  v8::Local<v8::Function> callback = args[1].As<v8::Function>();
  void* txn_handle = bdb->begin_transaction();
  bool hasData = false;

  for (unsigned int i = 0; i < array->Length(); i++) {
    if(!array->Get(i)->IsObject())
      continue;
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(array->Get(i));
    v8::Local<v8::Value> keyHandle = obj->Get(NanSymbol("key"));
    char* key = NanFromV8String(keyHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
    if (obj->Get(NanSymbol("type"))->StrictEquals(NanSymbol("del"))) {
      _db->DeleteValue(key, txn_handle);
      if (!hasData)
        hasData = true;
    } else if (obj->Get(NanSymbol("type"))->StrictEquals(NanSymbol("put"))) {
      v8::Local<v8::Value> valueHandle = obj->Get(NanSymbol("value"));
      char* value = NanFromV8String(valueHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
      _db->PutValue(key, value, txn_handle);
      if (!hasData)
        hasData = true;
    }
  }
  v8::Local<v8::Object> arr = v8::Local<v8::Object>::Cast(array);
  if (hasData) {
    BatchWorker* worker = new BatchWorker(
        _db
      , new NanCallback(callback)
      , txn_handle
      , arr
    );
    v8::Local<v8::Object> _this = args.This();
    worker->SavePersistent("_db", _this);
    NanAsyncQueueWorker(worker);
  } else {
    //node::MakeCallback(                                                          \
    //  v8::Context::GetCurrent()->Global(), callback, 0, NULL);
  }
  
  NanReturnUndefined();
}

}
