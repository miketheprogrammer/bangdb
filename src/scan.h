#ifndef BD_ITERATOR_H
#define BD_ITERATOR_H

#include <node.h>
#include "nan.h"
#include "bangdb.h"
#include "database.h"
namespace bangdb {

class Database;

class Scan : public node::ObjectWrap {

public:
  static void Init();
  static v8::Local<v8::Object> NewInstance (
      v8::Local<v8::Object> db
    , v8::Local<v8::String> skey
    , v8::Local<v8::String> ekey
  );

  Scan (
      Database* db
    , v8::Local<v8::String> skey
    , v8::Local<v8::String> ekey
  );

  ~Scan();

  bool ScanNext(std::string& key, std::string& value); // Has Next; move iter; out key:val
  bool ScanPeekNext(); // Is there more data coming
  bool ScanHasNext();
  bool ScanClose();
  bool ScanEnd();
  resultset* NewScan(char* skey, char* ekey);
  
  bool keyAsBuffer;
  bool valueAsBuffer;
  bool nexting;
  bool ended;
private:
    Database* db;
    resultset* rs;
    v8::Local<v8::String> skey;
    v8::Local<v8::String> ekey;
    char* c_skey;
    char* c_ekey;
    v8::Persistent<v8::Object> persistentHandle;

    bool GetScan();

    static NAN_METHOD(New);
    static NAN_METHOD(Next);
    static NAN_METHOD(Peek);
    static NAN_METHOD(Close);
    static NAN_METHOD(HasNext);
};
}


#endif
