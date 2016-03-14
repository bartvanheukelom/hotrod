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
extern Local<Context>* theContext;
extern Persistent<ObjectTemplate> jsPointerTpl;

void eval(const Local<Context>& ctx, const string& js);
Local<String> jsString(const string& src);
string fromJsString(const Handle<Value>& src);
Local<Object> createPointerObject(void* ptr);

template <typename T>
T* getPointerFromObject(Local<Object> obj) {
	return static_cast<T*>(obj->GetAlignedPointerFromInternalField(0));
}

void bullet_setUpContext(Local<Context> ctx);
void graphics_setUpContext(Local<Context> ctx);
void gl_setUpContext(Local<Context> ctx);
