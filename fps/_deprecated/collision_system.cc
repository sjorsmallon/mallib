#include "collision_system.h"
#include <btBulletCollisionCommon.h>



namespace
{
	btDefaultCollisionConfiguration* collision_configuration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlapping_pair_cache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamics_world;

	btAlignedObjectArray<btCollisionShape*> collision_shapes;
}

//@FIXME(Sjors): this is leaking.
void init_collision_system()
{
	collision_configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_configuration);
	overlapping_pair_cache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamics_world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));


	///create a few basic rigid bodies

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collision_shapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, 0, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamics_world->addRigidBody(body);
	}

	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(0, 10, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}

}

void simulate()
{
	dynamics_world->stepSimulation(1.f / 60.f, 10);

	//print positions of all objects
	for (int j = dynamics_world->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		btCollisionObject* obj = dynamics_world->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = obj->getWorldTransform();
		}
		printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
	}
}



	// ///-----cleanup_start-----

	// //remove the rigidbodies from the dynamics world and delete them
	// for (i = dynamics_world->getNumCollisionObjects() - 1; i >= 0; i--)
	// {
	// 	btCollisionObject* obj = dynamics_world->getCollisionObjectArray()[i];
	// 	btRigidBody* body = btRigidBody::upcast(obj);
	// 	if (body && body->getMotionState())
	// 	{
	// 		delete body->getMotionState();
	// 	}
	// 	dynamics_world->removeCollisionObject(obj);
	// 	delete obj;
	// }

	// //delete collision shapes
	// for (int j = 0; j < collision_shapes.size(); j++)
	// {
	// 	btCollisionShape* shape = collision_shapes[j];
	// 	collision_shapes[j] = 0;
	// 	delete shape;
	// }

	//delete dynamics world
	// delete dynamics_world;

	// //delete solver
	// delete solver;

	// //delete broadphase
	// delete overlapping_pair_cache;

	// //delete dispatcher
	// delete dispatcher;

	// delete collision_configuration;

	// //next line is optional: it will be cleared by the destructor when the array goes out of scope
	// collision_shapes.clear();