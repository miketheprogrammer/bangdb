#ifndef BD_DATABASE_H
#define BD_DATABASE_H

#include <map>
#include <vector>
#include <node.h>
#include <string>
#include <bangdb/database.h>
#include "nan.h"
#include "iterator.h"

namespace bangdb {

NAN_METHOD(Bang);

class Database : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Value> NewInstance (v8::Local<v8::String> &location);

  //Accessor Methods
  database* OpenDatabase(char* name);
  int CloseDatabase();

  // OpenTable is what we are considering the actualy "database"
  int OpenTable(char* name);
  int CloseTable(char* name);

  resultset* NewIterator (char* skey, char* ekey);
  // For now we dont expose the underlying connection layer.
  // We will consider 1 connection per table for now.
  // Later maybe use a connection pool if needed.
  // Since nodejs is primarily singlethreaded we dont need to worry.

  int PutValue(char* key, char* val);
  FDT* GetValue(char* key, std::string& value);

  const char* Name() const;

  Database (char* name);
  ~Database ();

private:
  database* bangdb; // pointer to db file
  table* bangtable; // 
  connection* bangconnection;
  char* name;

  // Public API Methods
  static NAN_METHOD(New);  // Initlizes. Private;
  static NAN_METHOD(Put); //PutValue
  static NAN_METHOD(Get); // GetValue

  static NAN_METHOD(Open); // OpenTable
  static NAN_METHOD(Close); // CloseTable
  
  static NAN_METHOD(Iterator);
  
  // Free is a pointer to CloseDatabase
  static NAN_METHOD(Free);

};

} 

#endif
