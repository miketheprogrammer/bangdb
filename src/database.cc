
#include <node.h> 
#include <node_buffer.h>
#include "database.h"
#include "scan.h"
#include <bangdb/database.h>
#include <bangdb/resultset.h>
#include "autodestroy.h"
#include "database_async.h"
#include "transaction.h"
#include <inttypes.h>
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
  NanAssignPersistent(database_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("Database"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Database::Open);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "new", Database::New);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", Database::Put);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Database::Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "del", Database::Delete);

  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Database::Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "free", Database::Free);
  NODE_SET_PROTOTYPE_METHOD(tpl, "transaction", Database::Transaction);
  NODE_SET_PROTOTYPE_METHOD(tpl, "scan", Database::Scan);
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

resultset* Database::NewScan (char* skey, char* ekey, v8::Local<v8::Object> optionsObj) {
  scan_filter sf;
  
  // Messy switches is there a better way to handle enum props with javascript inputs
  uint32_t skey_op = NanUInt32OptionValue(optionsObj, NanNew<v8::String>("skey_op"), 1);
  uint32_t ekey_op = NanUInt32OptionValue(optionsObj, NanNew<v8::String>("ekey_op"), 0);
  uint32_t limit_by = NanUInt32OptionValue(optionsObj, NanNew<v8::String>("limit_by"), 1);
  uint32_t limit = NanUInt32OptionValue(optionsObj, NanNew<v8::String>("limit"), 100);
  //printf("has ekey_op :: %u \n", skey_op);
  switch (skey_op) {
    case 0:
      sf.skey_op = GT;
      break;
    case 1:
      sf.skey_op = GTE;
      break;
    case 2:
      sf.skey_op = LT;
      break;
    case 3:
      sf.skey_op = LTE;
      break;
    default:
      sf.skey_op = GT;
  }
  switch (ekey_op) {
    case 0:
      sf.ekey_op = GT;
      break;
    case 1:
      sf.ekey_op = GTE;
      break;
    case 2:
      sf.ekey_op = LT;
      break;
    case 3:
      sf.ekey_op = LTE;
      break;
    default:
      sf.ekey_op = LTE;
  }

  if (limit_by == 0) {
    sf.limitby = LIMIT_RESULT_SIZE;
  } else {
    sf.limitby = LIMIT_RESULT_ROW;
  }

  sf.limit = limit;  //1000 rows

  resultset* rs = bangconnection->scan(skey, ekey, &sf);

  return rs;
}

NAN_METHOD(Database::New) {
  NanScope();

  if (args.Length() == 0)
    return NanThrowError("constructor requires at least a location argument");

  if (!args[0]->IsString())
    return NanThrowError("constructor requires a location string argument");

  size_t count;
  char* name = NanCString(args[0], &count);

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
 
  size_t count;
  char* tablename = NanCString(args[0], &count);

  _db->OpenTable(tablename);

  
  NanReturnUndefined();
}

NAN_METHOD(Database::Close) {
  NanScope();
 
  Database* _db = ObjectWrap::Unwrap<Database>(args.This());
 
  size_t count;
  char* tablename = NanCString(args[0], &count);

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

  size_t cKey;
  size_t cVal;
  char* key = NanCString(args[0], &cKey);
  char* val = NanCString(args[1], &cVal);

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
    worker->SaveToPersistent("database", _this);
    NanAsyncQueueWorker(worker);
  }
  NanReturnUndefined();
}

NAN_METHOD(Database::Get) {
  NanScope();

  BD_METHOD_SETUP_COMMON(get, 1, 2);

  size_t cKey;
  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();

  char* key = NanCString(args[0], &cKey);
  if (callback->IsNull() || callback->IsUndefined()) {
    std::string out;
    FDT* result = _db->GetValue(key, out);

    //NanReturnValue(NanNewBufferHandle((char*)result->data, result->length));
    NanReturnValue(v8::String::New(out.c_str()));

    if (result != NULL) {
      result->free();
    }
  } else {
    bool asBuffer = NanBooleanOptionValue(optionsObj, NanNew<v8::String>("asBuffer"), true);
    bool fillCache = NanBooleanOptionValue(optionsObj, NanNew<v8::String>("fillCache"), true);
    ReadWorker* worker = new ReadWorker(
        _db
      , new NanCallback(callback)
      , key
      , asBuffer
      , fillCache
      , keyHandle
    );

    v8::Local<v8::Object> _this = args.This();
    worker->SaveToPersistent("database", _this);
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

}

NAN_METHOD(Database::Delete) {
  NanScope();

  Database* _db = ObjectWrap::Unwrap<Database>(args.This());

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();

  size_t cKey;
  char* key = NanCString(args[0], &cKey);

  v8::Local<v8::Function> callback = args[1].As<v8::Function>();

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
    worker->SaveToPersistent("database", _this);
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
      NanNew<v8::FunctionTemplate>(database_constructor);

  if (name.IsEmpty()) {
    instance = constructorHandle->GetFunction()->NewInstance(0, NULL);
  } else {
    v8::Handle<v8::Value> argv[] = { name };
    instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  }

  return instance;
}

NAN_METHOD(Database::Scan) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args.This());

  //v8::Local<v8::String> skey = NanCString(args[0].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
  //v8::Local<v8::String> ekey = NanCString(args[1].As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  v8::Local<v8::String> skey = args[0].As<v8::String>();
  v8::Local<v8::String> ekey = args[1].As<v8::String>();
  v8::Local<v8::Object> optionsObj;
  if (args[2]->IsObject()) {
    printf("saving Object \n");
    printf("saving object\n");
    optionsObj = args[2].As<v8::Object>();
    printf("is empty :: %d \n", optionsObj.IsEmpty());
    printf("has skey_op :: %d \n", optionsObj->Has(NanNew<v8::String>("skey_op")));
  }

  v8::TryCatch try_catch;
  
  v8::Local<v8::Object> scanHandle = Scan::NewInstance(
      args.This()
    , skey
    , ekey
    , optionsObj
  );

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }

  NanReturnValue(scanHandle);
}

NAN_METHOD(Database::Transaction) {
  NanScope();
  if ((args.Length() == 0 || args.Length() == 1) && !args[0]->IsArray()) { 
    NanReturnValue(Transaction::NewInstance(args.This()));
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

    v8::Local<v8::Value> keyHandle = obj->Get(NanNew<v8::String>("key"));

    size_t cKey;
    char* key = NanCString(keyHandle, &cKey);

    if (obj->Get(NanNew<v8::String>("type"))->StrictEquals(NanNew<v8::String>("del"))) {
      
      _db->DeleteValue(key, txn_handle);
      
      if (!hasData)
        hasData = true;
    } else if (obj->Get(NanNew<v8::String>("type"))->StrictEquals(NanNew<v8::String>("put"))) {
  
      v8::Local<v8::Value> valueHandle = obj->Get(NanNew<v8::String>("value"));

      size_t cVal;
      char* value = NanCString(valueHandle, &cVal);
      
      _db->PutValue(key, value, txn_handle);
      if (!hasData)
        hasData = true;
    }
  }
  v8::Local<v8::Object> arr = v8::Local<v8::Object>::Cast(array);
  if (hasData) {
    TransactionWorker* worker = new TransactionWorker(
        _db
      , new NanCallback(callback)
      , txn_handle
      , arr
    );
    v8::Local<v8::Object> _this = args.This();
    worker->SaveToPersistent("_db", _this);
    NanAsyncQueueWorker(worker);
  } else {
    //node::MakeCallback(                                                          \
    //  v8::Context::GetCurrent()->Global(), callback, 0, NULL);
  }
  
  NanReturnUndefined();
}

}
