//
// C++ Implementation: game
//
// Description:
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "game.h"
#include "objectregistry.h"
#include <boost/shared_ptr.hpp>
#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/mpl/list.hpp>
#include <ctime> 
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include "FeedDataTypes.h"

namespace fs = boost::filesystem;
namespace ar = boost::archive;
namespace sc = boost::statechart;
namespace mpl = boost::mpl;

struct Active;
struct GameState : sc::state_machine< GameState, Active > {};

struct Loading;
struct Inactive;
struct EvAppQuit : sc::event< EvAppQuit > {};
struct Active : sc::simple_state< Active, GameState, Loading>
{
	~Active();
	typedef sc::transition< EvAppQuit, Inactive > reactions;
};

struct Inactive : sc::simple_state< Inactive, GameState>
{
};

struct MainMenu;
struct EvFinishedLoading : sc::event< EvFinishedLoading > {};
struct Loading : sc::simple_state< Loading, Active >
{
	Loading();
	~Loading();
	typedef sc::transition< EvFinishedLoading, MainMenu > reactions;
};

struct MainGame : sc::simple_state< MainGame, Active >
{
public:
	MainGame();
};
struct Settings : sc::simple_state< Settings, Active > {};
struct EvMainGameStarted : sc::event< EvMainGameStarted > {};
struct EvSettingsDialogLaunched : sc::event< EvSettingsDialogLaunched > {};
struct MainMenu : sc::simple_state< MainMenu, Active >
{
	typedef mpl::list<
	sc::transition< EvMainGameStarted, MainGame >,
	sc::transition< EvSettingsDialogLaunched, Settings >
	> reactions;
};


class GameImpl : public Task
{
	public:
		GameImpl();
		~GameImpl();
		void handleKeyEvents ( const DataContainer& data );
		void handleGUIEvents ( const DataContainer& data );
		void handleThreadEvents ( const DataContainer& data );
		void handleAppEvents ( const DataContainer& data );
		bool doStep();
		void threadWillStart();
		void threadWillStop();
	private:
		int loadingThreads;
		GameState myState;
		CameraPosition camPos;
};

Game::Game() : impl(new GameImpl) { }
Game::~Game() { }
bool Game::doStep() { return impl->step(); }
void Game::threadWillStart() { impl->threadWillStart(); }
void Game::threadWillStop() { impl->threadWillStop(); }

Active::~Active()
{
	InformationManager::Instance()->postDataToFeed ( "app_event", DataContainer ( APP_SHUTDOWN ) );
}

Loading::Loading()
{
	InformationManager::Instance()->postDataToFeed ( "app_event", DataContainer ( APP_STARTING ) );
}

Loading::~Loading()
{
	InformationManager::Instance()->postDataToFeed ( "app_event", DataContainer ( APP_STARTED ) );
}

MainGame::MainGame()
{
	Terrain terrain;
	{
		boost::filesystem::ifstream file ( "Media/custom/simple.terrain" );
		ar::text_iarchive ia ( file );
		ia >> terrain;
	}
	terrain.node.ID = ObjectRegistry::Instance().addObject ( terrain.specification );
	InformationManager::Instance()->postDataToFeed ( "create_terrain", DataContainer ( terrain ) );
}

void GameImpl::handleKeyEvents ( const DataContainer& data )
{
	InputKeyboardEvent ev = boost::any_cast<InputKeyboardEvent> ( data.data );
	if ( ev.type == KEY_UP && ev.action == BUTTON_PRESSED )
	{
		camPos.position += Ogre::Vector3 ( 0.0, 0.0, 1.0 );
		InformationManager::Instance()->postDataToFeed ( "camera_position", DataContainer ( APP_SHUTDOWN ) );
	}
	else if ( ( ev.type == KEY_Q || ev.type == KEY_ESCAPE ) && ev.action == BUTTON_PRESSED )
	{
		myState.process_event ( EvAppQuit() );
	}
}


void GameImpl::handleGUIEvents ( const DataContainer& data )
{
	gui_event ev = boost::any_cast<gui_event> ( data.data );
	if ( ev == DO_BUTTON )
	{
		boost::minstd_rand generator( static_cast<unsigned int>(std::time(0)) );
		boost::uniform_real<> uni_dist(-2,2);
		boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > uni(generator, uni_dist);
		
		int numOfObjects = 5;
		int spaceInBetween = 20;
		for ( int i = 0; i < numOfObjects; i++ )
		{
			for ( int j = 0; j < numOfObjects; j++ )
			{
				for ( int k = 0; k < numOfObjects; k++ )
				{
					boost::shared_ptr<ObjectToCreate> obj(new ObjectToCreate);
					obj->node.pos = Ogre::Vector3 ( spaceInBetween/2 * numOfObjects - i * spaceInBetween + uni(), spaceInBetween/2 * numOfObjects - k * spaceInBetween + uni(), spaceInBetween/2 * numOfObjects - j * spaceInBetween + uni() );
					obj->scale = Ogre::Vector3 ( 0.2,0.2,0.2 );
					obj->node.orient = Ogre::Quaternion();
					obj->node.ID = ObjectRegistry::Instance().addObject ( "ogrehead" );
					obj->specification = "ogrehead.mesh";
					InformationManager::Instance()->postDataToFeed ( "create_object", DataContainer ( obj ) );
				}
			}
		}
		myState.process_event ( EvMainGameStarted() );
	}
	else if ( ev == EXIT_BUTTON )
	{
		myState.process_event ( EvAppQuit() );
	}
}


void GameImpl::handleThreadEvents ( const DataContainer& data )
{
	thread_event ev = boost::any_cast<thread_event> ( data.data );

	if ( ev == THREAD_STARTING )
	{
		++loadingThreads;
	}
	else if ( ev == THREAD_STARTED )
	{
		--loadingThreads;
		if ( loadingThreads < 1 )
		{
			myState.process_event ( EvFinishedLoading() );
		}
	}
}


void GameImpl::handleAppEvents ( const DataContainer& data )
{
	app_event ev = boost::any_cast<app_event> ( data.data );

	if ( ev == APP_SHUTDOWN )
	{
		Dout << "game received shutdown event";
		this->running = false;
	}
}

GameImpl::GameImpl() : loadingThreads ( 0 )
{
	myState.initiate();
}

GameImpl::~GameImpl()
{

}

bool GameImpl::doStep()
{
#ifndef DEBUG_BUILD
	boost::this_thread::sleep ( boost::posix_time::milliseconds ( 10 ) );		// Wait 1/100 s
#endif
	return this->running;
}

void GameImpl::threadWillStart()
{
	InformationManager::Instance()->subscribeToFeed ( "app_event", boost::bind ( &GameImpl::handleAppEvents, this, _1 ) );
	InformationManager::Instance()->subscribeToFeed ( "thread_event", boost::bind ( &GameImpl::handleThreadEvents, this, _1 ) );
	InformationManager::Instance()->subscribeToFeed ( "input_keyboard", boost::bind ( &GameImpl::handleKeyEvents, this, _1 ) );
	InformationManager::Instance()->subscribeToFeed ( "gui_event", boost::bind ( &GameImpl::handleGUIEvents, this, _1 ) );
}

void GameImpl::threadWillStop()
{

}

