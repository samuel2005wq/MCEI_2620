#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <gsl/gsl_spline.h>

#define MAX_PUNTOS 100

// Función auxiliar para obtener el tiempo actual en milisegundos
double get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

int main(void)
{
    // 1. Cargar el archivo CSV
    FILE *fp = fopen("/home/sag/MCEI_2620/taller_integracion/datos_sensor.csv", "r");
    if (fp == NULL)
    {
        perror("Error al abrir datos_sensor.csv");
        return EXIT_FAILURE;
    }

    char buffer[256];
    // Omitir la primera línea (encabezado "x,y")
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        fclose(fp);
        return EXIT_FAILURE;
    }

    double x[MAX_PUNTOS];
    double y[MAX_PUNTOS];
    int n = 0;

    while (fscanf(fp, "%lf,%lf", &x[n], &y[n]) == 2)
    {
        n++;
        if (n >= MAX_PUNTOS)
            break;
    }
    fclose(fp);

    printf("--- INFORMACION DE DATOS LEIDOS ---\n");
    printf("Total de puntos (N): %d\n", n);
    printf("Rango de x: [%.2f, %.2f]\n", x[0], x[n - 1]);
    printf("Paso (h): %.2f\n\n", x[1] - x[0]);

    // ---------------------------------------------------------
    // ESTRATEGIA 1: Trapecio Explícito (Medición de tiempo)
    // ---------------------------------------------------------
    double t0 = get_time_ms();

    double h = x[1] - x[0];
    double suma_trapecio = y[0] + y[n - 1];

    for (int i = 1; i < n - 1; i++)
    {
        suma_trapecio += 2.0 * y[i];
    }
    double I_trap = (h / 2.0) * suma_trapecio;

    double t_trap = get_time_ms() - t0;

    // ---------------------------------------------------------
    // ESTRATEGIA 2: Spline GSL (Medición de tiempo)
    // ---------------------------------------------------------
    t0 = get_time_ms();

    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, n);

    // Inicializar el spline e integrar sobre [x_0, x_n-1]
    gsl_spline_init(spline, x, y, n);
    double I_spline = gsl_spline_eval_integ(spline, x[0], x[n - 1], acc);

    double t_spline = get_time_ms() - t0;

    // ---------------------------------------------------------
    // CÁLCULO DE ERRORES Y REPORTE
    // ---------------------------------------------------------
    double I_ref = 21.19201822;

    double err_trap = fabs(I_trap - I_ref) / I_ref * 100.0;
    double err_spline = fabs(I_spline - I_ref) / I_ref * 100.0;

    printf("=========================================================================\n");
    printf("RESULTADOS DE INTEGRACION CON DATOS DISCRETOS EQUIESPACIADOS EN C / C++ CON GSL\n");
    printf("=========================================================================\n");
    printf("1. Trapecio explicito:    I = %.8f | Err Rel: %.6f%% | Tiempo: %.4f ms\n", I_trap, err_trap, t_trap);
    printf("2. Spline GSL (Cubic):    I = %.8f | Err Rel: %.6f%% | Tiempo: %.4f ms\n", I_spline, err_spline, t_spline);
    printf("=========================================================================\n");

    // Liberar memoria asignada por GSL
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);

    return EXIT_SUCCESS;
}