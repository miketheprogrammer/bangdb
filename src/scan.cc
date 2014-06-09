#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "scan.h"

namespace bangdb {

v8::Persistent<v8::FunctionTemplate> scan_constructor;

Scan::Scan (
    Database* db
  , v8::Local<v8::String> skey
  , v8::Local<v8::String> ekey
  , v8::Local<v8::Object> optionsObj
) : db(db)
  , skey(skey)
  , ekey(ekey)
  , optionsObj(optionsObj)
{
  NanScope();

  v8::Local<v8::Object> obj = v8::Object::New();

  NanAssignPersistent(persistentHandle, obj);

  size_t sz_skey;
  size_t sz_ekey;

  c_skey = NanCString(skey, &sz_skey);
  c_ekey = NanCString(ekey, &sz_ekey);
  rs = db->NewScan(c_skey, c_ekey, optionsObj);

}

Scan::~Scan (
  ) {

};


bool Scan::ScanNext (std::string& key, std::string& value) {
  if(rs->hasNext()) {
    key.assign((char* )rs->getNextKey()->data, rs->getNextKey()->length);
    value.assign((char* )rs->getNextVal()->data, rs->getNextVal()->length);
    rs->moveNext();
    return true;    
  } else {
    if (rs->moreDataToCome()) {
      char* sk = (char*) rs->lastEvaluatedKey()->data;
      char* ek = c_ekey;
      rs->clear();
      delete rs;

      //v8::Local<v8::Value> newVal = NanNew<v8::Value>(est);
      optionsObj->Set(NanNew<v8::String>("skey_op"), v8::Number::New(1));
      rs = db->NewScan(sk, ek, optionsObj);
      rs->moveNext();
      if (rs->hasNext()) {
        key.assign((char* )rs->getNextKey()->data, rs->getNextKey()->length);
        value.assign((char* )rs->getNextVal()->data, rs->getNextVal()->length);
        rs->moveNext();
        return true;
      } 
      
      return false;
    } else {
    }
    return false;
  }
}
bool Scan::ScanHasNext() {
  if(rs->hasNext()) {
    return true;
  }
  if(rs->moreDataToCome()){
    return true;
  }

  return false;
}

bool Scan::ScanPeekNext() {
  return rs->moreDataToCome();
}

bool Scan::ScanClose() {
  if (rs != NULL) {
    rs->clear();
    delete rs;
    return true;
  }
  return false;
}
void Scan::Init () {
  v8::Local<v8::FunctionTemplate> tpl =
      v8::FunctionTemplate::New(Scan::New);
  NanAssignPersistent(scan_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("Scan"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Scan::Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peek", Scan::Peek);
  NODE_SET_PROTOTYPE_METHOD(tpl, "hasNext", Scan::HasNext);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "end", Scan::End);
}

v8::Local<v8::Object> Scan::NewInstance (
        v8::Local<v8::Object> db
      , v8::Local<v8::String> skey
      , v8::Local<v8::String> ekey
      , v8::Local<v8::Object> optionsObj
    ) {

  NanScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanNew<v8::FunctionTemplate>(scan_constructor);

  v8::Handle<v8::Value> argv[4] = { db, skey, ekey, optionsObj };
  instance = constructorHandle->GetFunction()->NewInstance(4, argv);

  return instance;
}
NAN_METHOD(Scan::New) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  v8::Local<v8::String> skey = args[1].As<v8::String>();
  v8::Local<v8::String> ekey = args[2].As<v8::String>();
  v8::Local<v8::Object> optionsObj = args[3].As<v8::Object>();
  Scan* scan = new Scan(
      database
    , skey
    , ekey
    , optionsObj
  );
  scan->Wrap(args.This());

  NanReturnValue(args.This());
}
NAN_METHOD(Scan::HasNext){
  NanScope();
  Scan* scan = node::ObjectWrap::Unwrap<Scan>(args.This());
  NanReturnValue(v8::Boolean::New(scan->ScanHasNext()));
}

NAN_METHOD(Scan::Next) {
  NanScope();
  Scan* scan = node::ObjectWrap::Unwrap<Scan>(args.This());

  std::string key;
  std::string value;

  scan->ScanNext(key, value);

  NanReturnValue(v8::String::New(value.c_str()));
}

NAN_METHOD(Scan::Peek) {
  NanScope();
  Scan* scan = node::ObjectWrap::Unwrap<Scan>(args.This());

  NanReturnValue(v8::Boolean::New(scan->ScanPeekNext()));

}
}
