cmake_minimum_required( VERSION 3.20 )
project(matrices)

find_package( Eigen3 )

add_executable( punto_1.1 main.cpp )
add_executable( punto_1.2 main12.cpp )
add_executable( punto_1.3 main13.cpp )
add_executable( punto_2.0 main20.cpp )