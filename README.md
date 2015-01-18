# proj2cmake
Convert an entire Visual Studio solution and all contained projects to CMake.

The output is split into multiple files:
 - CMake files that contain the lists of sources and dependencies between the projects in the solution
 - CMake files that can be edited by the user and where the platform specific settings and compiler settings belong

Only the first onces will be overwritten on further runs of proj2cmake. So it is designed to keep the CMake files in sync with the VS solution.

This project is currently in an early state and may not work correctly for your Visual Studio projects.

## Usage
Call proj2cmake with your solution file as the argument:
```
# create cmake files
proj2cmake ~/projects/MyMsVcProject/MyMsVcProject.sln
# edit the settings
vim ~/projects/MyMsVcProject/cmake_config/MyMsVcProject.cmake
# run cmake
cmake /projects/MyMsVcProject
```

## License
Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
