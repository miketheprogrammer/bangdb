/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef BD_TRANSACTION_ASYNC_H
#define BD_TRANSACTION_ASYNC_H

#include <node.h>

#include "nan.h"
#include "async.h"
#include "transaction.h"
#include "database.h"
#include <bangdb/database.h>

namespace bangdb {

class TransactionWriteWorker : public AsyncWorker {
public:
  TransactionWriteWorker (
      Transaction* transaction
    , NanCallback *callback
  );

  virtual ~TransactionWriteWorker ();
  virtual void Execute ();
  virtual void WorkComplete ();
private:
  Transaction* transaction;
};

} // namespace leveldown

#endif
