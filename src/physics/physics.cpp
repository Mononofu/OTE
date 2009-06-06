//
// C++ Implementation: physics
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "physics.h"
#include "resourcemanager.h"
#include "objectregistry.h"
#include "timer.h"

#include "Ogre.h"
#include "OgreNewt.h"
#include "FeedDataTypes.h"

class PhysicsImpl : public Task
{
	public:
		PhysicsImpl();
		~PhysicsImpl();
		
		/** creates a new object in the world.
		 * @param identifier A string refering to a file somewhere in the ressources of the game,
		 * containing all important data about the object
		 * @return an int acting as ID for the object
		 **/
		void newObject(const std::string& specification, int ID, const Ogre::Vector3& pos, const Ogre::Quaternion& orient, Ogre::Vector3 scale, bool dynamic = true);
		bool doStep();
		void threadWillStart();
		void threadWillStop();
		void receiveData ( std::string feedName, const DataContainer& data );
		
		void handleKeyEvents(const DataContainer& data);
		void handleAppEvents(const DataContainer& data);
		void handleObjectEvents(const DataContainer& data);
		void handleTerrainEvents(const DataContainer& data);
		
		void handleTransform(OgreNewt::Body* body , const Ogre::Quaternion& orient, const Ogre::Vector3& pos, int threadIndex);
	private:
		WorldGraph worldGraph;
		OgreNewt::World* m_World;
		int desired_framerate;
		Ogre::Real m_update, m_elapsed;
		
		double workTime, overheadTime;
		int frames;
};

Physics::Physics() : impl(new PhysicsImpl) { }
Physics::~Physics() { }
bool Physics::doStep() { return impl->step(); }
void Physics::threadWillStart() { impl->threadWillStart(); }
void Physics::threadWillStop() { impl->threadWillStop(); }

PhysicsImpl::PhysicsImpl() : desired_framerate(150), m_elapsed(0.0f), workTime(0.0), overheadTime(0.0), frames(0)
{
	m_World = new OgreNewt::World();
	m_World->setWorldSize(Ogre::Vector3(-1000.0,-1000.0,-1000.0), Ogre::Vector3(1000.0,1000.0,1000.0));

	m_update = (Ogre::Real)(1.0f / (Ogre::Real)desired_framerate);
}

PhysicsImpl::~PhysicsImpl()
{
	delete m_World;
}

bool PhysicsImpl::doStep()
{
	Timer timer;
	m_elapsed += timeSinceLastFrame();

    // loop through and update as many times as necessary (up to 10 times maximum).
	if ((m_elapsed > m_update) && (m_elapsed < (m_update * 10)) )
	{
		while (m_elapsed > m_update)
		{
			m_World->update( m_update );
			m_elapsed -= m_update;
		}
	}
	else
	{
		if (m_elapsed < (m_update))
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds( 500.0f / desired_framerate ));	// Wait half the duration of one frame
		}
		else
		{
            // too much time has passed (would require more than 10 updates!), so just update once and reset.
            // this often happens on the first frame of a game, where assets and other things were loading, then
            // the elapsed time since the last drawn frame is very long.
			m_World->update( m_elapsed );
			m_elapsed = 0.0f; // reset the elapsed time so we don't become "eternally behind".
		}
	}
	workTime += timer.time();
	timer.reset();
	InformationManager::Instance()->postDataToFeed( "world_dynamic", DataContainer(&worldGraph) );
	overheadTime += timer.time();
	frames++;
	return running;
}

void PhysicsImpl::threadWillStart()
{
	InformationManager::Instance()->postDataToFeed( "thread_event", DataContainer(THREAD_STARTING) );
	InformationManager::Instance()->subscribeToFeed("app_event", boost::bind( &PhysicsImpl::handleAppEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("input_keyboard", boost::bind( &PhysicsImpl::handleKeyEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("create_object", boost::bind( &PhysicsImpl::handleObjectEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("create_terrain", boost::bind( &PhysicsImpl::handleTerrainEvents, this, _1));
	InformationManager::Instance()->postDataToFeed( "thread_event", DataContainer(THREAD_STARTED) );
#ifdef DEBUG_BUILD
	DataContainer myManager = InformationManager::Instance()->requestData("ogre.sceneManager", 10);
	m_World->getDebugger().init( boost::any_cast<Ogre::SceneManager*>(myManager.data) );
	m_World->getDebugger().showDebugInformation();
#endif
}
void PhysicsImpl::threadWillStop()
{
	Dout << "Time spent working: " << workTime << "s or " << workTime/(workTime + overheadTime)*100 << "%";
	Dout << "Time spent on overhead: " << overheadTime << "s or " << overheadTime/(workTime + overheadTime)*100 << "%";
	Dout << "Total runtime: " << (workTime + overheadTime);
	Dout << "FPS: " << frames / (workTime + overheadTime);
}

void PhysicsImpl::handleKeyEvents(const DataContainer& data)
{
	InputKeyboardEvent ev = boost::any_cast<InputKeyboardEvent>(data.data);
	if( ev.type == KEY_LEFT && ev.action == BUTTON_PRESSED )
	{
		worldGraph.nodes.front()->setPosition( Ogre::Vector3(-50.0,-10.0,-20.0) );
	}
	else if( ev.type == KEY_RIGHT && ev.action == BUTTON_PRESSED )
	{
		worldGraph.nodes.front()->setPosition( Ogre::Vector3(50.0,-10.0,-20.0) );
	}
}

void PhysicsImpl::handleAppEvents(const DataContainer& data)
{
	app_event ev = boost::any_cast<app_event>(data.data);
		
	if( ev == APP_SHUTDOWN )
	{
		Dout << "physics received shutdown event";
		this->running = false;
	}
}

void PhysicsImpl::handleObjectEvents(const DataContainer& data)
{
	ObjectToCreate obj = boost::any_cast<ObjectToCreate>(data.data);
		
	newObject(obj.specification, obj.node.ID, obj.node.pos, obj.node.orient, obj.scale);
}

void PhysicsImpl::handleTerrainEvents(const DataContainer& data)
{
	Terrain obj = boost::any_cast<Terrain>(data.data);
		
	newObject(obj.specification, obj.node.ID, obj.node.pos, obj.node.orient, obj.scale, false);
}

void PhysicsImpl::handleTransform(OgreNewt::Body* body , const Ogre::Quaternion& orient, const Ogre::Vector3& pos, int threadIndex)
{
	if(pos.y < -100)
	{
//		InformationManager::Instance()->postDataToFeed( "world_removed", DataContainer(body->m_node->ID) );
		delete body;
	}
}

void PhysicsImpl::newObject(const std::string& specification, int ID, const Ogre::Vector3& pos, const Ogre::Quaternion& orient, Ogre::Vector3 scale, bool dynamic)
{
	// look up the identifier and get relevant data - still to add
	OgreNewt::Node *node = new OgreNewt::Node(ID);
	worldGraph.addNode( node );

	if(dynamic)
	{
		// collision primitve - type and size should be determined according to a data file
		OgreNewt::ConvexCollisionPtr col = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::Cylinder(m_World, 4.9, 9.8));

		// now we make a new rigid body based on this collision shape.
		OgreNewt::Body* body = new OgreNewt::Body( m_World, col );
		Ogre::Vector3 inertia, offset, dir;
		col->calculateInertialMatrix(inertia, offset);
		
		body->setMassMatrix( 10.0, 10.0*inertia );
		
		// this is a standard callback that simply add a gravitational force (-9.8*mass) to the body.
		body->setStandardForceCallback();
		body->setVelocity( Ogre::Vector3(-pos.x,-pos.y,-pos.z) );
		body->setLinearDamping(0);
		
		body->attachNode( node );	
		body->setPositionOrientation( pos, orient );
		body->setContinuousCollisionMode(1);
		//body->setCustomTransformCallback( boost::bind( &PhysicsImpl::handleTransform, this, _1, _2, _3, _4 ) );
	}
	else
	{
		DataContainer data = ResourceManager::Instance().loadResource(specification);
		OgreNewt::CollisionPtr col = boost::shared_ptr<OgreNewt::Collision>(new OgreNewt::CollisionPrimitives::TreeCollision(m_World, boost::any_cast<Ogre::MeshPtr>(data.data), true, OgreNewt::CollisionPrimitives::FW_DEFAULT, scale));
		OgreNewt::Body* body = new OgreNewt::Body(m_World, col);
		
		body->attachNode( node );	
		body->setPositionOrientation( pos, orient );
	}	
}

