cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

project(chess_engine)
add_compile_options(-Ofast -Wall -Wpedantic -Wextra -Wunused-value -Wno-sign-compare)

find_package(Boost REQUIRED)

if (BOOST_FOUND)
    set(Boost_USE_STATIC_LIBS ON)
    include_directories(${Boost_INCLUDE_DIRS})
    file(GLOB SRC "*.cpp")
    add_executable(chess_engine ${SRC})
    target_link_libraries(chess_engine ${Boost_LIBRARIES})
endif()
