cmake_minimum_required(VERSION 2.6)

PROJECT(ote)

#if you don't want the full compiler output, remove the following line
SET(DEBUG_BUILD ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(LIB_INCLUDE_DIR /home/mononofu/Programmieren/include)

#add definitions, compiler switches, etc.
IF(DEBUG_BUILD)
	ADD_DEFINITIONS(-Wall -O0 -g3)
ELSE()
	ADD_DEFINITIONS(-Wall -O3 -g0)
ENDIF()

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${OTE_SOURCE_DIR}/inc ${LIB_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${LIB_INCLUDE_DIR}/OGRE ${LIB_INCLUDE_DIR}/MYGUI ${LIB_INCLUDE_DIR}/Caelum)
INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${LIB_INCLUDE_DIR}/OgreNewt ${LIB_INCLUDE_DIR}/newton)

LINK_DIRECTORIES(/home/mononofu/Programmieren/lib /usr/local/lib)

ADD_SUBDIRECTORY(src)