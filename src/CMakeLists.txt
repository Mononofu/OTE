ADD_SUBDIRECTORY(physics)
ADD_SUBDIRECTORY(graphics)

#list all source files here

ADD_EXECUTABLE(ote main.cpp input.cpp game.cpp objectregistry.cpp resourcemanager.cpp settingsmanager.cpp)

ADD_EXECUTABLE(serializer serialize.cpp)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(ote OgreMain Newton taskengine boost_thread log4cpp boost_system boost_serialization boost_log boost_log_setup OIS ote_physics ote_graphics Caelum)
 
TARGET_LINK_LIBRARIES(serializer boost_serialization boost_system)
