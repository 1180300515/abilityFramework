# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.26.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.26.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/luozhaoxin/Documents/AbilityFrameworkCpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/luozhaoxin/Documents/AbilityFrameworkCpp

# Include any dependencies generated for this target.
include CMakeFiles/Module_A.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Module_A.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Module_A.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Module_A.dir/flags.make

CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o: CMakeFiles/Module_A.dir/flags.make
CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o: src/plugincore/pluginload.cpp
CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o: CMakeFiles/Module_A.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/luozhaoxin/Documents/AbilityFrameworkCpp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o -MF CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o.d -o CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o -c /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/plugincore/pluginload.cpp

CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/plugincore/pluginload.cpp > CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.i

CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/plugincore/pluginload.cpp -o CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.s

# Object files for target Module_A
Module_A_OBJECTS = \
"CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o"

# External object files for target Module_A
Module_A_EXTERNAL_OBJECTS =

lib/modules/libModule_A.a: CMakeFiles/Module_A.dir/src/plugincore/pluginload.cpp.o
lib/modules/libModule_A.a: CMakeFiles/Module_A.dir/build.make
lib/modules/libModule_A.a: CMakeFiles/Module_A.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/luozhaoxin/Documents/AbilityFrameworkCpp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library lib/modules/libModule_A.a"
	$(CMAKE_COMMAND) -P CMakeFiles/Module_A.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Module_A.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Module_A.dir/build: lib/modules/libModule_A.a
.PHONY : CMakeFiles/Module_A.dir/build

CMakeFiles/Module_A.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Module_A.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Module_A.dir/clean

CMakeFiles/Module_A.dir/depend:
	cd /Users/luozhaoxin/Documents/AbilityFrameworkCpp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/luozhaoxin/Documents/AbilityFrameworkCpp /Users/luozhaoxin/Documents/AbilityFrameworkCpp /Users/luozhaoxin/Documents/AbilityFrameworkCpp /Users/luozhaoxin/Documents/AbilityFrameworkCpp /Users/luozhaoxin/Documents/AbilityFrameworkCpp/CMakeFiles/Module_A.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Module_A.dir/depend

