# CMakeList.txt : CMake project for fundsAPI, include source and define
# project specific logic here.
#



cmake_minimum_required (VERSION 3.14)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("fundsAPI")

set(BUILD_SHARED_LIBS OFF)
include (FetchContent)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 1.10.5)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3)
FetchContent_MakeAvailable(json)







# Add source to this project's executable.
add_executable (fundsAPI "fundsAPI.cpp" "fundsAPI.h")

target_link_libraries(fundsAPI PRIVATE cpr::cpr nlohmann_json::nlohmann_json)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET fundsAPI PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.



