cmake_minimum_required( VERSION 3.20 )
project(Taller_Integral)

find_package( GSL REQUIRED )

add_executable( ejercicio_1 main_1.cpp )
add_executable( ejercicio_2 main_2.cpp )
add_executable( ejercicio_3 main_3.cpp )

target_link_libraries( ejercicio_1 PRIVATE GSL::gsl GSL::gslcblas )
target_link_libraries( ejercicio_2 PRIVATE GSL::gsl GSL::gslcblas )
target_link_libraries( ejercicio_3 PRIVATE GSL::gsl GSL::gslcblas )