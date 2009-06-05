ADD_SUBDIRECTORY(physics)
ADD_SUBDIRECTORY(graphics)

#list all source files here

ADD_EXECUTABLE(ote main.cpp input.cpp game.cpp objectregistry.cpp resourcemanager.cpp)

ADD_EXECUTABLE(serializer serialize.cpp)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(ote OgreNewt OgreMain Newton taskengine boost_thread-gcc43-mt-1_38 log4cpp boost_system-gcc43-mt-1_38 boost_serialization-gcc43-mt-1_38 boost_log-gcc43-mt-1_38 OIS mygui ote_physics ote_graphics Caelum)
 
TARGET_LINK_LIBRARIES(serializer boost_serialization-gcc43-mt-1_38 boost_system-gcc43-mt-1_38)