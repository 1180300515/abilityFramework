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
CMAKE_BINARY_DIR = /Users/luozhaoxin/Documents/AbilityFrameworkCpp/build

# Include any dependencies generated for this target.
include CMakeFiles/AbilityFramework.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/AbilityFramework.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/AbilityFramework.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/AbilityFramework.dir/flags.make

CMakeFiles/AbilityFramework.dir/src/main.cpp.o: CMakeFiles/AbilityFramework.dir/flags.make
CMakeFiles/AbilityFramework.dir/src/main.cpp.o: /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/main.cpp
CMakeFiles/AbilityFramework.dir/src/main.cpp.o: CMakeFiles/AbilityFramework.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/luozhaoxin/Documents/AbilityFrameworkCpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/AbilityFramework.dir/src/main.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AbilityFramework.dir/src/main.cpp.o -MF CMakeFiles/AbilityFramework.dir/src/main.cpp.o.d -o CMakeFiles/AbilityFramework.dir/src/main.cpp.o -c /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/main.cpp

CMakeFiles/AbilityFramework.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AbilityFramework.dir/src/main.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/main.cpp > CMakeFiles/AbilityFramework.dir/src/main.cpp.i

CMakeFiles/AbilityFramework.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AbilityFramework.dir/src/main.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/main.cpp -o CMakeFiles/AbilityFramework.dir/src/main.cpp.s

CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o: CMakeFiles/AbilityFramework.dir/flags.make
CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o: /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/plugincore/pluginload.cpp
CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o: CMakeFiles/AbilityFramework.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/luozhaoxin/Documents/AbilityFrameworkCpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o -MF CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o.d -o CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o -c /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/plugincore/pluginload.cpp

CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/plugincore/pluginload.cpp > CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.i

CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/plugincore/pluginload.cpp -o CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.s

CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o: CMakeFiles/AbilityFramework.dir/flags.make
CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o: /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/eventhandler/eventhandler.cpp
CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o: CMakeFiles/AbilityFramework.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/luozhaoxin/Documents/AbilityFrameworkCpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o -MF CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o.d -o CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o -c /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/eventhandler/eventhandler.cpp

CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/eventhandler/eventhandler.cpp > CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.i

CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/luozhaoxin/Documents/AbilityFrameworkCpp/src/eventhandler/eventhandler.cpp -o CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.s

# Object files for target AbilityFramework
AbilityFramework_OBJECTS = \
"CMakeFiles/AbilityFramework.dir/src/main.cpp.o" \
"CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o" \
"CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o"

# External object files for target AbilityFramework
AbilityFramework_EXTERNAL_OBJECTS =

bin/AbilityFramework: CMakeFiles/AbilityFramework.dir/src/main.cpp.o
bin/AbilityFramework: CMakeFiles/AbilityFramework.dir/src/plugincore/pluginload.cpp.o
bin/AbilityFramework: CMakeFiles/AbilityFramework.dir/src/eventhandler/eventhandler.cpp.o
bin/AbilityFramework: CMakeFiles/AbilityFramework.dir/build.make
bin/AbilityFramework: /opt/homebrew/lib/libyaml-cpp.0.7.0.dylib
bin/AbilityFramework: CMakeFiles/AbilityFramework.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/luozhaoxin/Documents/AbilityFrameworkCpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable bin/AbilityFramework"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/AbilityFramework.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/AbilityFramework.dir/build: bin/AbilityFramework
.PHONY : CMakeFiles/AbilityFramework.dir/build

CMakeFiles/AbilityFramework.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/AbilityFramework.dir/cmake_clean.cmake
.PHONY : CMakeFiles/AbilityFramework.dir/clean

CMakeFiles/AbilityFramework.dir/depend:
	cd /Users/luozhaoxin/Documents/AbilityFrameworkCpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/luozhaoxin/Documents/AbilityFrameworkCpp /Users/luozhaoxin/Documents/AbilityFrameworkCpp /Users/luozhaoxin/Documents/AbilityFrameworkCpp/build /Users/luozhaoxin/Documents/AbilityFrameworkCpp/build /Users/luozhaoxin/Documents/AbilityFrameworkCpp/build/CMakeFiles/AbilityFramework.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/AbilityFramework.dir/depend
