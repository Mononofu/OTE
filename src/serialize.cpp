#include "FeedDataTypes.h"
#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>

namespace fs = boost::filesystem;
namespace ar = boost::archive;

int main()
{
	OgreNewt::Node node;
	std::cout << sizeof(node) << std::endl;
	Terrain terrain;
	terrain.node.pos = Ogre::Vector3(0,0,0);
	terrain.scale = Ogre::Vector3(2.0, 2.0, 2.0);
	terrain.node.ID = -1;
	terrain.specification = "simple_terrain.mesh";
	
	fs::ofstream file("Media/custom/simple.terrain");
	ar::text_oarchive oa(file);
	oa << terrain;
}