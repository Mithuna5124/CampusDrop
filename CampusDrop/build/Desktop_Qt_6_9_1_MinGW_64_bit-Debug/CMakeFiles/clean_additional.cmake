# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CampusDrop_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CampusDrop_autogen.dir\\ParseCache.txt"
  "CampusDrop_autogen"
  )
endif()
