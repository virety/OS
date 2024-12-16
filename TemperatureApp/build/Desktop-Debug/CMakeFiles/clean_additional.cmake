# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/TemperatureApp_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/TemperatureApp_autogen.dir/ParseCache.txt"
  "TemperatureApp_autogen"
  )
endif()
