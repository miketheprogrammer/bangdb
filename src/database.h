#ifndef BD_DATABASE_H
#define BD_DATABASE_H

#include <map>
#include <vector>
#include <node.h>
#include <bangdb/database.h>
#include "nan.h"


namespace bangdown {

NAN_METHOD(Bang);


struct Reference {
  v8::Persistent<v8::Object> handle;
  leveldb::Slice slice;

  Reference(v8::Local<v8::Value> obj, leveldb::Slice slice) : slice(slice) {
    v8::Local<v8::Object> _obj = v8::Object::New();
    _obj->Set(NanSymbol("obj"), obj);
    NanAssignPersistent(v8::Object, handle, _obj);
  };
};

static inline void ClearReferences (std::vector<Reference *> *references) {
  for (std::vector<Reference *>::iterator it = references->begin()
      ; it != references->end()
      ; ) {
    DisposeStringOrBufferFromSlice((*it)->handle, (*it)->slice);
    it = references->erase(it);
  }
  delete references;
}

class Database : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Value> NewInstance (v8::Local<v8::String> &location);

  database* OpenDatabase(std::string name);

  Database (char* name);
  ~Database ();

private:
  database* db;
  char* name;

  static NAN_METHOD(New);
};

} // namespace leveldown


}

#endif
