add_executable(${PROJECT_NAME} main.cpp errors.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_compiler_warnings(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::program_options)
