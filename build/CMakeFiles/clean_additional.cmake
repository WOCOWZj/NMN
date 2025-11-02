# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\NMN_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\NMN_autogen.dir\\ParseCache.txt"
  "NMN_autogen"
  )
endif()
