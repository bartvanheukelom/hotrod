#pragma once

#include "common.h"
#include <include/v8.h>
#include <cstdint>


// ================== FROM V8 TO C++ ================== //

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
template<> const int32_t getArg<const int32_t>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
// TODO allow ArrayBuffer for large vals
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
// TODO allow ArrayBuffer for large vals
template<> uint64_t getArg<uint64_t>(ARG) {
    return (uint64_t) v8::Local<v8::Number>::Cast(arg)->Value();
}

template <typename P>
P* abPtr(const v8::Local<v8::Value>& arg) {
    if (arg->IsNumber()) return reinterpret_cast<P*>((size_t) v8::Local<v8::Number>::Cast(arg)->Value());
    if (arg->IsNull()) return nullptr;
    return reinterpret_cast<P*>(v8::Local<v8::ArrayBuffer>::Cast(arg)->GetContents().Data());
}

template<> void* getArg<void*>(ARG) {
    return abPtr<void>(arg);
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
template<> const double* getArg<const double*>(ARG) {
    return abPtr<const double>(arg);
}
template<> const float* getArg<const float*>(ARG) {
    return abPtr<const float>(arg);
}

// char as int8_t should just work but doesn't
// TODO also accept strings (take utf8 val)
template<> char* getArg<char*>(ARG) {
    return abPtr<char>(arg);
}
template<> const char* getArg<const char*>(ARG) {
    return abPtr<const char>(arg);
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
template<> int64_t* getArg<int64_t*>(ARG) {
    return abPtr<int64_t>(arg);
}
template<> uint64_t* getArg<uint64_t*>(ARG) {
    return abPtr<uint64_t>(arg);
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
template<> const int64_t* getArg<const int64_t*>(ARG) {
    return abPtr<const int64_t>(arg);
}
template<> const uint64_t* getArg<const uint64_t*>(ARG) {
    return abPtr<const uint64_t>(arg);
}





// ================== FROM C++ TO V8 ================== //

bool toJsReturn(bool v) { return v; }
double toJsReturn(float v) { return v; }
double toJsReturn(double v) { return v; }
int32_t toJsReturn(int32_t v) { return v; }
uint32_t toJsReturn(uint32_t v) { return v; }
// TODO not safe (return arraybuffer?)
double toJsReturn(int64_t v) { return (double) v; }
double toJsReturn(uint64_t v) { return (double) v; }
