#INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${LIB_INCLUDE_DIR}/OGRE ${LIB_INCLUDE_DIR}/MYGUI ${LIB_INCLUDE_DIR}/Caelum})

#build a shared library
ADD_LIBRARY(ote_graphics SHARED graphics.cpp listener.cpp)

TARGET_LINK_LIBRARIES(ote_graphics MyGUI.OgrePlatform MyGUIEngine)