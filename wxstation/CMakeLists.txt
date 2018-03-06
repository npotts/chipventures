cmake_minimum_required(VERSION 3.0)
project(cpp-project-template)



macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

use_cxx11()

if (MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()


#add_subdirectory(doc)
add_subdirectory(src)
#add_subdirectory(test)
add_subdirectory(tools)

#add_subdirectory(third-party/googletest EXCLUDE_FROM_ALL)
include_directories(AFTER "src")

#
# List additional files in QtCreator project tree.
#
add_custom_target(additional_project_files
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format
        ${CMAKE_CURRENT_SOURCE_DIR}/.travis.yml
        ${CMAKE_CURRENT_SOURCE_DIR}/appveyor.yml
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    )


#
# Turn off warning about unused QT_QMAKE_EXECUTABLE added by QtCreator
#
unset(${QT_QMAKE_EXECUTABLE} CACHE)
