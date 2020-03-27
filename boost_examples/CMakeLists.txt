function(add_example name)
    add_executable(run_${name} ${name}_examples.cpp)
    target_include_directories(run_${name} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(run_${name} PRIVATE ${Boost_LIBRARIES})
    target_compile_options(run_${name} PRIVATE -Wall -Wextra)
endfunction()

add_subdirectory(ch03)
add_subdirectory(ch04)