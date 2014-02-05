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
        Database* database
      , NanCallback *callback
    );

  virtual ~OpenWorker();
  virtual void Execute();
}

class CloseWorker : public AsyncWorker {
public:
  CloseWorker (
      Database *database
    , NanCallback *callback
  );

  virtual ~CloseWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();
};

class IOWorker    : public AsyncWorker {
public:
  IOWorker (
      Database *database
    , NanCallback *callback
    , v8::Local<v8::Object> &keyHandle
  );

  virtual ~IOWorker ();
  virtual void WorkComplete ();

};

class ReadWorker : public IOWorker {
public:
  ReadWorker (
      Database *database
    , NanCallback *callback
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
      Database *database
    , NanCallback *callback
    , leveldb::Slice key
    , bool sync
    , v8::Local<v8::Object> &keyHandle
  );

  virtual ~DeleteWorker ();
  virtual void Execute ();

protected:
  leveldb::WriteOptions* options;
};

class WriteWorker : public DeleteWorker {
public:
  WriteWorker (
      Database *database
    , NanCallback *callback
    , bool sync
    , v8::Local<v8::Object> &keyHandle
    , v8::Local<v8::Object> &valueHandle
  );

  virtual ~WriteWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();

};

} // namespace bangdown

#endif
