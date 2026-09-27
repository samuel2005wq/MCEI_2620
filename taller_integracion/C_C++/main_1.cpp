#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_spline.h>

#define MAX_PUNTOS 100

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
    // Omitir la primera línea correspondiente a los encabezados "x,y"
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        fprintf(stderr, "Error al leer el encabezado del archivo.\n");
        fclose(fp);
        return EXIT_FAILURE;
    }

    double x[MAX_PUNTOS];
    double y[MAX_PUNTOS];
    int n = 0;

    // Lectura formateada de cada fila del CSV
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
    // ESTRATEGIA 1: Trapecio Explícito (Manual)
    // ---------------------------------------------------------
    double h = x[1] - x[0];
    double suma_trapecio = y[0] + y[n - 1];

    for (int i = 1; i < n - 1; i++)
    {
        suma_trapecio += 2.0 * y[i];
    }
    double I_trap = (h / 2.0) * suma_trapecio;

    // ---------------------------------------------------------
    // ESTRATEGIA 2: Interpolación por Spline Cúbico e Integración con GSL
    // ---------------------------------------------------------
    // Reserva de memoria para acelerador de búsqueda e interpolador spline
    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, n);

    // Inicializar el spline con las 50 muestras de datos
    gsl_spline_init(spline, x, y, n);

    // Integrar la función spline sobre todo el dominio [x_0, x_n-1]
    double I_spline = gsl_spline_eval_integ(spline, x[0], x[n - 1], acc);

    // Referencia de la función analítica generadora
    double I_ref = 21.19201822;

    double err_trap = fabs(I_trap - I_ref) / I_ref * 100.0;
    double err_spline = fabs(I_spline - I_ref) / I_ref * 100.0;

    // Imprimir reporte de resultados
    printf("=======================================================\n");
    printf("RESULTADOS DE INTEGRACION CON DATOS DISCRETOS EQUIESPACIADOS EN C / C++ CON GSL\n");
    printf("=======================================================\n");
    printf("1. Trapecio explicito:    I = %.8f | Error Rel: %.6f%%\n", I_trap, err_trap);
    printf("2. Spline GSL (Cubic):    I = %.8f | Error Rel: %.6f%%\n", I_spline, err_spline);
    printf("=======================================================\n");

    // Liberar memoria asignada por GSL
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);

    return EXIT_SUCCESS;
}