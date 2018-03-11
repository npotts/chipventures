execute_process(
  COMMAND git describe --always
  WORKING_DIRECTORY "@CMAKE_SOURCE_DIR@/.."
  OUTPUT_VARIABLE GIT_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_TAG="${GIT_TAG}")

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
find_package(Boost 1.45.0 REQUIRED COMPONENTS system filesystem program_options) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(wx-station wxstation.cpp)
target_link_libraries(wx-station
  PUBLIC
    ${Boost_LIBRARIES} 
  PRIVATE
     wxstation 
)

include_directories(AFTER "../src") 

