# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dio/Begin/JsonRpcFramework/source/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dio/Begin/JsonRpcFramework/source/test/Build

# Include any dependencies generated for this target.
include CMakeFiles/serverTest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/serverTest.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/serverTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/serverTest.dir/flags.make

CMakeFiles/serverTest.dir/server.cpp.o: CMakeFiles/serverTest.dir/flags.make
CMakeFiles/serverTest.dir/server.cpp.o: /home/dio/Begin/JsonRpcFramework/source/test/server.cpp
CMakeFiles/serverTest.dir/server.cpp.o: CMakeFiles/serverTest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dio/Begin/JsonRpcFramework/source/test/Build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/serverTest.dir/server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/serverTest.dir/server.cpp.o -MF CMakeFiles/serverTest.dir/server.cpp.o.d -o CMakeFiles/serverTest.dir/server.cpp.o -c /home/dio/Begin/JsonRpcFramework/source/test/server.cpp

CMakeFiles/serverTest.dir/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/serverTest.dir/server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dio/Begin/JsonRpcFramework/source/test/server.cpp > CMakeFiles/serverTest.dir/server.cpp.i

CMakeFiles/serverTest.dir/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/serverTest.dir/server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dio/Begin/JsonRpcFramework/source/test/server.cpp -o CMakeFiles/serverTest.dir/server.cpp.s

# Object files for target serverTest
serverTest_OBJECTS = \
"CMakeFiles/serverTest.dir/server.cpp.o"

# External object files for target serverTest
serverTest_EXTERNAL_OBJECTS =

serverTest: CMakeFiles/serverTest.dir/server.cpp.o
serverTest: CMakeFiles/serverTest.dir/build.make
serverTest: CMakeFiles/serverTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/dio/Begin/JsonRpcFramework/source/test/Build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable serverTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/serverTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/serverTest.dir/build: serverTest
.PHONY : CMakeFiles/serverTest.dir/build

CMakeFiles/serverTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/serverTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/serverTest.dir/clean

CMakeFiles/serverTest.dir/depend:
	cd /home/dio/Begin/JsonRpcFramework/source/test/Build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dio/Begin/JsonRpcFramework/source/test /home/dio/Begin/JsonRpcFramework/source/test /home/dio/Begin/JsonRpcFramework/source/test/Build /home/dio/Begin/JsonRpcFramework/source/test/Build /home/dio/Begin/JsonRpcFramework/source/test/Build/CMakeFiles/serverTest.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/serverTest.dir/depend

