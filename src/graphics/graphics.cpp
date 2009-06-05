/***************************************************************************
 *   Copyright (C) 2008 by Mononofu   *
 *   mononofu@mononofu-laptop   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "graphics.h"
#include "objectregistry.h"
#include "FeedDataTypes.h"
#include "boost/lexical_cast.hpp"

#include "listener.h"
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "MyGUI.h"
#include "Caelum.h"
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

class GraphicsImpl : public Task, public Ogre::WindowEventListener, public DataProvider
{
	public:
		GraphicsImpl();
		~GraphicsImpl() { }
		bool doStep();
		void threadWillStart();
		void threadWillStop();
		
		void handleKeyEvents(const DataContainer& data);
		void handleMouseEvents(const DataContainer& data);
		void handleAppEvents(const DataContainer& data);
		void handleObjectEvents(const DataContainer& data);
		void handleTerrainEvents(const DataContainer& data);
		void handleWorldEvents(const DataContainer& data);
		
		void receiveData ( std::string feedName, const DataContainer& data );
		DataContainer getData(const DataIdentifier& id);
	private:
		boost::shared_ptr<Ogre::Root> root;
		boost::shared_ptr<MyGUI::Gui> gui;
		MyGUI::WidgetPtr mainGuiWidget;
		boost::shared_ptr<Caelum::CaelumSystem> caelumSystem;
		Ogre::RenderSystem* rSys;
		Ogre::RenderWindow* window;
		Ogre::SceneManager* sceneMgr;
		Ogre::Camera* camera;
		Ogre::Viewport* viewPort;
		std::string resourcePath;
		boost::shared_ptr<MyFrameListener> listener;

		void loadPlugins();
		void setupResources();
		bool configure();
		void chooseSceneManager();
		void createCamera();
		void createViewports();
		void createResourceListener();
		void loadResources();
		void createScene();
		void createFrameListener();
		void addNode(const ObjectToCreate& object);
		void updatePositions();
		void setupGUI();
		void guiCallback(MyGUI::WidgetPtr sender);
		
		void destroyScene();
		
		void windowResized(Ogre::RenderWindow* rw);
		bool windowClosing(Ogre::RenderWindow* rw);

		float moveScale;
		Ogre::Vector3 movementVector;
		
		/** WorldGraph currently used for rendering **/
		WorldGraph* frontWorld;
		/** WorldGraph which is updated in the background **/
		WorldGraph* backWorld;
		/** used to synch exchange of frontWorld and backWorld **/
		boost::mutex worldMutex;
		boost::mutex addNodesMutex;
		
		
		
		std::map<int, Ogre::SceneNode*> nodes;
		std::vector<ObjectToCreate> nodesToAdd;
		std::vector<Terrain> terrainToCreate;
		bool newWorld;
};

Graphics::Graphics() : impl( new GraphicsImpl() ) { }
bool Graphics::doStep() { return impl->step(); }
void Graphics::threadWillStart() { impl->threadWillStart(); }
void Graphics::threadWillStop() { impl->threadWillStop(); }
DataContainer Graphics::getData(const DataIdentifier& id) { return impl->getData(id); }

GraphicsImpl::GraphicsImpl() : movementVector(0, 0, 0), frontWorld(new WorldGraph), backWorld(new WorldGraph), newWorld(false)
{
}

bool GraphicsImpl::doStep()
{
	if(newWorld)
	{
		boost::mutex::scoped_lock lock(worldMutex);
		WorldGraph* temp;
		temp = frontWorld;
		frontWorld = backWorld;
		backWorld = temp;
		newWorld = false;
	}
	gui->injectFrameEntered(timeSinceLastFrame());
	moveScale = timeSinceLastFrame() * 100;
	camera->moveRelative(movementVector * moveScale);
	updatePositions();
	Ogre::WindowEventUtilities::messagePump();
	//caelumSystem->notifyCameraChanged (camera);
	root->renderOneFrame();
	return this->running;
}

void GraphicsImpl::threadWillStart()
{
	InformationManager::Instance()->postDataToFeed( "thread_event", DataContainer(THREAD_STARTING) );

	InformationManager::Instance()->subscribeToFeed("app_event", boost::bind( &GraphicsImpl::handleAppEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("input_keyboard", boost::bind( &GraphicsImpl::handleKeyEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("input_mouse", boost::bind( &GraphicsImpl::handleMouseEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("world_dynamic", boost::bind( &GraphicsImpl::handleWorldEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("create_object", boost::bind( &GraphicsImpl::handleObjectEvents, this, _1));
	InformationManager::Instance()->subscribeToFeed("create_terrain", boost::bind( &GraphicsImpl::handleTerrainEvents, this, _1));
	
	Dout <<  "Creating root";
	root.reset(new Ogre::Root ( "", "", resourcePath + "ogre.log" ));
	
	Dout << "Loading plugins" ;
	loadPlugins();
	
	Dout << "Setting Ressources" ;
	setupResources();
	
	Dout << "Configuring root" ;
	if(!configure())
	{
		Derr << "Failed to configure root";
	}

	
	Dout << "Create Scenemanager" ;
	chooseSceneManager();

	Dout << "Create Camera" ;
	createCamera();
	
	Dout << "Create Viewport" ;
	createViewports();	
	
	Dout << "Set default MipMap lvl" ;
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	Dout << "Create resource listeners" ;
	createResourceListener();
	
	Dout << "Load resources" ;
	loadResources();
	
	Dout << "Setup GUI";
	setupGUI();

	Dout << "Create Framelistener" ;
	createFrameListener();
	
	Dout << "Create Basic Scene" ;
	createScene();

	root->clearEventTimes();
	
	InformationManager::Instance()->offerData("window", this);
#ifdef DEBUG_BUILD
	InformationManager::Instance()->offerData("ogre", this);
#endif
	
	InformationManager::Instance()->postDataToFeed( "thread_event", DataContainer(THREAD_STARTED) );
}

void GraphicsImpl::threadWillStop()
{
	gui->shutdown();
	Ogre::WindowEventUtilities::removeWindowEventListener(window, this);
	windowClosed(window);
	root->shutdown();
	//boost::this_thread::sleep(boost::posix_time::milliseconds(100));		// Wait 1/100 s
}

void GraphicsImpl::handleKeyEvents(const DataContainer& data)
{
	InputKeyboardEvent ev = boost::any_cast<InputKeyboardEvent>(data.data);
				
	if(ev.action == BUTTON_PRESSED)
	{
		gui->injectKeyPress( (MyGUI::KeyCode::Enum) ev.type );
		if(ev.type == KEY_W)
		{
			movementVector.z = -1;
		}
		else if(ev.type == KEY_S)
		{
			movementVector.z = 1;
		}
		else if(ev.type == KEY_A)
		{
			movementVector.x = -1;
		}
		else if(ev.type == KEY_D)
		{
			movementVector.x = 1;
		}
	}
	else
	{
		gui->injectKeyRelease( (MyGUI::KeyCode::Enum) ev.type );
		if(ev.type == KEY_W)
		{
			movementVector.z = 0;
		}
		else if(ev.type == KEY_S)
		{
			movementVector.z = 0;
		}
		else if(ev.type == KEY_A)
		{
			movementVector.x = 0;
		}
		else if(ev.type == KEY_D)
		{
			movementVector.x = 0;
		}
	}
}

void GraphicsImpl::handleMouseEvents(const DataContainer& data)
{
	InputMouseEvent ev = boost::any_cast<InputMouseEvent>(data.data);
	static bool mouseButtonPressed = false;
		
	if(ev.action == BUTTON_SAME)
	{	
		gui->injectMouseMove(ev.mouseX, ev.mouseY, 0);
			
		if(mouseButtonPressed)
		{
			camera->yaw(Ogre::Degree(-ev.mouseDeltaX * 0.13));
			camera->pitch(Ogre::Degree(-ev.mouseDeltaY * 0.13));
		}
			
	}
	else
	{
		MyGUI::MouseButton id;
			
		if(ev.type == BUTTON_MOUSE_LEFT)
		{
			id = MyGUI::MouseButton::Left;
		}
		else if(ev.type == BUTTON_MOUSE_RIGHT)
		{
			id = MyGUI::MouseButton::Right;
		}
		else
		{
			id = MyGUI::MouseButton::Middle;
		}
		
		
		if(ev.action == BUTTON_PRESSED)
		{
			mouseButtonPressed = true;
			gui->injectMousePress(ev.mouseX, ev.mouseY, id);
		}
		else
		{
			mouseButtonPressed = false;
			gui->injectMouseRelease(ev.mouseX, ev.mouseY, id);
		}
	}	
}

void GraphicsImpl::handleAppEvents(const DataContainer& data)
{
	app_event ev = boost::any_cast<app_event>(data.data);
		
	if( ev == APP_SHUTDOWN )
	{
		Dout << "graphics received shutdown event";
		this->running = false;
	}
}

void GraphicsImpl::handleObjectEvents(const DataContainer& data)
{
	ObjectToCreate node = boost::any_cast<ObjectToCreate>(data.data);
	boost::mutex::scoped_lock lock(addNodesMutex);
	nodesToAdd.push_back( node );
}

void GraphicsImpl::handleTerrainEvents(const DataContainer& data)
{
	Terrain object = boost::any_cast<Terrain>(data.data);
	boost::mutex::scoped_lock lock(addNodesMutex);
	terrainToCreate.push_back( object );
}

void GraphicsImpl::handleWorldEvents(const DataContainer& data)
{
	boost::mutex::scoped_lock lock(worldMutex);
	delete backWorld;
	backWorld = new WorldGraph(boost::any_cast<WorldGraph*>(data.data));
	newWorld = true;
}

DataContainer GraphicsImpl::getData(const DataIdentifier& id)
{
	if(id == "window.handle")
	{
		std::ostringstream windowHndStr;
		size_t windowHnd = 0;
		window->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;

		return DataContainer(windowHndStr.str());
	}
#ifdef DEBUG_BUILD
	else if(id == "ogre.sceneManager")
	{
		return DataContainer(sceneMgr);
	}
#endif
	
	return DataContainer();
}

void GraphicsImpl::loadPlugins()
{
	root->loadPlugin("./RenderSystem_GL");
	root->loadPlugin("./Plugin_OctreeSceneManager");
	root->loadPlugin("./Plugin_ParticleFX" );
}

void GraphicsImpl::setupResources()
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(resourcePath + "resources.cfg");

        // Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                // OS X does not set the working directory relative to the app,
                // In order to make things portable on OS X we need to provide
                // the loading with it's own bundle path location
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					Ogre::String(macBundlePath() + "/" + archName), typeName, secName);
#else
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
#endif
		}
	}
}

bool GraphicsImpl::configure()
{
	Dout <<"Setting Rendering Subsystem" ;
	
	Ogre::RenderSystemList renderers = root->getAvailableRenderers(); 
	if (renderers.empty()) 
	{
		Dout <<"No rendering system available" ;
		return false;
	}
	 
	root->setRenderSystem(renderers.front());
	
	Dout <<"Init Root" ;
	
	root->initialise(false);
	
	Dout <<"Create window" ;
	
	window = root->createRenderWindow ( "Manual Ogre Window", 1024, 768, false, 0 );                   // use defaults for all other values
	
	
	return true;
}

void GraphicsImpl::chooseSceneManager()
{
	sceneMgr = root->createSceneManager("TerrainSceneManager");	
}

void GraphicsImpl::createCamera()
{
	camera = sceneMgr->createCamera("MainCamera");
	// Position it at 500 in Z direction
	camera->setPosition(Ogre::Vector3(0, 50, 100));
        // Look back along -Z
	camera->lookAt(Ogre::Vector3(0, -10, 0));
	camera->setNearClipDistance(5);
	camera->setFarClipDistance(0);
}

void GraphicsImpl::createViewports()
{
	viewPort = window->addViewport( camera );
	viewPort->setBackgroundColour(Ogre::ColourValue(0,0,0));
	camera->setAspectRatio(Ogre::Real(viewPort->getActualWidth()) / Ogre::Real(viewPort->getActualHeight()));
}

void GraphicsImpl::createResourceListener()
{
}

void GraphicsImpl::loadResources()
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void GraphicsImpl::createFrameListener()
{
	listener.reset(new MyFrameListener(window, camera, sceneMgr));
	listener->showDebugOverlay(true);
	root->addFrameListener(listener.get());
	
	Ogre::WindowEventUtilities::addWindowEventListener(window, this);

	//newtonListener = new OgreNewt::BasicFrameListener( window, sceneMgr, world, 120 );
	//root->addFrameListener(newtonListener);
	
	//caelumListener = new CaelumFrameListener( window, camera );
	//root->addFrameListener(caelumListener);
}

void GraphicsImpl::createScene()
{
	// Set ambient light
	sceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	// Create a light
	Ogre::Light* l = sceneMgr->createLight("MainLight");
	l->setPosition(20,80,50);
	
	caelumSystem.reset(new Caelum::CaelumSystem (root.get(), sceneMgr, Caelum::CaelumSystem::CAELUM_COMPONENTS_DEFAULT));
	caelumSystem->getUniversalClock ()->setTimeScale (512);
	caelumSystem->setManageSceneFog(false);
	window->addListener( caelumSystem.get() );
	root->addFrameListener ( caelumSystem.get() );
}

void GraphicsImpl::addNode(const ObjectToCreate& object)
{
	Ogre::Entity* ent;
	Ogre::SceneNode* node;
	//Dout << "Creating object with specification: " + object.specification;
	ent = sceneMgr->createEntity(ObjectRegistry::Instance().getNameForID(object.node.ID) + boost::lexical_cast<std::string>(object.node.ID), object.specification);

	node = sceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(object.node.pos);
	node->setOrientation(object.node.orient);
	node->setScale(object.scale);
	nodes[object.node.ID] = node;
}

void GraphicsImpl::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);
	mainGuiWidget->setSize(width, height);
	//windowSender.postMessage( WindowMessage(WINDOW_RESIZED, width, height, left, top) );
}

bool GraphicsImpl::windowClosing(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == window )
	{
		InformationManager::Instance()->postDataToFeed("gui_event", DataContainer(EXIT_BUTTON));
	}
	return true;
}

void GraphicsImpl::updatePositions()
{
	{
		boost::mutex::scoped_lock lock(addNodesMutex);
		while (!nodesToAdd.empty())
		{
			addNode(nodesToAdd.back());
			nodesToAdd.pop_back();
		}
		while( !terrainToCreate.empty())
		{
			Terrain terrain = terrainToCreate.back();
			
			Ogre::Entity* ent;
			Ogre::SceneNode* node;
			Dout << "Creating terrain with specification: " + terrain.specification;
			ent = sceneMgr->createEntity(ObjectRegistry::Instance().getNameForID(terrain.node.ID) + boost::lexical_cast<std::string>(terrain.node.ID), terrain.specification);

			node = sceneMgr->getRootSceneNode()->createChildSceneNode();
			node->attachObject(ent);
			node->setPosition(terrain.node.pos);
			ent->setMaterialName( "Simple/BeachStones" );
			node->setOrientation(terrain.node.orient);
			node->setScale(terrain.scale);
			nodes[terrain.node.ID] = node;
			
			terrainToCreate.pop_back();
		
		}
	}
	
	if(!frontWorld->nodes.empty())
	{
		for(std::vector< OgreNewt::Node* >::iterator iter = frontWorld->nodes.begin(); iter != frontWorld->nodes.end(); ++iter)
		{
			nodes[(*iter)->ID]->setOrientation( (*iter)->orient );
			nodes[(*iter)->ID]->setPosition( (*iter)->pos );
		}
	}
}

void GraphicsImpl::setupGUI()
{
	gui.reset(new MyGUI::Gui());
	gui->initialise(window);
	
	mainGuiWidget = gui->createWidgetReal<MyGUI::Widget>("Default", 0.0, 0.0, 1.0, 1.0, MyGUI::Align::Default, "Main");
	
	MyGUI::ButtonPtr button = mainGuiWidget->createWidgetReal<MyGUI::Button>("Button", 0.005, 0.008, 0.156, 0.05, MyGUI::Align::Default, "exit");
	button->setCaption("exit");
  // set callback
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GraphicsImpl::guiCallback);
	
	button = mainGuiWidget->createWidgetReal<MyGUI::Button>("Button", 0.182, 0.008, 0.156, 0.05, MyGUI::Align::Default, "do");
	button->setCaption("do something");
  // set callback
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GraphicsImpl::guiCallback);
}

void GraphicsImpl::guiCallback(MyGUI::WidgetPtr sender)
{
	std::string name = sender->getName();
	if(name == "do")
	{
		InformationManager::Instance()->postDataToFeed("gui_event", DataContainer(DO_BUTTON));
	}
	if(name == "exit")
	{
		InformationManager::Instance()->postDataToFeed("gui_event", DataContainer(EXIT_BUTTON));
	}
	
}