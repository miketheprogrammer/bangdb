/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef BD_BATCH_ASYNC_H
#define BD_BATCH_ASYNC_H

#include <node.h>

#include "nan.h"
#include "async.h"
#include "batch.h"
#include "database.h"
#include <bangdb/database.h>

namespace bangdb {

class BatchWriteWorker : public AsyncWorker {
public:
  BatchWriteWorker (
      Batch* batch
    , NanCallback *callback
  );

  virtual ~BatchWriteWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();
private:
  Batch* batch;
};

} // namespace leveldown

#endif