#include <node.h>
#include <node_buffer.h>

#include <bangdb/database.h>
#include "nan.h"
#include "database.h"
#include "batch.h"

namespace bangdb {

static v8::Persistent<v8::FunctionTemplate> batch_constructor;

Batch::Batch (bangdb::Database* db) : db(db) {
  txn_handle = db->GetDatabase()->begin_transaction();
  hasData = false;
  written = false;
}
Batch::~Batch () {}

long Batch::Write() {
  return db->GetDatabase()->commit_transaction(&txn_handle);
}

void Batch::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Batch::New);
  NanAssignPersistent(v8::FunctionTemplate, batch_constructor, tpl);
  tpl->SetClassName(NanSymbol("Batch"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", Batch::Put);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Batch::Del);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Batch::Clear);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Batch::Write);
}


NAN_METHOD(Batch::New) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  Batch* batch = new Batch(database);
  batch->Wrap(args.This());

  NanReturnValue(args.This());
}

v8::Handle<v8::Value> Batch::NewInstance (
        v8::Handle<v8::Object> database
    ) {

  NanScope();

  v8::Local<v8::Object> instance;

  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanPersistentToLocal(batch_constructor);

  v8::Handle<v8::Value> argv[1] = { database };
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);

  return scope.Close(instance);
}

NAN_METHOD(Batch::Put) {
  NanScope();

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  if (batch->written)
    return NanThrowError("write() already called on this batch");

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();
  v8::Local<v8::Object> valueHandle = args[1].As<v8::Object>();
  char* key = NanFromV8String(keyHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
  char* val = NanFromV8String(valueHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  batch->db->PutValue(key, val, batch->txn_handle);

  if (!batch->hasData)
    batch->hasData = true;


  NanReturnValue(args.Holder());
}

NAN_METHOD(Batch::Del) {
  NanScope();

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  if (batch->written)
    return NanThrowError("write() already called on this batch");

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();
  v8::Local<v8::Object> valueHandle = args[1].As<v8::Object>();
  char* key = NanFromV8String(keyHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);
  char* val = NanFromV8String(valueHandle, Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS);

  batch->db->DeleteValue(key, batch->txn_handle);

  if (!batch->hasData)
    batch->hasData = true;

  NanReturnValue(args.Holder());
}

NAN_METHOD(Batch::Clear) {
  NanScope();

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  if (batch->written)
    return NanThrowError("write() already called on this batch");

  batch->db->GetDatabase()->abort_transaction(&batch->txn_handle);
  batch->hasData = false;

  NanReturnValue(args.Holder());
}

NAN_METHOD(Batch::Write) {
  NanScope();

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  if (batch->written)
    return NanThrowError("write() already called on this batch");
  
  //if (args.Length() == 0)
  //  return NanThrowError("write() requires a callback argument");

  batch->written = true;

  if (batch->hasData) {
    batch->Write();
  } else {
  }

  NanReturnUndefined();
}

}