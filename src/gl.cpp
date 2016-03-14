#include "common.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
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

template<> GLdouble getArg<GLdouble>(ARG) {
    return v8::Local<v8::Number>::Cast(arg)->Value();
}
template<> GLfloat getArg<GLfloat>(ARG) {
    return (GLfloat) v8::Local<v8::Number>::Cast(arg)->Value();
}

template<> GLbyte getArg<GLbyte>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> GLshort getArg<GLshort>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> GLint getArg<GLint>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}

template<> GLubyte getArg<GLubyte>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}
template<> GLushort getArg<GLushort>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}
template<> GLuint getArg<GLuint>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}

template <typename P>
P* abPtr(const v8::Local<v8::Value>& arg) {
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
template<> const double* getArg<const double*>(ARG) {
    return abPtr<const double>(arg);
}
template<> const float* getArg<const float*>(ARG) {
    return abPtr<const float>(arg);
}
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
template<> const uint8_t* getArg<const uint8_t*>(ARG) {
    return abPtr<const uint8_t>(arg);
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
