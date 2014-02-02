#ifndef BD_ITERATOR_H
#define BD_ITERATOR_H

#include <node.h>
#include "nan.h"
#include "bangdown.h"
#include "database.h"

namespace bangdown {

class Database;

class Iterator : public node::ObjectWrap {

public:
  static void Init();
  static v8::Local<v8::Object> NewInstance (
      v8::Local<v8::Object> db;
    , char* skey
    , char* ekey
  );

  Iterator (
      Database* db
    , char* skey
    , char* ekey
  );

  ~Iterator();

  bool IteratorNext(std::string& key, std::string& value); // Has Next; move iter; out key:val
  bool IteratorPeekNext(); // Is there more data coming
  bool IteratorClose();
  resultset* NewIterator(char* skey, char* ekey);
  
private:
    Database* db;
    resultset* rs;
    char* skey;
    char* ekey;

    v8::Persistent<v8::Object> persistentHandle;

    bool GetIterator();

    static NAN_METHOD(New);
    static NAN_METHOD(Next);
    static NAN_METHOD(Peek);
    static NAN_METHOD(End);
};
}