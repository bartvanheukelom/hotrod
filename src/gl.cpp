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

template<> void* getArg<void*>(ARG) {
    return v8::Local<v8::ArrayBuffer>::Cast(arg)->GetContents().Data();
}
template<> void** getArg<void**>(ARG) {
    return reinterpret_cast<void**>(
        v8::Local<v8::ArrayBuffer>::Cast(arg)->GetContents().Data()
    );
}
template<> double* getArg<double*>(ARG) {
    return reinterpret_cast<double*>(
        v8::Local<v8::Float64Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> float* getArg<float*>(ARG) {
    return reinterpret_cast<float*>(
        v8::Local<v8::Float32Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const void* getArg<const void*>(ARG) {
    return v8::Local<v8::ArrayBuffer>::Cast(arg)->GetContents().Data();
}
template<> const double* getArg<const double*>(ARG) {
    return reinterpret_cast<const double*>(
        v8::Local<v8::Float64Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const float* getArg<const float*>(ARG) {
    return reinterpret_cast<const float*>(
        v8::Local<v8::Float32Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}


template<> int8_t* getArg<int8_t*>(ARG) {
    return reinterpret_cast<int8_t*>(
        v8::Local<v8::Int8Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> uint8_t* getArg<uint8_t*>(ARG) {
    return reinterpret_cast<uint8_t*>(
        v8::Local<v8::Uint8Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> int16_t* getArg<int16_t*>(ARG) {
    return reinterpret_cast<int16_t*>(
        v8::Local<v8::Int16Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> uint16_t* getArg<uint16_t*>(ARG) {
    return reinterpret_cast<uint16_t*>(
        v8::Local<v8::Uint16Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> int32_t* getArg<int32_t*>(ARG) {
    return reinterpret_cast<int32_t*>(
        v8::Local<v8::Int32Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> uint32_t* getArg<uint32_t*>(ARG) {
    return reinterpret_cast<uint32_t*>(
        v8::Local<v8::Uint32Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const int8_t* getArg<const int8_t*>(ARG) {
    return reinterpret_cast<const int8_t*>(
        v8::Local<v8::Int8Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const uint8_t* getArg<const uint8_t*>(ARG) {
    return reinterpret_cast<const uint8_t*>(
        v8::Local<v8::Uint8Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const int16_t* getArg<const int16_t*>(ARG) {
    return reinterpret_cast<const int16_t*>(
        v8::Local<v8::Int16Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const uint16_t* getArg<const uint16_t*>(ARG) {
    return reinterpret_cast<const uint16_t*>(
        v8::Local<v8::Uint16Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const int32_t* getArg<const int32_t*>(ARG) {
    return reinterpret_cast<const int32_t*>(
        v8::Local<v8::Int32Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
}
template<> const uint32_t* getArg<const uint32_t*>(ARG) {
    return reinterpret_cast<const uint32_t*>(
        v8::Local<v8::Uint32Array>::Cast(arg)->Buffer()->GetContents().Data()
    );
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
