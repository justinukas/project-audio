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
add_executable (ooptest 
  libraries/miniaudio.h
  
  include/audioMaster.hpp
  include/audioSetup.hpp
  include/commandExecutor.hpp
  include/commandParsing.hpp
  include/dataCallback.hpp
  include/outputProcessor.hpp
  include/helpers/audioPlayer.hpp
  include/helpers/seeker.hpp
  include/helpers/timeChecker.hpp
  include/helpers/volumeController.hpp
  include/helpers/queueMaster.hpp
  src/dataCallback.cpp
  src/main.cpp
  src/outputProcessor.cpp  
  src/helpers/audioPlayer.cpp
  src/helpers/seeker.cpp
  src/helpers/timeChecker.cpp
  src/helpers/volumeController.cpp
  src/helpers/queueMaster.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ooptest PROPERTY CXX_STANDARD 20)
endif()

find_package(ftxui REQUIRED)

target_link_libraries(ooptest
  # FTXUI library links
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component

  # Linux libraries (Probably remove them if building on Windows)
  dl
  pthread
  m
)

# TODO: Add tests and install targets if needed. (?)
