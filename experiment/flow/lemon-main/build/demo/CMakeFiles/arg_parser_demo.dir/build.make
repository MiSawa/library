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
include demo/CMakeFiles/arg_parser_demo.dir/depend.make

# Include the progress variables for this target.
include demo/CMakeFiles/arg_parser_demo.dir/progress.make

# Include the compile flags for this target's objects.
include demo/CMakeFiles/arg_parser_demo.dir/flags.make

demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o: demo/CMakeFiles/arg_parser_demo.dir/flags.make
demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o: ../demo/arg_parser_demo.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/demo && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o -c /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/demo/arg_parser_demo.cc

demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.i"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/demo && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/demo/arg_parser_demo.cc > CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.i

demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.s"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/demo && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/demo/arg_parser_demo.cc -o CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.s

demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.requires:
.PHONY : demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.requires

demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.provides: demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.requires
	$(MAKE) -f demo/CMakeFiles/arg_parser_demo.dir/build.make demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.provides.build
.PHONY : demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.provides

demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.provides.build: demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o

# Object files for target arg_parser_demo
arg_parser_demo_OBJECTS = \
"CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o"

# External object files for target arg_parser_demo
arg_parser_demo_EXTERNAL_OBJECTS =

demo/arg_parser_demo: demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o
demo/arg_parser_demo: demo/CMakeFiles/arg_parser_demo.dir/build.make
demo/arg_parser_demo: lemon/libemon.a
demo/arg_parser_demo: demo/CMakeFiles/arg_parser_demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable arg_parser_demo"
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/demo && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/arg_parser_demo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
demo/CMakeFiles/arg_parser_demo.dir/build: demo/arg_parser_demo
.PHONY : demo/CMakeFiles/arg_parser_demo.dir/build

demo/CMakeFiles/arg_parser_demo.dir/requires: demo/CMakeFiles/arg_parser_demo.dir/arg_parser_demo.cc.o.requires
.PHONY : demo/CMakeFiles/arg_parser_demo.dir/requires

demo/CMakeFiles/arg_parser_demo.dir/clean:
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/demo && $(CMAKE_COMMAND) -P CMakeFiles/arg_parser_demo.dir/cmake_clean.cmake
.PHONY : demo/CMakeFiles/arg_parser_demo.dir/clean

demo/CMakeFiles/arg_parser_demo.dir/depend:
	cd /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/demo /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/demo /home/misawa/Dropbox/Program/template/git/experiment/flow/lemon-main/build/demo/CMakeFiles/arg_parser_demo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : demo/CMakeFiles/arg_parser_demo.dir/depend

