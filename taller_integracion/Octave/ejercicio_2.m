% =========================================================
% TALLER: EJERCICIO 2 DE INTEGRACIÓN NUMÉRICA EN GNU OCTAVE
% =========================================================

clear; clc; close all;

% 1. Cargar el archivo CSV (saltando la primera fila de encabezado)
datos = csvread('/home/sag/MCEI_2620/taller_integracion/datos_sensor.csv', 1, 0);
x = datos(:, 1);
y = datos(:, 2);

N = length(x);            % N = 50 mediciones
n = N - 1;                % n = 49 subintervalos
h = x(2) - x(1);          % h = 0.2 paso de integración

fprintf('--- INFORMACIÓN DEL CONJUNTO DE DATOS ---\n');
fprintf('Número de puntos (N): %d\n', N);
fprintf('Número de subintervalos (n): %d\n', n);
fprintf('Ancho del intervalo (h): %.2f\n\n', h);

% ---------------------------------------------------------
% A. REGLA DEL TRAPECIO COMPUESTA
% ---------------------------------------------------------
tic; % Iniciar cronómetro para Trapecio
I_trapecio = trapz(x, y);
t_trap = toc * 1000; % Tiempo en milisegundos

fprintf('--- RESULTADOS DE INTEGRACIÓN ---\n');
fprintf('1. Integral por Trapecio Compuesto: %.6f | Tiempo: %.4f ms\n', I_trapecio, t_trap);

% ---------------------------------------------------------
% B. REGLA DE SIMPSON COMBINADA (1/3 + Trapecio)
% ---------------------------------------------------------
tic; % Iniciar cronómetro para Simpson

% Aplicar Simpson 1/3 a los primeros 49 puntos (48 subintervalos)
y_simp = y(1:49);
I_s13 = (h / 3) * (y_simp(1) + y_simp(end) + 4 * sum(y_simp(2:2:end-1)) + 2 * sum(y_simp(3:2:end-2)) );

% Trapecio en el último subintervalo
I_trap_ultimo = (h / 2) * (y(49) + y(50));

% Suma total de áreas
I_simpson = I_s13 + I_trap_ultimo;

t_simp = toc * 1000; % Tiempo en milisegundos

fprintf('2. Integral por Simpson (1/3 + Trapecio final): %.6f | Tiempo: %.4f ms\n\n', I_simpson, t_simp);

% ---------------------------------------------------------
% C. ERRORES DE CADA FORMA
% ---------------------------------------------------------

I_ref_teorica = 21.19201822; % Se toma el valor "real" calculado de forma analítica

% Errores de Trapecio
err_abs_trap = abs(I_trapecio - I_ref_teorica);
err_rel_trap = (err_abs_trap / I_ref_teorica) * 100;

% Errores de Simpson
err_abs_simp = abs(I_simpson - I_ref_teorica);
err_rel_simp = (err_abs_simp / I_ref_teorica) * 100;

fprintf('Valor de referencia para los errores: %.8f\n', I_ref_teorica);

fprintf('--- ERROR DE TRAPECIO ---\n');
fprintf('Error Absoluto: %.8f | Error Relativo: %.6f%%\n', err_abs_trap, err_rel_trap);

fprintf('--- ERROR DE SIMPSON ---\n');
fprintf('Error Absoluto: %.8f | Error Relativo: %.6f%%\n\n', err_abs_simp, err_rel_simp);

% ---------------------------------------------------------
% D. GRAFICAR LOS DATOS Y EL ÁREA APROXIMADA
% ---------------------------------------------------------
figure('Name', 'Integración Numérica - GNU Octave');

% Graficar el área bajo la curva aproximada
area(x, y, 'FaceColor', [0.75, 0.85, 0.95], 'EdgeColor', [0, 0.45, 0.74], 'LineWidth', 1.5);
hold on;

% Graficar las mediciones como puntos
plot(x, y, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');

% Configuración estética del gráfico
grid on;
title('Aproximación del área I = \int_{0}^{9.8} y(x) dx');
xlabel('x (posición)');
ylabel('y (medición)');
legend('Área aproximada (Trapecios/Parábolas)', 'Puntos medidos (50 datos)', 'Location', 'northwest');

hold off;
