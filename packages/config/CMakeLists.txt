file(GLOB Header "include/*.h" "include/*.hpp")
add_library(Config ${Header})

set_target_properties(Config PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries( Config INTERFACE  )

target_include_directories( 
    Config PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)