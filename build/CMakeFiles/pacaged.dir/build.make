# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/dio/Begin/JsonRpcFramework/example/FutureTest

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dio/Begin/JsonRpcFramework/build

# Include any dependencies generated for this target.
include CMakeFiles/pacaged.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/pacaged.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/pacaged.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pacaged.dir/flags.make

CMakeFiles/pacaged.dir/packagedtest.cpp.o: CMakeFiles/pacaged.dir/flags.make
CMakeFiles/pacaged.dir/packagedtest.cpp.o: /home/dio/Begin/JsonRpcFramework/example/FutureTest/packagedtest.cpp
CMakeFiles/pacaged.dir/packagedtest.cpp.o: CMakeFiles/pacaged.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dio/Begin/JsonRpcFramework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/pacaged.dir/packagedtest.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/pacaged.dir/packagedtest.cpp.o -MF CMakeFiles/pacaged.dir/packagedtest.cpp.o.d -o CMakeFiles/pacaged.dir/packagedtest.cpp.o -c /home/dio/Begin/JsonRpcFramework/example/FutureTest/packagedtest.cpp

CMakeFiles/pacaged.dir/packagedtest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pacaged.dir/packagedtest.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dio/Begin/JsonRpcFramework/example/FutureTest/packagedtest.cpp > CMakeFiles/pacaged.dir/packagedtest.cpp.i

CMakeFiles/pacaged.dir/packagedtest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pacaged.dir/packagedtest.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dio/Begin/JsonRpcFramework/example/FutureTest/packagedtest.cpp -o CMakeFiles/pacaged.dir/packagedtest.cpp.s

# Object files for target pacaged
pacaged_OBJECTS = \
"CMakeFiles/pacaged.dir/packagedtest.cpp.o"

# External object files for target pacaged
pacaged_EXTERNAL_OBJECTS =

pacaged: CMakeFiles/pacaged.dir/packagedtest.cpp.o
pacaged: CMakeFiles/pacaged.dir/build.make
pacaged: CMakeFiles/pacaged.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dio/Begin/JsonRpcFramework/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable pacaged"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pacaged.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pacaged.dir/build: pacaged
.PHONY : CMakeFiles/pacaged.dir/build

CMakeFiles/pacaged.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pacaged.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pacaged.dir/clean

CMakeFiles/pacaged.dir/depend:
	cd /home/dio/Begin/JsonRpcFramework/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dio/Begin/JsonRpcFramework/example/FutureTest /home/dio/Begin/JsonRpcFramework/example/FutureTest /home/dio/Begin/JsonRpcFramework/build /home/dio/Begin/JsonRpcFramework/build /home/dio/Begin/JsonRpcFramework/build/CMakeFiles/pacaged.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/pacaged.dir/depend

