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
include test/CMakeFiles/hao_orlin_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/hao_orlin_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/hao_orlin_test.dir/flags.make

test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o: test/CMakeFiles/hao_orlin_test.dir/flags.make
test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o: ../test/hao_orlin_test.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o -c /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test/hao_orlin_test.cc

test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.i"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test/hao_orlin_test.cc > CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.i

test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.s"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test/hao_orlin_test.cc -o CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.s

test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.requires:
.PHONY : test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.requires

test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.provides: test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.requires
	$(MAKE) -f test/CMakeFiles/hao_orlin_test.dir/build.make test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.provides.build
.PHONY : test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.provides

test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.provides.build: test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o

# Object files for target hao_orlin_test
hao_orlin_test_OBJECTS = \
"CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o"

# External object files for target hao_orlin_test
hao_orlin_test_EXTERNAL_OBJECTS =

test/hao_orlin_test: test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o
test/hao_orlin_test: test/CMakeFiles/hao_orlin_test.dir/build.make
test/hao_orlin_test: lemon/libemon.a
test/hao_orlin_test: test/CMakeFiles/hao_orlin_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable hao_orlin_test"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hao_orlin_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/hao_orlin_test.dir/build: test/hao_orlin_test
.PHONY : test/CMakeFiles/hao_orlin_test.dir/build

test/CMakeFiles/hao_orlin_test.dir/requires: test/CMakeFiles/hao_orlin_test.dir/hao_orlin_test.cc.o.requires
.PHONY : test/CMakeFiles/hao_orlin_test.dir/requires

test/CMakeFiles/hao_orlin_test.dir/clean:
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test && $(CMAKE_COMMAND) -P CMakeFiles/hao_orlin_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/hao_orlin_test.dir/clean

test/CMakeFiles/hao_orlin_test.dir/depend:
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/test /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/test/CMakeFiles/hao_orlin_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/hao_orlin_test.dir/depend

