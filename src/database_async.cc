
#include <node.h>
#include <node_buffer.h>
#include "database.h"
#include "bangdb.h"
#include <bangdb/database.h>
#include "database_async.h"

namespace bangdb {

OpenWorker::OpenWorker (
    Database *_db
  , NanCallback *callback
) : AsyncWorker(_db, callback)
{
};

OpenWorker::~OpenWorker () {
}

void OpenWorker::Execute () {
  _db->OpenTable((char*) _db->Name());
}

/** CLOSE WORKER **/

CloseWorker::CloseWorker (
    Database *_db
  , NanCallback *callback
) : AsyncWorker(_db, callback)
{};

CloseWorker::~CloseWorker () {}

void CloseWorker::Execute () {
  _db->CloseTable((char*)_db->Name());
}

void CloseWorker::WorkComplete () {
  NanScope();
  HandleOKCallback();
  delete callback;
  callback = NULL;
}

/** IO WORKER (abstract) **/

IOWorker::IOWorker (
    Database *_db
  , NanCallback *callback
  , char* key
  , v8::Local<v8::Object> &keyHandle
) : AsyncWorker(_db, callback)
, key(key)
{
  NanScope();

  SavePersistent("key", keyHandle);
};

IOWorker::~IOWorker () {}

void IOWorker::WorkComplete () {
  NanScope();

  //DisposeStringOrBufferFromSlice(GetFromPersistent("key"), key);
  AsyncWorker::WorkComplete();
}

/** READ WORKER **/

ReadWorker::ReadWorker (
    Database *_db
  , NanCallback *callback
  , char* key
  , bool asBuffer
  , bool fillCache
  , v8::Local<v8::Object> &keyHandle
) : IOWorker(_db, callback, key, keyHandle)
{
  NanScope();

  SavePersistent("key", keyHandle);
};

ReadWorker::~ReadWorker () {
}

void ReadWorker::Execute () {
  //FDT* Object
  FDT* result = _db->GetValue(key, value);
  if (result != NULL) {
    result->free();
  } else {
    value.assign('!ERROR!', 7);
  }
}

void ReadWorker::HandleOKCallback () {
  NanScope();

  v8::Local<v8::Value> returnValue;
  if (asBuffer) {
    returnValue = NanNewBufferHandle((char*)value.data(), value.size());
  } else {
    returnValue = v8::String::New((char*)value.data(), value.size());
  }
  v8::Local<v8::Value> argv[] = {
      NanNewLocal<v8::Value>(v8::Null())
    , returnValue
  };
  callback->Call(2, argv);
}

/** DELETE WORKER **/

DeleteWorker::DeleteWorker (
    Database *_db
  , NanCallback *callback
  , char* key
  , v8::Local<v8::Object> &keyHandle
) : IOWorker(_db, callback, key, keyHandle)
{
  NanScope();

  SavePersistent("key", keyHandle);
};

DeleteWorker::~DeleteWorker () {}

void DeleteWorker::Execute () {
  //database->DeleteFromDatabase(options, key));
  _db->DeleteValue(key);
}

/** WRITE WORKER **/

WriteWorker::WriteWorker (
    Database *_db
  , NanCallback *callback
  , char* key
  , char* val
  , v8::Local<v8::Object> &keyHandle
  , v8::Local<v8::Object> &valueHandle
) : DeleteWorker(_db, callback, key, keyHandle)
  , val(val)
{
  NanScope();

  //SavePersistent("value", valueHandle);
};

WriteWorker::~WriteWorker () {}

void WriteWorker::Execute () {
  //printf("WriteWorker::Execute\n");fflush(stdout);
  FILEOFF_T result = _db->PutValue(key, val);
}

void WriteWorker::WorkComplete () {
  NanScope();

  IOWorker::WorkComplete();
  //printf("WriteWorker::WorkComplete\n");fflush(stdout);
}

BatchWorker::BatchWorker (
    Database *_db
  , NanCallback *callback
  , void* txn_handle
  , v8::Local<v8::Array> array
) : AsyncWorker(_db, callback)
  , txn_handle(txn_handle)
  , array(array)
{};

BatchWorker::~BatchWorker () {}

void BatchWorker::Execute () {
  _db->GetDatabase()->commit_transaction(&txn_handle);
}
void BatchWorker::WorkComplete () {
  NanScope();
  HandleOKCallback();
  delete callback;
  callback = NULL;
}
}
