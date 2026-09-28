#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_POINTS 10000

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
    char header_buffer[256];
    fgets(header_buffer, sizeof(header_buffer), fp);
    double x[MAX_POINTS];
    double y[MAX_POINTS];
    int N = 0;

    // Lectura del CSV fila por fila
    while (fscanf(fp, "%lf,%lf", &x[N], &y[N]) == 2)
    {
        N++;
        if (N >= MAX_POINTS)
            break;
    }
    fclose(fp);

    if (N < 2)
    {
        printf("Error: Se necesitan al menos 2 puntos para diferenciar.\n");
        return EXIT_FAILURE;
    }

    double h = x[1] - x[0]; // Paso de malla uniforme

    double df_fwd[MAX_POINTS];
    double df_cent[MAX_POINTS];

    // -------------------------------------------------------------------------
    // A. DIFERENCIA HACIA ADELANTE (FORWARD DIFFERENCE - O(h))
    // -------------------------------------------------------------------------
    double t0 = get_time_ms();

    for (int i = 0; i < N - 1; i++)
    {
        df_fwd[i] = (y[i + 1] - y[i]) / h;
    }
    df_fwd[N - 1] = (y[N - 1] - y[N - 2]) / h; // Borde final: Backward

    double t_fwd = get_time_ms() - t0;

    // -------------------------------------------------------------------------
    // B. DIFERENCIA CENTRADA (CENTRAL DIFFERENCE - O(h^2))
    // -------------------------------------------------------------------------
    t0 = get_time_ms();

    df_cent[0] = (y[1] - y[0]) / h; // Borde inicial: Forward
    for (int i = 1; i < N - 1; i++)
    {
        df_cent[i] = (y[i + 1] - y[i - 1]) / (2.0 * h);
    }
    df_cent[N - 1] = (y[N - 1] - y[N - 2]) / h; // Borde final: Backward

    double t_cent = get_time_ms() - t0;

    // -------------------------------------------------------------------------
    // C. ERRORES
    // -------------------------------------------------------------------------

    double suma_error_abs = 0.0;
    double suma_error_cuad = 0.0;

    for (int i = 0; i < N; i++)
    {
        double diff = df_cent[i] - df_fwd[i];
        suma_error_abs += fabs(diff);
        suma_error_cuad += diff * diff;
    }

    double mae = suma_error_abs / N;
    double rmse = sqrt(suma_error_cuad / N);

    // -------------------------------------------------------------------------
    // IMPRESIÓN DE RESULTADOS
    // -------------------------------------------------------------------------
    printf("=== RENDIMIENTO Y TIEMPOS DE EJECUCIÓN (C) ===\n");
    printf("Diferencia Hacia Adelante O(h):   %.4f ms\n", t_fwd);
    printf("Diferencia Centrada O(h^2):       %.4f ms\n\n", t_cent);

    printf("Paso de malla (h): %.6f\n", h);
    printf("Muestra de valores en el punto interior x[9] (elemento 10):\n");
    printf("  - Forward:  %.6f\n", df_fwd[9]);
    printf("  - Centrada: %.6f\n", df_cent[9]);

    printf("MAE (Error Absoluto Medio): %.6f\n", mae);
    printf("RMSE:                       %.6f\n", rmse);

    return EXIT_SUCCESS;
}