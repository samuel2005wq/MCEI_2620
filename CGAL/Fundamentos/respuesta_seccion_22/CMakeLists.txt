cmake_minimum_required(VERSION 3.16)
project(sensores_cgal LANGUAGES CXX)

find_package(CGAL REQUIRED)

add_executable(sensores_cgal ejer_contex.cpp)
target_link_libraries(sensores_cgal PRIVATE CGAL::CGAL)