# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

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
include CMakeFiles/ote.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ote.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ote.dir/flags.make

CMakeFiles/ote.dir/main.o: CMakeFiles/ote.dir/flags.make
CMakeFiles/ote.dir/main.o: main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/mononofu/Programmieren/OTE/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/ote.dir/main.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ote.dir/main.o -c /home/mononofu/Programmieren/OTE/main.cpp

CMakeFiles/ote.dir/main.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ote.dir/main.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/mononofu/Programmieren/OTE/main.cpp > CMakeFiles/ote.dir/main.i

CMakeFiles/ote.dir/main.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ote.dir/main.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/mononofu/Programmieren/OTE/main.cpp -o CMakeFiles/ote.dir/main.s

CMakeFiles/ote.dir/main.o.requires:
.PHONY : CMakeFiles/ote.dir/main.o.requires

CMakeFiles/ote.dir/main.o.provides: CMakeFiles/ote.dir/main.o.requires
	$(MAKE) -f CMakeFiles/ote.dir/build.make CMakeFiles/ote.dir/main.o.provides.build
.PHONY : CMakeFiles/ote.dir/main.o.provides

CMakeFiles/ote.dir/main.o.provides.build: CMakeFiles/ote.dir/main.o
.PHONY : CMakeFiles/ote.dir/main.o.provides.build

# Object files for target ote
ote_OBJECTS = \
"CMakeFiles/ote.dir/main.o"

# External object files for target ote
ote_EXTERNAL_OBJECTS =

ote: CMakeFiles/ote.dir/main.o
ote: CMakeFiles/ote.dir/build.make
ote: CMakeFiles/ote.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ote"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ote.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ote.dir/build: ote
.PHONY : CMakeFiles/ote.dir/build

CMakeFiles/ote.dir/requires: CMakeFiles/ote.dir/main.o.requires
.PHONY : CMakeFiles/ote.dir/requires

CMakeFiles/ote.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ote.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ote.dir/clean

CMakeFiles/ote.dir/depend:
	cd /home/mononofu/Programmieren/OTE && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mononofu/Programmieren/OTE /home/mononofu/Programmieren/OTE /home/mononofu/Programmieren/OTE /home/mononofu/Programmieren/OTE /home/mononofu/Programmieren/OTE/CMakeFiles/ote.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ote.dir/depend

