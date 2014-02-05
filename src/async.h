/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef BD_ASYNC_H
#define BD_ASYNC_H

#include <node.h>
#include "nan.h"
#include "database.h"
#include <bangdb/database.h>
namespace bangdb {

class Database;

/* abstract */ class AsyncWorker : public NanAsyncWorker {
public:
  AsyncWorker (
      bangdb::Database* database
    , NanCallback *callback
  ) : NanAsyncWorker(callback), database(database) {
    NanScope();
    v8::Local<v8::Object> obj = v8::Object::New();
    NanAssignPersistent(v8::Object, persistentHandle, obj);
  }

protected:
  Database* database;
};

} // namespace leveldown

#endif

