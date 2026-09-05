#include <iostream>
#include <iomanip>
#include <cmath>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>

// Función objetivo: f(x) = ...
// Es donde se implementa la función que le queremos encontrar la raíz.
// Firma requerida por GSL: double func(double x, void *params)
double f(double x, void *params)
{
  // Se implementa aquí la función.
  return exp(-x) - x; // Segunda función de trabajo: f(x) = exp(-x) - x
}

int main()
{
  // Tipos y objetos del solver de GSL
  const gsl_root_fsolver_type *T; // se  indica el tipo de método del gsl
  gsl_root_fsolver *s;            // es un puntero al solver donde se tiene la información del método y el estado de la iteración
  gsl_function F;                 // la estructura que contiene la función y parámetros

  // Aqui se asocia la función y parámetros a la estructura del gsl_function
  F.function = &f;
  F.params = nullptr; // no se usan parámetros en este ejemplo

  // Intervalo inicial que debe tener la raíz
  double x_lo = 0.0;
  double x_hi = 1.0;

  // Se selecciona el método: Bisección
  T = gsl_root_fsolver_bisection;
  // Reservas una parte de la memoria para el solver
  s = gsl_root_fsolver_alloc(T);
  // Inicializas el solver con la función y el intervalo
  gsl_root_fsolver_set(s, &F, x_lo, x_hi);

  // Cabecera de la salida que mostrará la iteración y el intervalo
  std::cout << "iter\t" << "inf\t" << "sup\t" << "raíz\n";

  int status;
  int iter = 0;
  int max_iter = 100; // límite de iteraciones por seguridad
  double r;           // aproximación actual de la raíz

  // Bucle de iteración: se itera hasta convergencia o hasta max_iter
  do
  {
    iter++;
    // Ejecuta una iteración del solver
    status = gsl_root_fsolver_iterate(s);
    // Recupera la aproximación actual de la raíz
    r = gsl_root_fsolver_root(s);
    // Recupera los extremos del intervalo actual
    x_lo = gsl_root_fsolver_x_lower(s);
    x_hi = gsl_root_fsolver_x_upper(s);
    // Imprime el progreso de la iteración
    std::cout << iter << "\t" << x_lo << "\t" << x_hi << "\t" << r << "\n";
    // Comprueba si el intervalo es suficientemente pequeño (tolerancia absoluta)
    status = gsl_root_test_interval(x_lo, x_hi, 0.0, 1e-8);
  } while (status == GSL_CONTINUE && iter < max_iter);

  // Imprime la raíz encontrada y libera los recursos
  std::cout << "\nRaiz encontrada = " << r << std::endl;
  gsl_root_fsolver_free(s);
  return 0;
}