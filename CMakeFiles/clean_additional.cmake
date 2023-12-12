# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\FinalProject_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\FinalProject_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\StaticGLEW_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\StaticGLEW_autogen.dir\\ParseCache.txt"
  "FinalProject_autogen"
  "StaticGLEW_autogen"
  )
endif()
