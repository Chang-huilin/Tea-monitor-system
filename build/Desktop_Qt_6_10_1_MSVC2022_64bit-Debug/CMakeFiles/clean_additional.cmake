# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\teaSYS_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\teaSYS_autogen.dir\\ParseCache.txt"
  "teaSYS_autogen"
  )
endif()
