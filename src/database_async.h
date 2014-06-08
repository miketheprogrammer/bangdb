#ifndef BD_DATABASE_ASYNC_H
#define BD_DATABASE_ASYNC_H

#include <vector>
#include <node.h>

#include <bangdb/database.h>
#include "async.h"

namespace bangdb {

class OpenWorker: public AsyncWorker {
  public:
    OpenWorker (
        Database* _db
      , NanCallback *callback
    );

  virtual ~OpenWorker();
  virtual void Execute();
};

class CloseWorker : public AsyncWorker {
public:
  CloseWorker (
      Database *_db
    , NanCallback *callback
  );

  virtual ~CloseWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();
};

class IOWorker    : public AsyncWorker {
public:
  IOWorker (
      Database *_db
    , NanCallback *callback
    , char* key
    , v8::Local<v8::Object> &keyHandle
  );

  virtual ~IOWorker ();
  virtual void WorkComplete ();

protected:
  char* key;
};

class ReadWorker : public IOWorker {
public:
  ReadWorker (
      Database *_db
    , NanCallback *callback
    , char* key
    , bool asBuffer
    , bool fillCache
    , v8::Local<v8::Object> &keyHandle
  );

  virtual ~ReadWorker ();
  virtual void Execute ();
  virtual void HandleOKCallback ();

private:
  bool asBuffer;
  std::string value;
};

class DeleteWorker : public IOWorker {
public:
  DeleteWorker (
      Database *_db
    , NanCallback *callback
    , char* key
    , v8::Local<v8::Object> &keyHandle
  );

  virtual ~DeleteWorker ();
  virtual void Execute ();

};

class WriteWorker : public DeleteWorker {
public:
  WriteWorker (
      Database *_db
    , NanCallback *callback
    , char* key
    , char* val
    , v8::Local<v8::Object> &keyHandle
    , v8::Local<v8::Object> &valueHandle
  );

  virtual ~WriteWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();

private:
  char* val;
};

class TransactionWorker : public AsyncWorker {
public:
  TransactionWorker (
      Database *_db
    , NanCallback *callback
    , void* txn_handle
    , v8::Local<v8::Object> array
  );

  virtual ~TransactionWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();
private:
  void* txn_handle;
  v8::Local<v8::Object> array;
};
} // namespace bangdown

#endif
