cmake_minimum_required( VERSION 3.20 )
project(taller_diferenciacion)

find_package( Eigen3 )

add_executable( solucion solucion_problema_robot.cpp )