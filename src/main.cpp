#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <fstream>
#include <cerrno>
#include <stdexcept>

#include "common.h"

using namespace std;
using namespace v8;

Isolate* theOneIsolate;
Persistent<ObjectTemplate> jsPointerTpl;
Local<Context>* theContext;

class ArrayBufferAllocator : public ArrayBuffer::Allocator {
public:
    virtual void* Allocate(size_t length) {
        void* data = AllocateUninitialized(length);
        return data == nullptr ? data : memset(data, 0, length);
    }
    virtual void* AllocateUninitialized(size_t length) {
        return new char[length];
    }
    virtual void Free(void* data, size_t) {
        delete[] reinterpret_cast<char*>(data);
    }
};


string getFileContents(string filename) {
    ifstream in(filename, ios::in | ios::binary);
    if (in.fail()) throw runtime_error("ifstream error " + errno);

    string contents;
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], static_cast<streamsize>(contents.size()));
    in.close();
    return(contents);
}

void js_getFileContents(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);
    string data = getFileContents(fromJsString(args[0]));
    // copy into ArrayBuffer
    auto buf = ArrayBuffer::New(theOneIsolate, data.size());
    data.copy(reinterpret_cast<char*>(buf->GetContents().Data()), data.size());
    args.GetReturnValue().Set(buf);
}

Local<String> jsString(const string& src) {
    return String::NewFromUtf8(theOneIsolate, src.c_str(), NewStringType::kNormal)
        .ToLocalChecked();
}

string fromJsString(const Handle<Value>& src) {
    String::Utf8Value u(src);
    if (*u == nullptr) throw runtime_error("fromJsString failed");
    return string(*u, static_cast<size_t>(u.length()));
}

Local<Object> createPointerObject(void* ptr) {
    auto ret = jsPointerTpl.Get(theOneIsolate)->NewInstance();
    ret->SetAlignedPointerInInternalField(0, ptr);
    return ret;
}

void runScript(Local<Context> context, string filename) {
    HandleScope fhs(theOneIsolate);

    cout << "Loading " << filename << endl;
    string src = getFileContents(filename.c_str());

    cout << "Compiling " << filename << endl;
    ScriptOrigin org(jsString(filename));
    auto code = Script::Compile(context, jsString(src), &org).ToLocalChecked();
    //if (tc.HasCaught()) throw runtime_error("Caught JS exception");

    cout << "Running " << filename << endl;
    code->Run(context);
    //if (tc.HasCaught()) throw runtime_error("Caught JS exception");
}

void js_runScript(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);
    runScript(args.This()->CreationContext(), fromJsString(args[0]));
}

void eval(const Local<Context>& ctx, const string& js) {
    ScriptOrigin org(jsString("eval"));
    auto code = Script::Compile(ctx, jsString(js), &org).ToLocalChecked();
    code->Run(ctx);
}

Local<ArrayBuffer> stringToArrayBuffer(string data) {
    auto buf = ArrayBuffer::New(theOneIsolate, data.size());
    data.copy(reinterpret_cast<char*>(buf->GetContents().Data()), data.size());
    return buf;
}

void js_stringToUtf8(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);
    // TODO skip string
    string data = fromJsString(args[0]);
    args.GetReturnValue().Set(stringToArrayBuffer(data));
}

void js_stringFromUtf8(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);
    args.GetReturnValue().Set(String::NewFromUtf8(theOneIsolate, reinterpret_cast<char*>(
        Local<ArrayBuffer>::Cast(args[0])->GetContents().Data()
    ), NewStringType::kNormal).ToLocalChecked());
}

void js_log(const FunctionCallbackInfo<Value>& args) {
    for (int a = 0; a < args.Length(); a++) {
        if (a != 0) cout << " ";
        cout << fromJsString(args[a]);
    }
    cout << endl;
}

void bullet_setUpContext(Local<Context> ctx);
void graphics_setUpContext(Local<Context> ctx);
void gl_setUpContext(Local<Context> ctx);

Local<Context> setUpContext() {

    auto jspt = ObjectTemplate::New(theOneIsolate);
    jspt->SetInternalFieldCount(1);
    jsPointerTpl.Reset(theOneIsolate, jspt);


    Local<Context> ctx = Context::New(theOneIsolate);

    cout << "Creating globals" << endl;

    auto glob = ctx->Global();
    glob->Set(ctx, jsString("global"), glob);

    // --- define functions

    #define FUN(NAME) glob->Set(ctx, jsString(#NAME), Function::New(ctx, &js_ ## NAME).ToLocalChecked());
    FUN(runScript)
    FUN(log)
    FUN(getFileContents)
    FUN(stringFromUtf8)
    FUN(stringToUtf8)
    #undef FUN

    bullet_setUpContext(ctx);
    graphics_setUpContext(ctx);
    gl_setUpContext(ctx);

    return ctx;
}

int main(int argc, char** argv){

    // Initialize V8.
    V8::InitializeICU();
    V8::InitializeExternalStartupData(".");
    unique_ptr<Platform> myPf(platform::CreateDefaultPlatform());
    V8::InitializePlatform(myPf.get());
    V8::Initialize();

    // Create a new Isolate and make it the current one.
    ArrayBufferAllocator allocator;
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = &allocator;
    Isolate* isolate = Isolate::New(create_params);
    theOneIsolate = isolate;

    {
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);

        Local<Context> context = setUpContext();
        Context::Scope context_scope(context);
        theContext = &context;

        runScript(context, "../js/engine.js");

        std::cout << "Quittin'" << std::endl;
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();



}
