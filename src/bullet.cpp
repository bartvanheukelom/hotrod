#include <string>
#include <iostream>
#include <stdio.h>
#include <btBulletDynamicsCommon.h>

#include "common.h"

using namespace std;
using namespace v8;

void js_Box_destroy(const FunctionCallbackInfo<Value>& args) {
    HandleScope fhs(theOneIsolate);
    cout << "super destroy box" << endl;

    auto box = static_cast<btRigidBody*>(args.This()->GetAlignedPointerFromInternalField(0));

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

        w->collisionShapes =
            new btAlignedObjectArray<btCollisionShape*>();

        // TODO clean up after GC

        auto jsThis = args.This();
        jsThis->SetAlignedPointerInInternalField(0, w);


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


    static void createBox(const FunctionCallbackInfo<Value>& args) {
        HandleScope fhs(theOneIsolate);
        //cout << "super create box" << endl;

        auto w = getThis(args);
        auto physicsWorld = w->physicsWorld;
        auto collisionShapes = w->collisionShapes;

        btCollisionShape* colShape =
                // new btSphereShape(0.5);
                new btBoxShape(btVector3(1,1,1));
        collisionShapes->push_back(colShape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(
            Local<Number>::Cast(args[0])->Value(),
            Local<Number>::Cast(args[1])->Value(),
            Local<Number>::Cast(args[2])->Value()
        ));

        btScalar mass = 1;
        btVector3 localInertia(0,0,0);
        colShape->calculateLocalInertia(mass,localInertia);

        btDefaultMotionState* boxMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,boxMotionState,colShape,localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);

        physicsWorld->addRigidBody(body);

        // return the pointer to js
        auto ctx = args.This()->CreationContext();
        auto ret = jsPointerTpl.Get(theOneIsolate)->NewInstance();
        ret->SetAlignedPointerInInternalField(0, body);

        ret->Set(ctx, jsString("destroy"),
            Function::New(ctx, &js_Box_destroy).ToLocalChecked());

        args.GetReturnValue().Set(ret);


    }
    
    static void defineJsFunctions(Local<Object> proto, Local<Context> ctx) {
        proto->Set(ctx, jsString("stepSimulation"),
            Function::New(ctx, &stepSimulation).ToLocalChecked());
            proto->Set(ctx, jsString("createStaticBox"),
                Function::New(ctx, &createStaticBox).ToLocalChecked());
        proto->Set(ctx, jsString("createBox"),
            Function::New(ctx, &createBox).ToLocalChecked());
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
