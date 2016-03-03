#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <btBulletDynamicsCommon.h>
#include <fstream>
#include <cerrno>
#include <stdexcept>

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

#include "common.h"

#include "LoadShaders.h"
extern GLuint LoadShaders(ShaderInfo *shaderinfo);

using namespace std;
using namespace v8;

void Initialize();
void Display();
void Finalize();
// void Reshape(int width, int height);

SDL_Window *mainwindow;
float aspect;
GLuint render_prog;
GLuint vao2;
GLint render_mvp_loc;
GLint render_inColor_loc;
glm::mat4 pv;
glm::vec3 camPos(0, -30, 10);
float camAngle = 0;
float camTilt = 0.3;
glm::vec3 camUp(0,0,1);
int msX, msY;
bool lockCam = false;

bool headless;

Isolate* theOneIsolate;
Persistent<ObjectTemplate> jsPointerTpl;

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




string getFileContents(const char* filename) {
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

Local<String> jsString(const string& src) {
    return String::NewFromUtf8(theOneIsolate, src.c_str(), NewStringType::kNormal)
        .ToLocalChecked();
}

string fromJsString(const Handle<Value>& src) {
    String::Utf8Value u(src);
    if (*u == nullptr) throw runtime_error("fromJsString failed");
    return string(*u, static_cast<size_t>(u.length()));
}

void Initialize() {

    ShaderInfo shader_info[] = {
        { GL_VERTEX_SHADER, "primitive_restart.vs.glsl" },
        { GL_FRAGMENT_SHADER, "primitive_restart.fs.glsl" },
        { GL_NONE, NULL }
    };

    render_prog = LoadShaders(shader_info);
    render_mvp_loc = glGetUniformLocation(render_prog, "mvp");
    render_inColor_loc = glGetUniformLocation(render_prog, "inColor");

    GLuint vbo;

    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);

    // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void rotateVec(glm::vec3& vec, const glm::vec3 axis, float angle) {
	glm::mat4 rotation = glm::rotate(glm::mat4(), angle, axis);
	vec = glm::vec3(rotation * glm::vec4(vec, 1));
}

void Finalize() {
    glUseProgram(0);
    glDeleteProgram(render_prog);
    // glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao2);
}

void Reshape() {

    int width, height;
    SDL_GetWindowSize(mainwindow, &width, &height);

    cout << "Reshape " << width << "x" << height << endl;

    glViewport(0, 0 , width, height);
    aspect = float(height) / float(width);

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

void eval(Local<Context> ctx, const string& js) {
    ScriptOrigin org(jsString("eval"));
    auto code = Script::Compile(ctx, jsString(js), &org).ToLocalChecked();
    code->Run(ctx);
}

void js_nativeStep(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);

    SDL_GL_SwapWindow(mainwindow);

    SDL_Event event;
    bool doQuit = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            doQuit = true;
            break;
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                Reshape();
            }
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_F5:
                {
                    eval(args.This()->CreationContext(), "tmp_f5pressed()");
                } break;

                case SDLK_g:
                    lockCam = !lockCam;
                    break;

                case SDLK_ESCAPE:
                    doQuit = true;
                    break;
                default: break;
            }
        }
    }

    args.GetReturnValue().Set(!doQuit);

}

void js_prepareBoxRender(const FunctionCallbackInfo<Value>& args) {
    if (headless) return;
    HandleScope fhs(theOneIsolate);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_MULTISAMPLE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(render_prog);

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    // movement
    static const double camMoveSpeed = 0.66;
	if (state[SDL_SCANCODE_A]) camPos.x -= camMoveSpeed;
    if (state[SDL_SCANCODE_D]) camPos.x += camMoveSpeed;
    if (state[SDL_SCANCODE_S]) camPos.y -= camMoveSpeed;
    if (state[SDL_SCANCODE_W]) camPos.y += camMoveSpeed;
    if (state[SDL_SCANCODE_F]) camPos.z -= camMoveSpeed;
    if (state[SDL_SCANCODE_R]) camPos.z += camMoveSpeed;

    // rotation
    static const double camRotA = 0.01;
    static const double camRotT = 0.01;
    if (state[SDL_SCANCODE_J]) camAngle -= camRotA;
	if (state[SDL_SCANCODE_L]) camAngle += camRotA;
	if (state[SDL_SCANCODE_I]) camTilt += camRotT;
	if (state[SDL_SCANCODE_K]) camTilt -= camRotT;

    if (!lockCam) SDL_GetMouseState(&msX, &msY);

    int width, height;
    SDL_GetWindowSize(mainwindow, &width, &height);

	glm::vec3 camLook(0, 1, 0);
	rotateVec(camLook, glm::vec3(1,0,0), -(camTilt + (msY-height/2) * 0.003));
	rotateVec(camLook, camUp, -(camAngle + (msX-width/2) * 0.003));

    glm::mat4 projection_matrix(glm::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 1500.0f));
//    glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glm::mat4 view(glm::lookAt(camPos, camPos + camLook, camUp));
    pv = projection_matrix * view;

    // ----------- cubes ------------- //

    glBindVertexArray(vao2);
}

void js_renderBox(const FunctionCallbackInfo<Value>& args) {
    if (headless) return;
    HandleScope fhs(theOneIsolate);

    glm::mat4 ATTRIBUTE_ALIGNED16(model_matrix), mvp;
    glm::vec3 inColor;

    float family;
    if (args[0]->IsNumber()) {
        model_matrix = glm::translate(glm::mat4(1), glm::vec3(
            Local<Number>::Cast(args[0])->Value(),
            Local<Number>::Cast(args[1])->Value(),
            Local<Number>::Cast(args[2])->Value()
        ));
        float scale = static_cast<float>(Local<Number>::Cast(args[6])->Value());
        model_matrix = glm::scale(model_matrix, glm::vec3(scale, scale, scale));
        inColor.x = static_cast<float>(Local<Number>::Cast(args[3])->Value());
        inColor.y = static_cast<float>(Local<Number>::Cast(args[4])->Value());
        inColor.z = static_cast<float>(Local<Number>::Cast(args[5])->Value());
    } else {
        auto box = static_cast<btRigidBody*>(
                Local<Object>::Cast(args[0])->GetAlignedPointerFromInternalField(0));
        btTransform trans;
        box->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(glm::value_ptr(model_matrix));
        inColor.x = 0.5;
        inColor.y = 0;
        inColor.z = 0;
    }


    mvp = pv * model_matrix;
    glUniformMatrix4fv(render_mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(render_inColor_loc, inColor.x, inColor.y, inColor.z);
    glDrawArrays(GL_TRIANGLES, 0, 3*12);

}

void js_log(const FunctionCallbackInfo<Value>& args) {
    for (int a = 0; a < args.Length(); a++) {
        if (a != 0) cout << " ";
        cout << fromJsString(args[a]);
    }
    cout << endl;
}


Local<Context> setUpContext() {

    auto jspt = ObjectTemplate::New(theOneIsolate);
    jspt->SetInternalFieldCount(1);
    jsPointerTpl.Reset(theOneIsolate, jspt);


    Local<Context> ctx = Context::New(theOneIsolate);

    cout << "Creating globals" << endl;

    auto glob = ctx->Global();
    glob->Set(ctx, jsString("global"), glob);

    // define functions
    glob->Set(ctx, jsString("runScript"),
        Function::New(ctx, &js_runScript).ToLocalChecked());
    glob->Set(ctx, jsString("nativeStep"),
        Function::New(ctx, &js_nativeStep).ToLocalChecked());
    glob->Set(ctx, jsString("log"),
        Function::New(ctx, &js_log).ToLocalChecked());
    glob->Set(ctx, jsString("prepareBoxRender"),
        Function::New(ctx, &js_prepareBoxRender).ToLocalChecked());
    glob->Set(ctx, jsString("renderBox"),
        Function::New(ctx, &js_renderBox).ToLocalChecked());

    bullet_setUpContext(ctx);

    return ctx;
}

int main(int argc, char** argv){

    cout << argc << endl;
    if (argc == 2) headless = true;
    else headless = false;

    SDL_GLContext* maincontextp;
    if (!headless) {

        SDL_GLContext maincontext;
        maincontextp = &maincontext;

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "Unable to initialize SDL";
            return 1;
        }

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        mainwindow = SDL_CreateWindow(
            "TankEvo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        	1600, 900, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );
        if (mainwindow == nullptr) {
            std::cout << "SDL Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        maincontext = SDL_GL_CreateContext(mainwindow);

        GLenum rev;
        glewExperimental = GL_TRUE;
        rev = glewInit();

        if (GLEW_OK != rev){
            std::cout << "Error: " << glewGetErrorString(rev) << std::endl;
            exit(1);
        } else {
            std::cout << "GLEW Init: Success!" << std::endl;
        }

        SDL_GL_SetSwapInterval(0);

        Initialize();
        Reshape();
        
    }

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

        runScript(context, "engine.js");

        std::cout << "Quittin'" << std::endl;
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();

    Finalize();

    if (!headless) {
        SDL_GL_DeleteContext(*maincontextp);
        SDL_DestroyWindow(mainwindow);
        SDL_Quit();
    }


}
