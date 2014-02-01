
#include <node.h> 
#include "database.h"

namespace bangdown {
  NAN_METHOD(Bang){
    NanScope();

    v8::Local<v8::String> location;
    if (args.Length() != 0 && args[0]->IsString())
      location = args[0].As<v8::String>();
    NanReturnUndefined();
  }

}
