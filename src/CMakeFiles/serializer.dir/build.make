# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mononofu/Programmieren/OTE

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mononofu/Programmieren/OTE

# Include any dependencies generated for this target.
include src/CMakeFiles/serializer.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/serializer.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/serializer.dir/flags.make

src/CMakeFiles/serializer.dir/serialize.cpp.o: src/CMakeFiles/serializer.dir/flags.make
src/CMakeFiles/serializer.dir/serialize.cpp.o: src/serialize.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/mononofu/Programmieren/OTE/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/serializer.dir/serialize.cpp.o"
	cd /home/mononofu/Programmieren/OTE/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/serializer.dir/serialize.cpp.o -c /home/mononofu/Programmieren/OTE/src/serialize.cpp

src/CMakeFiles/serializer.dir/serialize.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/serializer.dir/serialize.cpp.i"
	cd /home/mononofu/Programmieren/OTE/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/mononofu/Programmieren/OTE/src/serialize.cpp > CMakeFiles/serializer.dir/serialize.cpp.i

src/CMakeFiles/serializer.dir/serialize.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/serializer.dir/serialize.cpp.s"
	cd /home/mononofu/Programmieren/OTE/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/mononofu/Programmieren/OTE/src/serialize.cpp -o CMakeFiles/serializer.dir/serialize.cpp.s

src/CMakeFiles/serializer.dir/serialize.cpp.o.requires:
.PHONY : src/CMakeFiles/serializer.dir/serialize.cpp.o.requires

src/CMakeFiles/serializer.dir/serialize.cpp.o.provides: src/CMakeFiles/serializer.dir/serialize.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/serializer.dir/build.make src/CMakeFiles/serializer.dir/serialize.cpp.o.provides.build
.PHONY : src/CMakeFiles/serializer.dir/serialize.cpp.o.provides

src/CMakeFiles/serializer.dir/serialize.cpp.o.provides.build: src/CMakeFiles/serializer.dir/serialize.cpp.o
.PHONY : src/CMakeFiles/serializer.dir/serialize.cpp.o.provides.build

# Object files for target serializer
serializer_OBJECTS = \
"CMakeFiles/serializer.dir/serialize.cpp.o"

# External object files for target serializer
serializer_EXTERNAL_OBJECTS =

src/serializer: src/CMakeFiles/serializer.dir/serialize.cpp.o
src/serializer: src/CMakeFiles/serializer.dir/build.make
src/serializer: src/CMakeFiles/serializer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable serializer"
	cd /home/mononofu/Programmieren/OTE/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/serializer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/serializer.dir/build: src/serializer
.PHONY : src/CMakeFiles/serializer.dir/build

src/CMakeFiles/serializer.dir/requires: src/CMakeFiles/serializer.dir/serialize.cpp.o.requires
.PHONY : src/CMakeFiles/serializer.dir/requires

src/CMakeFiles/serializer.dir/clean:
	cd /home/mononofu/Programmieren/OTE/src && $(CMAKE_COMMAND) -P CMakeFiles/serializer.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/serializer.dir/clean

src/CMakeFiles/serializer.dir/depend:
	cd /home/mononofu/Programmieren/OTE && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mononofu/Programmieren/OTE /home/mononofu/Programmieren/OTE/src /home/mononofu/Programmieren/OTE /home/mononofu/Programmieren/OTE/src /home/mononofu/Programmieren/OTE/src/CMakeFiles/serializer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/serializer.dir/depend

