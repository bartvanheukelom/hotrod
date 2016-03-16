#include "common.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <cstdint>

#include "include/v8.h"



// checkParams
template <int len>
void checkParams(const v8::FunctionCallbackInfo<Value>& args) {
    if (args.Length() < len) std::cout << "too few args" << std::endl;
}
template<> void checkParams<0>(const v8::FunctionCallbackInfo<Value>& args) {}



// getArg variations
#define ARG const v8::Local<Value>& arg

template <typename T> T getArg(ARG) {
    T::nope(); // only specialized versions should be called
};

template<> double getArg<double>(ARG) {
    return v8::Local<v8::Number>::Cast(arg)->Value();
}
template<> float getArg<float>(ARG) {
    return (float) v8::Local<v8::Number>::Cast(arg)->Value();
}
template<> const double getArg<const double>(ARG) {
    return v8::Local<v8::Number>::Cast(arg)->Value();
}
template<> const float getArg<const float>(ARG) {
    return (float) v8::Local<v8::Number>::Cast(arg)->Value();
}

template<> int8_t getArg<int8_t>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> int16_t getArg<int16_t>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> int32_t getArg<int32_t>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> const int32_t getArg<const GLint>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> int64_t getArg<int64_t>(ARG) {
    return (int64_t) v8::Local<v8::Number>::Cast(arg)->Value();
}

template<> uint8_t getArg<uint8_t>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}
template<> uint16_t getArg<uint16_t>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}
template<> uint32_t getArg<uint32_t>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}
template<> uint64_t getArg<uint64_t>(ARG) {
    return (GLuint64) v8::Local<v8::Number>::Cast(arg)->Value();
}

template <typename P>
P* abPtr(const v8::Local<v8::Value>& arg) {
    if (arg->IsNull()) return nullptr;
    return reinterpret_cast<P*>(v8::Local<v8::ArrayBuffer>::Cast(arg)->GetContents().Data());
}

template<> void* getArg<void*>(ARG) {
    return abPtr<void>(arg);
}
template<> void** getArg<void**>(ARG) {
    return abPtr<void*>(arg);
}
template<> double* getArg<double*>(ARG) {
    return abPtr<double>(arg);
}
template<> float* getArg<float*>(ARG) {
    return abPtr<float>(arg);
}
template<> const void* getArg<const void*>(ARG) {
    return abPtr<const void>(arg);
}
template<> const void* const* getArg<const void* const*>(ARG) {
    return abPtr<const void*>(arg);
}
template<> const void** getArg<const void**>(ARG) {
    return abPtr<const void*>(arg);
}
template<> const double* getArg<const double*>(ARG) {
    return abPtr<const double>(arg);
}
template<> const float* getArg<const float*>(ARG) {
    return abPtr<const float>(arg);
}
// struct is arraybuffer
template<> __GLsync* getArg<__GLsync*>(ARG) {
    return abPtr<__GLsync>(arg);
}
// char though int8_t should just work
template<> char* getArg<char*>(ARG) {
    return abPtr<char>(arg);
}
template<> const char* getArg<const char*>(ARG) {
    return abPtr<const char>(arg);
}
template<> const char** getArg<const char**>(ARG) {
    return abPtr<const char*>(arg);
}
template<> const char* const* getArg<const char* const*>(ARG) {
    return abPtr<const char*>(arg);
}
// end char
template<> int8_t* getArg<int8_t*>(ARG) {
    return abPtr<int8_t>(arg);
}
template<> uint8_t* getArg<uint8_t*>(ARG) {
    return abPtr<uint8_t>(arg);
}
template<> int16_t* getArg<int16_t*>(ARG) {
    return abPtr<int16_t>(arg);
}
template<> uint16_t* getArg<uint16_t*>(ARG) {
    return abPtr<uint16_t>(arg);
}
template<> int32_t* getArg<int32_t*>(ARG) {
    return abPtr<int32_t>(arg);
}
template<> uint32_t* getArg<uint32_t*>(ARG) {
    return abPtr<uint32_t>(arg);
}
template<> const int8_t* getArg<const int8_t*>(ARG) {
    return abPtr<const int8_t>(arg);
}
template<> const int8_t** getArg<const int8_t**>(ARG) {
    return abPtr<const int8_t*>(arg);
}
template<> const int8_t* const* getArg<const int8_t* const*>(ARG) {
    return abPtr<const int8_t*>(arg);
}
template<> const uint8_t* getArg<const uint8_t*>(ARG) {
    return abPtr<const uint8_t>(arg);
}
template<> const uint8_t** getArg<const uint8_t**>(ARG) {
    return abPtr<const uint8_t*>(arg);
}
template<> const int16_t* getArg<const int16_t*>(ARG) {
    return abPtr<const int16_t>(arg);
}
template<> const uint16_t* getArg<const uint16_t*>(ARG) {
    return abPtr<const uint16_t>(arg);
}
template<> const int32_t* getArg<const int32_t*>(ARG) {
    return abPtr<const int32_t>(arg);
}
template<> const uint32_t* getArg<const uint32_t*>(ARG) {
    return abPtr<const uint32_t>(arg);
}
template<> const int64_t* getArg<const int64_t*>(ARG) {
    return abPtr<const int64_t>(arg);
}
template<> const uint64_t* getArg<const uint64_t*>(ARG) {
    return abPtr<const uint64_t>(arg);
}


// these macros are used in the generated file
#define FUNCTION_SIGNATURE(NAME) void js_ ## NAME (const FunctionCallbackInfo<Value>& args)
#define FUNCTION_BODY_START(LEN) checkParams<LEN>(args);

// TODO
#define DECLARE_FUNCTIONS_START void gl_setUpContext(v8::Local<v8::Context> ctx) {
#define DECLARE_FUNCTION(NAME) ctx->Global()->Set(ctx, jsString(#NAME), v8::Function::New(ctx, &js_ ## NAME).ToLocalChecked());
// #define DEFINE_CONSTANT(NAME) ctx->Global()->Set(ctx, jsString(#NAME), v8::Number::New(theOneIsolate, NAME));
#define DECLARE_FUNCTIONS_END }

// include the generated file
#include "gl.cpp.incl"
