#include "async.h"
#include "transaction.h"
#include "transaction_async.h"

namespace bangdb {

/** NEXT WORKER **/

TransactionWriteWorker::TransactionWriteWorker (
    Transaction* transaction
  , NanCallback *callback
) : AsyncWorker(NULL, callback)
  , transaction(transaction)
{};

TransactionWriteWorker::~TransactionWriteWorker () {}

void TransactionWriteWorker::Execute () {
  transaction->Commit();
}
void TransactionWriteWorker::WorkComplete () {
  NanScope();
  HandleOKCallback();
  delete callback;
  callback = NULL;
}
} // namespace leveldown
