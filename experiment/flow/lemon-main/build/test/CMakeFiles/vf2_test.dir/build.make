# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build

# Include any dependencies generated for this target.
include test/CMakeFiles/vf2_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/vf2_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/vf2_test.dir/flags.make

test/CMakeFiles/vf2_test.dir/vf2_test.cc.o: test/CMakeFiles/vf2_test.dir/flags.make
test/CMakeFiles/vf2_test.dir/vf2_test.cc.o: ../test/vf2_test.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/CMakeFiles/vf2_test.dir/vf2_test.cc.o"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/vf2_test.dir/vf2_test.cc.o -c /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test/vf2_test.cc

test/CMakeFiles/vf2_test.dir/vf2_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vf2_test.dir/vf2_test.cc.i"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test/vf2_test.cc > CMakeFiles/vf2_test.dir/vf2_test.cc.i

test/CMakeFiles/vf2_test.dir/vf2_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vf2_test.dir/vf2_test.cc.s"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test/vf2_test.cc -o CMakeFiles/vf2_test.dir/vf2_test.cc.s

test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.requires:
.PHONY : test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.requires

test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.provides: test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.requires
	$(MAKE) -f test/CMakeFiles/vf2_test.dir/build.make test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.provides.build
.PHONY : test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.provides

test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.provides.build: test/CMakeFiles/vf2_test.dir/vf2_test.cc.o

# Object files for target vf2_test
vf2_test_OBJECTS = \
"CMakeFiles/vf2_test.dir/vf2_test.cc.o"

# External object files for target vf2_test
vf2_test_EXTERNAL_OBJECTS =

test/vf2_test: test/CMakeFiles/vf2_test.dir/vf2_test.cc.o
test/vf2_test: test/CMakeFiles/vf2_test.dir/build.make
test/vf2_test: lemon/libemon.a
test/vf2_test: test/CMakeFiles/vf2_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable vf2_test"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vf2_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/vf2_test.dir/build: test/vf2_test
.PHONY : test/CMakeFiles/vf2_test.dir/build

test/CMakeFiles/vf2_test.dir/requires: test/CMakeFiles/vf2_test.dir/vf2_test.cc.o.requires
.PHONY : test/CMakeFiles/vf2_test.dir/requires

test/CMakeFiles/vf2_test.dir/clean:
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && $(CMAKE_COMMAND) -P CMakeFiles/vf2_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/vf2_test.dir/clean

test/CMakeFiles/vf2_test.dir/depend:
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test/CMakeFiles/vf2_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/vf2_test.dir/depend

