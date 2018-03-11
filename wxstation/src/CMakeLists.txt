set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
find_package(Boost 1.45.0 REQUIRED COMPONENTS date_time filesystem) 




file( GLOB wxstation_SRC "*.hpp" "*.cpp" )

add_library( wxstation ${wxstation_SRC} )
target_link_libraries(wxstation ${Boost_LIBRARIES})
target_include_directories( wxstation
                            INTERFACE
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${Boost_INCLUDE_DIRS}  
                          )

