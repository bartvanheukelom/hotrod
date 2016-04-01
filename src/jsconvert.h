#pragma once

#include "common.h"
#include <include/v8.h>
#include <cstdint>


template <typename P>
P* abPtr(const v8::Local<v8::Value>& arg) {
    if (arg->IsArrayBuffer())
        return reinterpret_cast<P*>(
            v8::Local<v8::ArrayBuffer>::Cast(arg)->GetContents().Data()
        );
    if (arg->IsArrayBufferView())
        return reinterpret_cast<P*>(
            v8::Local<v8::ArrayBufferView>::Cast(arg)->Buffer()->GetContents().Data()
        );
    if (arg->IsObject())
        return reinterpret_cast<P*>(
            v8::Local<v8::Object>::Cast(arg)->GetAlignedPointerFromInternalField(0)
        );
    if (arg->IsNull())
        return nullptr;
    // for offsets
    if (arg->IsNumber())
        return reinterpret_cast<P*>((size_t) v8::Local<v8::Number>::Cast(arg)->Value());
    // TODO throw
    return nullptr;
}



// ================== FROM V8 TO C++ ================== //

// checkParams
template <int len>
void checkParams(const v8::FunctionCallbackInfo<Value>& args) {
    if (args.Length() < len) std::cout << "too few args" << std::endl;
}
template<> void checkParams<0>(const v8::FunctionCallbackInfo<Value>& args) {}


// getArg variations
#define ARG const v8::Local<Value>& arg

template <typename T>
T* getPtrArg(ARG) {
    return abPtr<T>(arg);
};

template <typename T>
T getNumArg(ARG) {
    T::nope(); // only specialized versions should be called
};

template<> double getNumArg<double>(ARG) {
    return v8::Local<v8::Number>::Cast(arg)->Value();
}
template<> float getNumArg<float>(ARG) {
    return (float) v8::Local<v8::Number>::Cast(arg)->Value();
}

template<> int8_t getNumArg<int8_t>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> int16_t getNumArg<int16_t>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}
template<> int32_t getNumArg<int32_t>(ARG) {
    return v8::Local<v8::Int32>::Cast(arg)->Value();
}

// TODO allow ArrayBuffer for large vals
template<> int64_t getNumArg<int64_t>(ARG) {
    return (int64_t) v8::Local<v8::Number>::Cast(arg)->Value();
}

template<> uint8_t getNumArg<uint8_t>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}
template<> uint16_t getNumArg<uint16_t>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}
template<> uint32_t getNumArg<uint32_t>(ARG) {
    return v8::Local<v8::Uint32>::Cast(arg)->Value();
}

// TODO allow ArrayBuffer for large vals
template<> uint64_t getNumArg<uint64_t>(ARG) {
    return (uint64_t) v8::Local<v8::Number>::Cast(arg)->Value();
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
