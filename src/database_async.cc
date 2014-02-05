
#include <node.h>
#include <node_buffer.h>
#include "database.h"
#include "bangdb.h"
#include <bangdb/database.h>
#include "database_async.h"

namespace bangdb {

OpenWorker::OpenWorker (
    Database *database
  , NanCallback *callback
) : AsyncWorker(database, callback)
{
};

OpenWorker::~OpenWorker () {
}

void OpenWorker::Execute () {
  database->OpenTable(database->Name());
}

/** CLOSE WORKER **/

CloseWorker::CloseWorker (
    Database *database
  , NanCallback *callback
) : AsyncWorker(database, callback)
{};

CloseWorker::~CloseWorker () {}

void CloseWorker::Execute () {
  database->CloseTable(database->Name());
}

void CloseWorker::WorkComplete () {
  NanScope();
  HandleOKCallback();
  delete callback;
  callback = NULL;
}

/** IO WORKER (abstract) **/

IOWorker::IOWorker (
    Database *database
  , NanCallback *callback
  , v8::Local<v8::Object> &keyHandle
) : AsyncWorker(database, callback)
{
  NanScope();

  SavePersistent("key", keyHandle);
};

IOWorker::~IOWorker () {}

void IOWorker::WorkComplete () {
  NanScope();

  DisposeStringOrBufferFromSlice(GetFromPersistent("key"), key);
  AsyncWorker::WorkComplete();
}

/** READ WORKER **/

ReadWorker::ReadWorker (
    Database *database
  , NanCallback *callback
  , leveldb::Slice key
  , bool asBuffer
  , bool fillCache
  , v8::Local<v8::Object> &keyHandle
) : IOWorker(database, callback, key, keyHandle)
  , asBuffer(asBuffer)
{
  NanScope();

  SavePersistent("key", keyHandle);
};

ReadWorker::~ReadWorker () {
  delete options;
}

void ReadWorker::Execute () {
  database->GetFromDatabase(options, key, value));
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

}
