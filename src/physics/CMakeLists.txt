#INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${LIB_INCLUDE_DIR}/OgreNewt ${LIB_INCLUDE_DIR}/newton)

#build a shared library
ADD_LIBRARY(ote_physics SHARED OgreNewt_BasicFrameListener.cpp OgreNewt_BasicJoints.cpp OgreNewt_Body.cpp OgreNewt_BodyInAABBIterator.cpp OgreNewt_Collision.cpp OgreNewt_CollisionPrimitives.cpp OgreNewt_CollisionSerializer.cpp OgreNewt_ContactCallback.cpp OgreNewt_ContactJoint.cpp OgreNewt_Debugger.cpp OgreNewt_Joint.cpp OgreNewt_MaterialID.cpp OgreNewt_MaterialPair.cpp OgreNewt_PlayerController.cpp OgreNewt_RayCast.cpp OgreNewt_Tools.cpp OgreNewt_Vehicle.cpp OgreNewt_World.cpp physics.cpp
)
