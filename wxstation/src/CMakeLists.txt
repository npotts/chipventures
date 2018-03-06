file( GLOB wxstation_SRC "*.hpp" "*.cpp" )

add_library( wxstation ${wxstation_SRC} )

target_include_directories( wxstation
                            INTERFACE
                            ${CMAKE_CURRENT_SOURCE_DIR}
                          )
