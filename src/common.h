#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <fstream>
#include <cerrno>
#include <stdexcept>

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

using namespace std;
using namespace v8;

extern Isolate* theOneIsolate;
extern Persistent<ObjectTemplate> jsPointerTpl;

Local<String> jsString(const string& src);
string fromJsString(const Handle<Value>& src);

void bullet_setUpContext(Local<Context> ctx);
