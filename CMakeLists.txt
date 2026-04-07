# CMakeList.txt : CMake project for project-audio, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ooptest")

# Add source to this project's executable.
add_executable (ooptest "include/miniaudio.h" "src/main.cpp" "include/audioPlayer.hpp" "src/playbackImplementation.cpp" "src/playlistImplementation.cpp" "include/audioSetup.hpp" "include/commandParsing.hpp" "src/commandParsing.cpp" "include/dataCallback.hpp" "src/dataCallback.cpp" "include/seeker.hpp" "include/timeChecker.hpp" "include/volumeControl.hpp" "include/globalVars.hpp" "src/globalVarsDefaults.cpp" "include/playlistManager.hpp" "include/outputProcessor.hpp" "src/outputProcessor.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ooptest PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

# Linux libraries (Probably remove them if building on Windows)
target_link_libraries(ooptest
    dl
    pthread
    m
)
