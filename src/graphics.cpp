#include <GL/glew.h>
#include <GL/gl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <btBulletDynamicsCommon.h>

#include "common.h"

using namespace std;
using namespace v8;

SDL_Window *mainwindow;
SDL_GLContext maincontext;
float aspect;

glm::mat4 proj_view_matrix;

glm::vec3 camPos(0, -30, 50);
float camAngle = 0;
float camTilt = 0.3;
glm::vec3 camUp(0,0,1);
int msX, msY;
bool lockCam = false;

void rotateVec(glm::vec3& vec, const glm::vec3 axis, float angle) {
	glm::mat4 rotation = glm::rotate(glm::mat4(), angle, axis);
	vec = glm::vec3(rotation * glm::vec4(vec, 1));
}

void Reshape() {

    int width, height;
    SDL_GetWindowSize(mainwindow, &width, &height);

    cout << "Reshape " << width << "x" << height << endl;

    glViewport(0, 0 , width, height);
    aspect = float(height) / float(width);

}


void js_initGraphics(const FunctionCallbackInfo<Value>& args) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw runtime_error("Unable to initialize SDL");

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
    if (mainwindow == nullptr)
        throw runtime_error(string("SDL_CreateWindow error ") + SDL_GetError());

    maincontext = SDL_GL_CreateContext(mainwindow);

    GLenum rev;
    glewExperimental = GL_TRUE;
    rev = glewInit();

    if (GLEW_OK != rev) {
        std::cout << "Error: " << glewGetErrorString(rev) << std::endl;
        exit(1);
    } else {
        std::cout << "GLEW Init: Success!" << std::endl;
    }

    // vsync 0 = off 1 = on
    SDL_GL_SetSwapInterval(1);
    // SDL_GL_SetSwapInterval(0);

    Reshape();

}

void js_destroyGraphics(const FunctionCallbackInfo<Value>& args) {
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();
}

void js_updateCam(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);

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

	if (state[SDL_SCANCODE_G]) lockCam = true;
	if (state[SDL_SCANCODE_H]) lockCam = false;

    if (state[SDL_SCANCODE_V]) SDL_GL_SetSwapInterval(1);
    if (state[SDL_SCANCODE_B]) SDL_GL_SetSwapInterval(0);

    if (!lockCam) SDL_GetMouseState(&msX, &msY);

    int width, height;
    SDL_GetWindowSize(mainwindow, &width, &height);

	glm::vec3 camLook(0, 1, 0);
	rotateVec(camLook, glm::vec3(1,0,0), -(camTilt + (msY-height/2) * 0.003));
	rotateVec(camLook, camUp, -(camAngle + (msX-width/2) * 0.004));

    glm::mat4 projection_matrix(glm::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 1500.0f));
    glm::mat4 view(glm::lookAt(camPos, camPos + camLook, camUp));
    proj_view_matrix = projection_matrix * view;

}

void js_setMvp(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);

    glm::mat4 model_matrix;
    if (args[1]->IsNumber()) {
        model_matrix = glm::translate(glm::mat4(1), glm::vec3(
            Local<Number>::Cast(args[1])->Value(),
            Local<Number>::Cast(args[2])->Value(),
            Local<Number>::Cast(args[3])->Value()
        ));
    } else {
        auto box = static_cast<btRigidBody*>(
                Local<Object>::Cast(args[1])->GetAlignedPointerFromInternalField(0));
        btTransform trans;
        box->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(glm::value_ptr(model_matrix));
    }

    glm::mat4 mvp = proj_view_matrix * model_matrix;
    glUniformMatrix4fv(Local<Int32>::Cast(args[0])->Value(), 1, GL_FALSE, glm::value_ptr(mvp));

}

void js_nativeStep(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);

    SDL_GL_SwapWindow(mainwindow);

    // TODO headlesssss

    SDL_Event event;
    bool doQuit = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            doQuit = true;
            break;
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                //Reshape(); TODO fix
            }
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_F5:
                {
                    eval(args.This()->CreationContext(), "tmp_f5pressed()");
                } break;

                case SDLK_ESCAPE:
                    doQuit = true;
                    break;
                default: break;
            }
        }
    }

    args.GetReturnValue().Set(!doQuit);

}

void graphics_setUpContext(Local<Context> ctx) {

    auto glob = ctx->Global();

	#define FUN(NAME) glob->Set(ctx, jsString(#NAME), Function::New(ctx, &js_ ## NAME).ToLocalChecked());
	FUN(initGraphics)
	FUN(destroyGraphics)
    FUN(nativeStep)
	FUN(updateCam)
	FUN(setMvp)
	#undef FUN

}
