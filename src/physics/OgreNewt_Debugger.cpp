#include "OgreNewt_stdafx.h"
#include "OgreNewt_Debugger.h"
#include "OgreNewt_World.h"
#include "OgreNewt_Body.h"
#include "OgreNewt_Joint.h"
#include "OgreNewt_Collision.h"

#include <NewtonCustomJoint.h>

#include <sstream>

#ifdef __APPLE__
#   include <Ogre/OgreSceneNode.h>
#   include <Ogre/OgreSceneManager.h>
#   include <Ogre/OgreManualObject.h>
#else
#   include <OgreSceneNode.h>
#   include <OgreSceneManager.h>
#   include <OgreManualObject.h>
#endif

namespace OgreNewt
{

//////////////////////////////////////////////////////////
// DEUBBER FUNCTIONS
//////////////////////////////////////////////////////////
Debugger::Debugger(const OgreNewt::World* world)
{
    m_world = world;
    m_debugnode = NULL;
    m_raycastsnode = NULL;
    m_defaultcolor = Ogre::ColourValue::White;

    m_recordraycasts = false;
    m_markhitbodies = false;
    m_raycol = Ogre::ColourValue::Green;
    m_convexcol = Ogre::ColourValue::Blue;
    m_hitbodycol = Ogre::ColourValue::Red;
    m_prefilterdiscardedcol = Ogre::ColourValue::Black;
}

Debugger::~Debugger()
{
    deInit();
}

void Debugger::init( Ogre::SceneManager* smgr )
{
    if( !m_debugnode )
    {
        m_debugnode = smgr->getRootSceneNode()->createChildSceneNode("__OgreNewt__Debugger__Node__");
        m_debugnode->setListener(this);
    }

    if( !m_raycastsnode )
    {
        m_raycastsnode = smgr->getRootSceneNode()->createChildSceneNode("__OgreNewt__Raycasts_Debugger__Node__");
        m_raycastsnode->setListener(this);
    }

	m_sceneManager = smgr;
}

void Debugger::deInit()
{
    clearBodyDebugDataCache();
    if (m_debugnode)
    {
        m_debugnode->setListener(NULL);
        m_debugnode->removeAndDestroyAllChildren();
        m_debugnode->getParentSceneNode()->removeAndDestroyChild( m_debugnode->getName() );
        m_debugnode = NULL;
    }


    clearRaycastsRecorded();
    if( m_raycastsnode )
    {
        m_raycastsnode->setListener(NULL);
        m_raycastsnode->removeAndDestroyAllChildren();
        m_raycastsnode->getParentSceneNode()->removeAndDestroyChild( m_raycastsnode->getName() );
        m_raycastsnode = NULL;
    }
}

void Debugger::nodeDestroyed (const Ogre::Node *node)
{
    if(node == m_debugnode)
    {
        m_debugnode = NULL;
        clearBodyDebugDataCache();
    }

    if(node == m_raycastsnode)
    {
        m_raycastsnode = NULL;
        clearRaycastsRecorded();
    }
}


void Debugger::clearBodyDebugDataCache()
{
	for(BodyDebugDataMap::iterator it = m_cachemap.begin(); it != m_cachemap.end(); it++)
	{
		Ogre::ManualObject* mo = it->second.m_lines;
		if( mo )
			delete mo;
		OgreNewt::OgreAddons::MovableText *text = it->second.m_text;
		if( text )
			delete text;
	}
	m_cachemap.clear();
}



void Debugger::showDebugInformation( )
{
    if (!m_debugnode)
        return;

    m_debugnode->removeAllChildren();

    // make the new lines.
    for( Body* body = m_world->getFirstBody(); body; body = body->getNext() )
    {
        processBody(body);
    }
    
	// display any joint debug information
	NewtonWorldForEachJointDo (m_world->getNewtonWorld(), newtonprocessJoints, this);

    // delete old entries
    BodyDebugDataMap newbodymap;
    for(BodyDebugDataMap::iterator it = m_cachemap.begin(); it != m_cachemap.end(); it++)
    {
        if( it->second.m_updated )
            newbodymap.insert(*it);
        else
        {
            Ogre::ManualObject* mo = it->second.m_lines;
            if( mo )
                delete mo;
            OgreNewt::OgreAddons::MovableText *text = it->second.m_text;
            if( text )
                delete text;
        }
    }
    m_cachemap.swap(newbodymap);
}

void Debugger::hideDebugInformation()
{
    // erase any existing lines!
    if( m_debugnode )
        m_debugnode->removeAllChildren();
}

void Debugger::setMaterialColor(const MaterialID* mat, Ogre::ColourValue col)
{
    m_materialcolors[mat->getID()] = col;
}

void Debugger::setDefaultColor(Ogre::ColourValue col)
{
    m_defaultcolor = col;
}


void _CDECL Debugger::newtonprocessJoints (const NewtonJoint* newtonJoint, void* userData)
{
	Debugger* me = (Debugger*) userData;
	NewtonCustomJoint* customJoint = (NewtonCustomJoint*) NewtonJointGetUserData(newtonJoint);
	me->processJoint((Joint*) customJoint->GetUserData());
}


void Debugger::processJoint(Joint* joint)
{
	// show joint info
	joint->showDebugData(m_debugnode);
}


void Debugger::buildDebugObjectFromCollision(Ogre::ManualObject* object, Ogre::ColourValue colour, const OgreNewt::Collision& shape) const
{
	object->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST );

	// set color
//	if( it != m_materialcolors.end() )
//		object->colour(it->second);
//	else
//		object->colour(m_defaultcolor);

	object->colour(colour);

	float matrix[16];
	Converters::QuatPosToMatrix(Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, &matrix[0]);

	NewtonCollisionForEachPolygonDo (shape.getNewtonCollision(), &matrix[0], newtonPerPoly, object);

	object->end();
}


void Debugger::processBody( OgreNewt::Body* bod )
{
    NewtonBody* newtonBody = bod->getNewtonBody();
    MaterialIdColorMap::iterator it = m_materialcolors.find( NewtonBodyGetMaterialGroupID(newtonBody) );

    Ogre::Vector3 pos, vel, omega;
    Ogre::Quaternion ori;
    bod->getVisualPositionOrientation(pos, ori);
	
	vel = bod->getVelocity();
	omega = bod->getOmega();

	// ----------- create debug-text ------------
	std::ostringstream oss_name;
	oss_name << "__OgreNewt__Debugger__Body__" << bod << "__";
	std::ostringstream oss_info;
	oss_info.precision(2);
	oss_info.setf(std::ios::fixed,std::ios::floatfield);
	Ogre::Vector3 inertia;
	Ogre::Real mass;
	bod->getMassMatrix(mass, inertia);
    
	oss_info << "[" << bod->getOgreNode()->getName() << "]" << std::endl;
	oss_info << "Mass: " << mass << std::endl;
	oss_info << "Position: " << pos[0] << " x " << pos[1] << " x " << pos[2] << std::endl;
	oss_info << "Velocity: " << vel[0] << " x " << vel[1] << " x " << vel[2] << std::endl;
	oss_info << "Omega: " << omega[0] << " x " << omega[1] << " x " << omega[2] << std::endl;
	oss_info << "Inertia: " << inertia[0] << " x " << inertia[1] << " x " << inertia[2] << std::endl;

    // ----------- ------------------ ------------

    // look for cached data
    BodyDebugData* data = &m_cachemap[bod];

    if( data->m_lastcol == bod->getCollision() ) // use cached data
    {
        // set new position...
        data->m_node->setPosition(pos);
        data->m_node->setOrientation(ori);
        data->m_updated = 1;
        m_debugnode->addChild(data->m_node);
        data->m_text->setCaption(oss_info.str());
        data->m_text->setLocalTranslation(bod->getAABB().getSize().y*1.1*Ogre::Vector3::UNIT_Y);
    }
    else
    {
        data->m_lastcol = bod->getCollision();
        data->m_updated = 1;

        if( data->m_node )
        {
            data->m_node->detachAllObjects();
            data->m_node->setPosition(pos);
            data->m_node->setOrientation(ori);
        }
        else
            data->m_node = m_debugnode->createChildSceneNode(pos, ori);

        if( data->m_lines )
		{
            data->m_lines->clear();
		}
        else
        {
            std::ostringstream oss;
            oss << "__OgreNewt__Debugger__Lines__" << bod << "__";
            data->m_lines = new Ogre::ManualObject(oss.str());
        }

        if( data->m_text )
        {
            data->m_text->setCaption(oss_info.str());
            data->m_text->setLocalTranslation(bod->getAABB().getMaximum().y*1.1*Ogre::Vector3::UNIT_Y);
        }
        else
        {
            data->m_text = new OgreNewt::OgreAddons::MovableText( oss_name.str(), oss_info.str(), "BlueHighway-10",0.5);
            data->m_text->setLocalTranslation(bod->getAABB().getMaximum().y/2*Ogre::Vector3::UNIT_Y+Ogre::Vector3::UNIT_Y*0.1);
            data->m_text->setTextAlignment( OgreNewt::OgreAddons::MovableText::H_LEFT, OgreNewt::OgreAddons::MovableText::V_ABOVE );
        }

        data->m_node->attachObject(data->m_text);
        
/*
        data->m_lines->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST );

        // set color
        if( it != m_materialcolors.end() )
            data->m_lines->colour(it->second);
        else
            data->m_lines->colour(m_defaultcolor);

        float matrix[16];
        Converters::QuatPosToMatrix(Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, &matrix[0]);
        
        NewtonCollisionForEachPolygonDo( NewtonBodyGetCollision(newtonBody), &matrix[0], newtonPerPoly, data->m_lines );
        data->m_lines->end();
*/
		buildDebugObjectFromCollision (data->m_lines, m_defaultcolor, *bod->getCollision());

        data->m_node->attachObject(data->m_lines);
    }
}



void _CDECL Debugger::newtonPerPoly( void* userData, int vertexCount, const float* faceVertec, int id )
{
    Ogre::ManualObject* lines = (Ogre::ManualObject*)userData;
    Ogre::Vector3 p0, p1;

        if( vertexCount < 2 )
            return;

    int i= vertexCount - 1;
    p0 = Ogre::Vector3( faceVertec[(i*3) + 0], faceVertec[(i*3) + 1], faceVertec[(i*3) + 2] );


    for (i=0;i<vertexCount;i++)
    {
        p1 = Ogre::Vector3( faceVertec[(i*3) + 0], faceVertec[(i*3) + 1], faceVertec[(i*3) + 2] );

        lines->position( p0 );
        lines->position( p1 );

        p0 = p1;
    }
}




// ----------------- raycast-debugging -----------------------
void Debugger::startRaycastRecording(bool markhitbodies)
{
    m_recordraycasts = true;
    m_markhitbodies = markhitbodies;
}

bool Debugger::isRaycastRecording()
{
    return m_recordraycasts;
}

bool Debugger::isRaycastRecordingHitBodies()
{
    return m_markhitbodies;
}

void Debugger::clearRaycastsRecorded()
{
    if( m_raycastsnode )
    {
/*
        while( m_raycastsnode->numAttachedObjects() > 0 )
        {
            delete m_raycastsnode->detachObject((unsigned short)0);
        }
*/
        m_raycastsnode->removeAndDestroyAllChildren();
    }

    for(ManualObjectList::iterator it = mRecordedRaycastObjects.begin(); it != mRecordedRaycastObjects.end(); it++)
    {
        delete (*it);
    }
    mRecordedRaycastObjects.clear();
}

void Debugger::stopRaycastRecording()
{
    m_recordraycasts = false;
}

void Debugger::setRaycastRecordingColor(Ogre::ColourValue rayCol, Ogre::ColourValue convexCol, Ogre::ColourValue hitBodyCol, Ogre::ColourValue prefilterDiscardedBodyCol)
{
    m_raycol = rayCol;
    m_convexcol = convexCol;
    m_hitbodycol = hitBodyCol;
    m_prefilterdiscardedcol = prefilterDiscardedBodyCol;
}

void Debugger::addRay(const Ogre::Vector3 &startpt, const Ogre::Vector3 &endpt)
{
    if (!m_raycastsnode)
        return;

    static int i = 0;
    std::ostringstream oss;
    oss << "__OgreNewt__Raycast_Debugger__Lines__Raycastline__" << i++ << "__";
    Ogre::ManualObject *line = new Ogre::ManualObject(oss.str());

    line->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST );
    line->colour(m_raycol);
    line->position(startpt);
    line->position(endpt);
    line->end();

#ifndef WIN32
    m_world->ogreCriticalSectionLock();
#endif
    mRecordedRaycastObjects.push_back(line);
    m_raycastsnode->attachObject(line);
#ifndef WIN32
    m_world->ogreCriticalSectionUnlock();
#endif
}

void Debugger::addConvexRay(const OgreNewt::ConvexCollisionPtr& col, const Ogre::Vector3 &startpt, const Ogre::Quaternion &colori, const Ogre::Vector3 &endpt)
{
    if (!m_raycastsnode)
        return;

    static int i = 0;
    // lines from aab
    std::ostringstream oss;
    oss << "__OgreNewt__Raycast_Debugger__Lines__Convexcastlines__" << i++ << "__";
    Ogre::ManualObject *line = new Ogre::ManualObject(oss.str());

    line->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST );
    line->colour(m_convexcol);

    // aab1
    Ogre::AxisAlignedBox aab1 = col->getAABB(colori, startpt);
    const Ogre::Vector3* corners1 = aab1.getAllCorners();
    Ogre::AxisAlignedBox aab2 = col->getAABB(colori, endpt);
    const Ogre::Vector3* corners2 = aab2.getAllCorners();
    for(int i = 0; i < 4; i++)
    {
        line->position(corners1[i]); line->position(corners1[(i+1)%4]);
        line->position(corners1[i+4]); line->position(corners1[(i+1)%4+4]);
        line->position(corners2[i]); line->position(corners2[(i+1)%4]);
        line->position(corners2[i+4]); line->position(corners2[(i+1)%4+4]);
        line->position(corners1[i]); line->position(corners2[i]);
        line->position(corners1[i+4]); line->position(corners2[i+4]);
    }
    line->position(corners1[0]); line->position(corners1[6]);
    line->position(corners1[1]); line->position(corners1[5]);
    line->position(corners1[2]); line->position(corners1[4]);
    line->position(corners1[3]); line->position(corners1[7]);
    line->position(corners2[0]); line->position(corners2[6]);
    line->position(corners2[1]); line->position(corners2[5]);
    line->position(corners2[2]); line->position(corners2[4]);
    line->position(corners2[3]); line->position(corners2[7]);

    // bodies
    float matrix[16];
    Converters::QuatPosToMatrix(colori, startpt, &matrix[0]);
    NewtonCollisionForEachPolygonDo( col->getNewtonCollision(), &matrix[0], newtonPerPoly, line );
    Converters::QuatPosToMatrix(colori, endpt, &matrix[0]);
    NewtonCollisionForEachPolygonDo( col->getNewtonCollision(), &matrix[0], newtonPerPoly, line );


    line->end();


#ifndef WIN32
    m_world->ogreCriticalSectionLock();
#endif
    mRecordedRaycastObjects.push_back(line);
    m_raycastsnode->attachObject(line);
#ifndef WIN32
    m_world->ogreCriticalSectionUnlock();
#endif
}

void Debugger::addDiscardedBody(const OgreNewt::Body* body)
{
    if (!m_raycastsnode)
        return;

    static int i = 0;
    float matrix[16];
    Ogre::Vector3 pos;
    Ogre::Quaternion ori;

    std::ostringstream oss;
    oss << "__OgreNewt__Raycast_Debugger__Lines__DiscardedBody__" << i++ << "__";
    Ogre::ManualObject *line = new Ogre::ManualObject(oss.str());

    line->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST );
    line->colour(m_prefilterdiscardedcol);

    body->getVisualPositionOrientation(pos, ori);
    Converters::QuatPosToMatrix(ori, pos, &matrix[0]);
    NewtonCollisionForEachPolygonDo( body->getCollision()->getNewtonCollision() , &matrix[0], newtonPerPoly, line );

    line->end();


#ifndef WIN32
    m_world->ogreCriticalSectionLock();
#endif
    mRecordedRaycastObjects.push_back(line);
    m_raycastsnode->attachObject(line);
#ifndef WIN32
    m_world->ogreCriticalSectionUnlock();
#endif
}

void Debugger::addHitBody(const OgreNewt::Body* body)
{
    if (!m_raycastsnode)
        return;

    static int i = 0;
    float matrix[16];
    Ogre::Vector3 pos;
    Ogre::Quaternion ori;

    std::ostringstream oss;
    oss << "__OgreNewt__Raycast_Debugger__Lines__HitBody__" << i++ << "__";
    Ogre::ManualObject *line = new Ogre::ManualObject(oss.str());

    line->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST );
    line->colour(m_hitbodycol);

    body->getVisualPositionOrientation(pos, ori);
    Converters::QuatPosToMatrix(ori, pos, &matrix[0]);
    NewtonCollisionForEachPolygonDo( body->getCollision()->getNewtonCollision() , &matrix[0], newtonPerPoly, line );

    line->end();

#ifndef WIN32
    m_world->ogreCriticalSectionLock();
#endif
    mRecordedRaycastObjects.push_back(line);
    m_raycastsnode->attachObject(line);
#ifndef WIN32
    m_world->ogreCriticalSectionUnlock();
#endif
}

}   // end namespace OgreNewt

