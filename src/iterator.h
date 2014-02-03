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
      v8::Local<v8::Object> db
    , v8::Local<v8::String> skey
    , v8::Local<v8::String> ekey
  );

  Iterator (
      Database* db
    , v8::Local<v8::String> skey
    , v8::Local<v8::String> ekey
  );

  ~Iterator();

  bool IteratorNext(std::string& key, std::string& value); // Has Next; move iter; out key:val
  bool IteratorPeekNext(); // Is there more data coming
  bool IteratorClose();
  bool IteratorEnd();
  resultset* NewIterator(char* skey, char* ekey);
  
private:
    Database* db;
    resultset* rs;
    v8::Local<v8::String> skey;
    v8::Local<v8::String> ekey;

    v8::Persistent<v8::Object> persistentHandle;

    bool GetIterator();

    static NAN_METHOD(New);
    static NAN_METHOD(Next);
    static NAN_METHOD(Peek);
    static NAN_METHOD(End);
};
}


#endif
