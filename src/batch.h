#ifndef BD_BATCH_H
#define BD_BATCH_H

#include <vector>
#include <node.h>
#include <bangdb/database.h>
#include "database.h"

namespace bangdb {

class Batch : public node::ObjectWrap {
public:
  static void Init();
  static v8::Handle<v8::Value> NewInstance (
      v8::Handle<v8::Object> database
  );

  Batch (bangdb::Database* db);
  ~Batch ();

  long Write();

private:
  bangdb::Database* db;
  void* txn_handle; // Holds our transaction state.
  bool hasData; // keep track of whether we're writing data or not
  bool written;

  static NAN_METHOD(New);
  static NAN_METHOD(Put);
  static NAN_METHOD(Del);
  static NAN_METHOD(Clear);
  static NAN_METHOD(Write);
};

}

#endif