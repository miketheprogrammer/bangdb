#include <node.h>
#include <node_buffer.h>

#include <bangdb/database.h>
#include "nan.h"
#include "database.h"
#include "transaction.h"
#include "transaction_async.h"

namespace bangdb {

static v8::Persistent<v8::FunctionTemplate> transaction_constructor;

Transaction::Transaction (bangdb::Database* db) : db(db) {
  txn_handle = db->GetDatabase()->begin_transaction();
  hasData = false;
  written = false;
}
Transaction::~Transaction () {}

long Transaction::Commit() {
  return db->GetDatabase()->commit_transaction(&txn_handle);
}

void Transaction::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Transaction::New);
  NanAssignPersistent(transaction_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("Transaction"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", Transaction::Put);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Transaction::Del);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Transaction::Clear);
  NODE_SET_PROTOTYPE_METHOD(tpl, "commit", Transaction::Commit);
}


NAN_METHOD(Transaction::New) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  Transaction* transaction = new Transaction(database);
  transaction->Wrap(args.This());

  NanReturnValue(args.This());
}

v8::Handle<v8::Value> Transaction::NewInstance (
        v8::Handle<v8::Object> database
    ) {

  NanScope();

  v8::Local<v8::Object> instance;

  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanNew<v8::FunctionTemplate>(transaction_constructor);

  v8::Handle<v8::Value> argv[1] = { database };
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);

  return scope.Close(instance);
}

NAN_METHOD(Transaction::Put) {
  NanScope();

  Transaction* transaction = ObjectWrap::Unwrap<Transaction>(args.Holder());

  if (transaction->written)
    return NanThrowError("commit() already called on this transaction");

  size_t sz_keyHandle;
  size_t sz_valueHandle;

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();
  v8::Local<v8::Object> valueHandle = args[1].As<v8::Object>();
  char* key = NanCString(keyHandle, &sz_keyHandle);
  char* val = NanCString(valueHandle, &sz_valueHandle);

  transaction->db->PutValue(key, val, transaction->txn_handle);

  if (!transaction->hasData)
    transaction->hasData = true;


  NanReturnValue(args.Holder());
}

NAN_METHOD(Transaction::Del) {
  NanScope();

  Transaction* transaction = ObjectWrap::Unwrap<Transaction>(args.Holder());

  if (transaction->written)
    return NanThrowError("commit() already called on this transaction");

  v8::Local<v8::Object> keyHandle = args[0].As<v8::Object>();

  size_t sz_key;
  char* key = NanCString(keyHandle, &sz_key);

  transaction->db->DeleteValue(key, transaction->txn_handle);

  if (!transaction->hasData)
    transaction->hasData = true;

  NanReturnValue(args.Holder());
}

NAN_METHOD(Transaction::Clear) {
  NanScope();

  Transaction* transaction = ObjectWrap::Unwrap<Transaction>(args.Holder());

  if (transaction->written)
    return NanThrowError("commit() already called on this transaction");

  transaction->db->GetDatabase()->abort_transaction(&transaction->txn_handle);
  transaction->hasData = false;

  NanReturnValue(args.Holder());
}

NAN_METHOD(Transaction::Commit) {
  NanScope();

  Transaction* transaction = ObjectWrap::Unwrap<Transaction>(args.Holder());

  if (transaction->written)
    return NanThrowError("commit() already called on this transaction");
  

  transaction->written = true;
  if (transaction->hasData) {
    if (args.Length() == 0) {
      transaction->Commit();
    } else {
      TransactionWriteWorker* worker  = new TransactionWriteWorker(transaction, NULL);
      v8::Local<v8::Object> _this = args.This();
      worker->SaveToPersistent("transaction", _this);
      NanAsyncQueueWorker(worker);
    }
  } else {
  }

  NanReturnUndefined();
}

}