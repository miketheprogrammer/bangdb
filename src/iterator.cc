#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "iterator.h"

namespace bangdown {

v8::Persistent<v8::FunctionTemplate> iterator_constructor;

Iterator::Iterator (
    Database* db
  , v8::Local<v8::String> skey
  , v8::Local<v8::String> ekey
) : db(db)
  , skey(skey)
  , ekey(ekey)
{
  NanScope();

  v8::Local<v8::Object> obj = v8::Object::New();

  NanAssignPersistent(v8::Object, persistentHandle, obj);

  rs = db->NewIterator(
      NanFromV8String(skey.As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS), 
      NanFromV8String(ekey.As<v8::Object>(), Nan::UTF8, NULL, NULL, 0, v8::String::NO_OPTIONS)
  );

}

Iterator::~Iterator (
  ) {

};


bool Iterator::IteratorNext (std::string& key, std::string& value) {
  if(rs->hasNext()) {
    key.assign((char* )rs->getNextKey()->data, rs->getNextKey()->length);
    value.assign((char* )rs->getNextVal()->data, rs->getNextVal()->length);
  }

  return true;
}

bool Iterator::IteratorPeekNext() {
  return rs->moreDataToCome();
}

bool Iterator::IteratorEnd() {
  if (rs != NULL) {
    rs->clear();
    delete rs;
    return true;
  }
  return false;
}
void Iterator::Init () {
  v8::Local<v8::FunctionTemplate> tpl =
      v8::FunctionTemplate::New(Iterator::New);
  NanAssignPersistent(v8::FunctionTemplate, iterator_constructor, tpl);
  tpl->SetClassName(NanSymbol("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Iterator::Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "peek", Iterator::Peek);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "end", Iterator::End);
}

v8::Local<v8::Object> Iterator::NewInstance (
        v8::Local<v8::Object> db
      , v8::Local<v8::String> skey
      , v8::Local<v8::String> ekey
    ) {

  NanScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanPersistentToLocal(iterator_constructor);

  v8::Handle<v8::Value> argv[3] = { db, skey, ekey };
  instance = constructorHandle->GetFunction()->NewInstance(3, argv);

  return instance;
}
NAN_METHOD(Iterator::New) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  v8::Local<v8::String> skey = args[1].As<v8::String>();
  v8::Local<v8::String> ekey = args[2].As<v8::String>();

  Iterator* iterator = new Iterator(
      database
    , skey
    , ekey
  );
  iterator->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(Iterator::Next) {
  NanScope();
  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  std::string key;
  std::string value;

  iterator->IteratorNext(key, value);

  NanReturnValue(v8::String::New(value.c_str()));
}

NAN_METHOD(Iterator::Peek) {
  NanScope();
  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  NanReturnValue(v8::Boolean::New(iterator->IteratorPeekNext()));

}
}
