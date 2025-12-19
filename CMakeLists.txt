# CMakeList.txt : CMake project for project-audio, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("project-audio")

# Add source to this project's executable.
add_executable (project-audio "src/main.cpp" "include/main.h" "src/miniaudio.c" "include/miniaudio.h" "include/inputHandling.h" "src/inputHandling.cpp" "include/dataCallback.h" "src/dataCallback.cpp"   "include/volumeControl.h" "src/volumeControl.cpp" "include/playbackControl.h"  "src/playbackControl.cpp" "include/time.h"  "src/time.cpp" "include/seeking.h" "src/seeking.cpp" "src/helpCommand.cpp" "include/helpCommand.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET project-audio PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
