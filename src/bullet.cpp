#include <string>
#include <iostream>
#include <stdio.h>
#include <btBulletDynamicsCommon.h>

#include "common.h"

using namespace std;
using namespace v8;

void js_Box_destroy(const FunctionCallbackInfo<Value>& args) {
    // HandleScope fhs(theOneIsolate);
    // cout << "super destroy box" << endl;
    //
    // auto box = static_cast<btRigidBody*>(args.This()->GetAlignedPointerFromInternalField(0));
    //
}

void js_BulletBody_applyImpulse(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);
    auto body = getPointerFromObject<btRigidBody>(args.This());
    body->applyCentralImpulse(btVector3(
        Local<Number>::Cast(args[0])->Value(),
        Local<Number>::Cast(args[1])->Value(),
        Local<Number>::Cast(args[2])->Value()
    ));
}

void js_BulletBody_getPosition(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);
    auto body = getPointerFromObject<btRigidBody>(args.This());
    auto pos = body->getCenterOfMassPosition();
    auto ctx = args.This()->CreationContext();
    auto dest = Local<Object>::Cast(args[0]);
    dest->Set(ctx, jsString("x"), Number::New(theOneIsolate, pos.x()));
    dest->Set(ctx, jsString("y"), Number::New(theOneIsolate, pos.y()));
}

Persistent<FunctionTemplate> BulletWorldTemplate;

class js_BulletWorld {
public:

    btDiscreteDynamicsWorld* physicsWorld;
    btAlignedObjectArray<btCollisionShape*>* collisionShapes;

    static void construct(const FunctionCallbackInfo<Value>& args) {
        HandleScope fhs(theOneIsolate);

        cout << "c++btCreateWorld" << endl;

        auto w = new js_BulletWorld();
        args.This()->SetAlignedPointerInInternalField(0, w);

        btDefaultCollisionConfiguration* collisionConfiguration =
                new btDefaultCollisionConfiguration();
        btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
        btDbvtBroadphase* overlappingPairCache = new btDbvtBroadphase();
        btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
        w->physicsWorld = new btDiscreteDynamicsWorld(
                dispatcher,overlappingPairCache,
                solver,collisionConfiguration
        );
        w->physicsWorld->setGravity(btVector3(0,0,-30));

        w->collisionShapes = new btAlignedObjectArray<btCollisionShape*>();

        // TODO clean up after GC

    }

    static void createStaticBox(const FunctionCallbackInfo<Value>& args) {
        HandleScope fhs(theOneIsolate);

        auto w = getThis(args);

        // define and add the shape
        btCollisionShape* groundShape = new btBoxShape(btVector3(
            Local<Number>::Cast(args[0])->Value(),
            Local<Number>::Cast(args[1])->Value(),
            Local<Number>::Cast(args[2])->Value()
        ));
        w->collisionShapes->push_back(groundShape);

        // define the position
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(
            Local<Number>::Cast(args[3])->Value(),
            Local<Number>::Cast(args[4])->Value(),
            Local<Number>::Cast(args[5])->Value()
        ));
        btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);

        // create and add the body
        btRigidBody* groundBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, groundMotionState, groundShape));
        w->physicsWorld->addRigidBody(groundBody);

        // TODO return something

    }

    static void stepSimulation(const FunctionCallbackInfo<Value>& args) {
        HandleScope fhs(theOneIsolate);
        getThis(args)->physicsWorld->stepSimulation(Local<Number>::Cast(args[0])->Value(), 10);
    }

    static void createSphere(const FunctionCallbackInfo<Value>& args) {
        HandleScope fhs(theOneIsolate);
        createBody(new btSphereShape(Local<Number>::Cast(args[0])->Value()), args);
    }

    static void createBox(const FunctionCallbackInfo<Value>& args) {
        HandleScope fhs(theOneIsolate);
        double r = Local<Number>::Cast(args[0])->Value();
        createBody(new btBoxShape(btVector3(r,r,r)), args);
    }

    static void createBody(btCollisionShape* colShape, const FunctionCallbackInfo<Value>& args) {
        auto w = getThis(args);
        w->collisionShapes->push_back(colShape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(
            Local<Number>::Cast(args[1])->Value(),
            Local<Number>::Cast(args[2])->Value(),
            Local<Number>::Cast(args[3])->Value()
        ));

        btScalar mass = 1;
        btVector3 localInertia(0,0,0);
        colShape->calculateLocalInertia(mass,localInertia);

        btDefaultMotionState* boxMotionState = new btDefaultMotionState(startTransform);
        btRigidBody* body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass,boxMotionState,colShape,localInertia));

        w->physicsWorld->addRigidBody(body);

        auto ret = createPointerObject(body);
        args.GetReturnValue().Set(ret);
        auto ctx = args.This()->CreationContext();
        ret->Set(ctx, jsString("applyImpulse"),
            Function::New(ctx, &js_BulletBody_applyImpulse).ToLocalChecked());
        ret->Set(ctx, jsString("getPosition"),
            Function::New(ctx, &js_BulletBody_getPosition).ToLocalChecked());



    }

    static void defineJsFunctions(Local<Object> proto, Local<Context> ctx) {
        #define FUN(NAME) proto->Set(ctx, jsString(#NAME), Function::New(ctx, &NAME).ToLocalChecked());
        FUN(stepSimulation)
        FUN(createStaticBox)
        FUN(createBox)
        FUN(createSphere)
        #undef FUN
    }

private:
    static js_BulletWorld* getThis(const FunctionCallbackInfo<Value>& args) {
        return static_cast<js_BulletWorld*>(args.This()->GetAlignedPointerFromInternalField(0));
    }

};


void bullet_setUpContext(Local<Context> ctx) {

    // BulletWorld requires 2 pointers
    auto bwt = FunctionTemplate::New(theOneIsolate, &js_BulletWorld::construct);
    bwt->InstanceTemplate()->SetInternalFieldCount(1);
    bwt->SetClassName(jsString("BulletWorld"));
    BulletWorldTemplate.Reset(theOneIsolate, bwt);

    // make BulletWorld available
    auto bwc = bwt->GetFunction(ctx).ToLocalChecked();
    ctx->Global()->Set(ctx, jsString("BulletWorld"), bwc);

    js_BulletWorld::defineJsFunctions(
        Local<Object>::Cast(bwc->Get(ctx, jsString("prototype")).ToLocalChecked()),
        ctx
    );

}
