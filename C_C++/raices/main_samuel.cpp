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
  return x * x * x - 5 * x + 1; // Primera función de trabajo: f(x) = x^3 - 5x + 1
}

// Derivada de f
double df(double x, void *params)
{
  return 3.0 * x * x - 5.0;
}

// f y df juntos (rellena y y dy)
void fdf(double x, void *params, double *y, double *dy)
{
  *y = f(x, params);
  *dy = df(x, params);
}

int main()
{
  // Tipos y objetos del solver de GSL (usando Newton con derivada)
  const gsl_root_fdfsolver_type *T; // tipo de método con derivada (Newton)
  gsl_root_fdfsolver *s;            // solver para métodos con derivada
  gsl_function_fdf F;               // estructura que contiene f, df, fdf y params

  // Aqui se asocia la función y parámetros a la estructura del gsl_function_fdf
  F.f = &f;           // función objetivo
  F.df = &df;         // derivada de la función
  F.fdf = &fdf;       // función que calcula f y df juntos
  F.params = nullptr; // no se usan parámetros en este ejemplo

  // Punto inicial para Newton
  double x0 = 0; // aproximación inicial

  // Se selecciona el método: newton (con derivada)
  T = gsl_root_fdfsolver_newton; // Método de Newton-Raphson (requiere df)
  // Reserva memoria para el solver
  s = gsl_root_fdfsolver_alloc(T);
  // Inicializa el solver con la función y la estimación inicial
  gsl_root_fdfsolver_set(s, &F, x0);

  // Cabecera de la salida que mostrará la iteración y el intervalo
  std::cout << "iter\t" << "inf\t" << "sup\t" << "raíz\n";

  int status;
  int iter = 0;
  int max_iter = 100; // límite de iteraciones por seguridad
  double r;           // aproximación actual de la raíz
  double x_prev = x0; // valor previo para comprobar delta

  // Bucle de iteración: se itera hasta convergencia o hasta max_iter
  do
  {
    iter++;
    // Ejecuta una iteración del solver (método con derivada)
    status = gsl_root_fdfsolver_iterate(s);
    // Recupera la aproximación actual de la raíz
    r = gsl_root_fdfsolver_root(s);
    // Imprime el progreso de la iteración
    std::cout << iter << "\t" << x_prev << "\t" << r << "\t" << (r - x_prev) << "\n";
    // Comprueba si la diferencia entre iteraciones es suficientemente pequeña
    status = gsl_root_test_delta(r, x_prev, 0.0, 1e-8);
    x_prev = r;
  } while (status == GSL_CONTINUE && iter < max_iter);

  // Imprime la raíz encontrada y libera los recursos
  std::cout << "\nRaiz encontrada = " << r << std::endl;
  gsl_root_fdfsolver_free(s);
  return 0;
}
