
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
  _db->GetValue(key, value)->free();
}

void ReadWorker::HandleOKCallback () {
  NanScope();

  v8::Local<v8::Value> returnValue;
  if (false) {
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

}
