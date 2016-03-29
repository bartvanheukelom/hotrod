#include "common.h"
#include "jsconvert.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <cstdint>

#include <include/v8.h>



// checkParams
template <int len>
void checkParams(const v8::FunctionCallbackInfo<Value>& args) {
    if (args.Length() < len) std::cout << "too few args" << std::endl;
}
template<> void checkParams<0>(const v8::FunctionCallbackInfo<Value>& args) {}

// getArg specializations
// struct is arraybuffer
// template<> __GLsync* getArg<__GLsync*>(ARG) {
//     return abPtr<__GLsync>(arg);
// }

void gl_setUpContextIncl(v8::Local<v8::Context> ctx);
void gl_setUpContext(v8::Local<v8::Context> ctx) {
    gl_setUpContextIncl(ctx);
}

// these macros are used in the generated file
#define FUNCTION_SIGNATURE(NAME) void js_ ## NAME (const FunctionCallbackInfo<Value>& args)
#define FUNCTION_BODY_START(LEN) checkParams<LEN>(args);
#define FUNCTION_BODY_RETURN(R) args.GetReturnValue().Set(toJsReturn(R));

#define DECLARE_FUNCTIONS_START void gl_setUpContextIncl(v8::Local<v8::Context> ctx) {
#define DECLARE_FUNCTION(NAME) ctx->Global()->Set(ctx, jsString(#NAME), v8::Function::New(ctx, &js_ ## NAME).ToLocalChecked());
#define DECLARE_FUNCTIONS_END }

// include the generated file
#include "../bin/gen/gl.cpp.incl"
