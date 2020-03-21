find_package( Boost 1.65.1 REQUIRED COMPONENTS program_options system filesystem )
find_package( Threads )

file(GLOB Header "include/*.h")
file(GLOB Source "src/*.h" "src/*.cpp")
add_library(
    Pipeline
    ${Header}
    ${Source}
)

target_link_libraries( 
    Pipeline PRIVATE 
    Config 
    Boost::program_options rt  
    ${CMAKE_THREAD_LIBS_INIT} 
    ${Boost_LIBRARIES} 
)

target_include_directories( 
    Pipeline PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ 
)
