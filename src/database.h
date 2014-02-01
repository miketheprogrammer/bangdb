#ifndef BD_DATABASE_H
#define BD_DATABASE_H

#include <map>
#include <vector>
#include <node.h>
#include <bangdb/database.h>
#include "nan.h"


namespace bangdown {

NAN_METHOD(Bang);

class Database : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Value> NewInstance (v8::Local<v8::String> &location);

  database* OpenDatabase(char* name);

  Database (char* name);
  ~Database ();

private:
  database* db;
  char* name;

  static NAN_METHOD(New);
  static NAN_METHOD(Put);
  static NAN_METHOD(Get);

};

} 

#endif
