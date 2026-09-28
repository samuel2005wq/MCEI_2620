% =========================================================================
% EJERCICIO 6: DIFERENCIACIÓN NUMÉRICA EN GNU OCTAVE
% =========================================================================
clc; clear; close all;

% 1. Cargar los datos experimentales desde el CSV
datos = csvread('/home/sag/MCEI_2620/taller_integracion/datos_sensor.csv', 1, 0);
x = datos(:, 1);
y = datos(:, 2);

N = length(x);
h = x(2) - x(1); % Paso uniforme

% -------------------------------------------------------------------------
% A. DIFERENCIA HACIA ADELANTE (FORWARD DIFFERENCE - O(h))
% -------------------------------------------------------------------------
tic;
df_fwd = zeros(N, 1);
df_fwd(1:N-1) = (y(2:N) - y(1:N-1)) / h;
df_fwd(N) = (y(N) - y(N-1)) / h; % Borde final (Backward)
t_fwd = toc * 1000; % Tiempo en ms

% -------------------------------------------------------------------------
% B. DIFERENCIA CENTRADA (CENTRAL DIFFERENCE - O(h^2))
% -------------------------------------------------------------------------
tic;
df_cent = zeros(N, 1);
df_cent(2:N-1) = (y(3:N) - y(1:N-2)) / (2 * h);
df_cent(1) = (y(2) - y(1)) / h;   % Borde inicial (Forward)
df_cent(N) = (y(N) - y(N-1)) / h; % Borde final (Backward)
t_cent = toc * 1000; % Tiempo en ms

% -------------------------------------------------------------------------
% C. FUNCIÓN NATIVA DE OCTAVE (gradient)
% -------------------------------------------------------------------------
tic;
df_octave = gradient(y, h);
t_native = toc * 1000; % Tiempo en ms

% -------------------------------------------------------------------------
% D. ERRORES
% -------------------------------------------------------------------------

error_abs = abs(df_cent - df_fwd);

mae = mean(error_abs);
rmse = sqrt(mean((df_cent - df_fwd).^2));

% -------------------------------------------------------------------------
% E. PRESENTACIÓN DE RESULTADOS
% -------------------------------------------------------------------------
printf('=== RENDIMIENTO Y TIEMPOS DE EJECUCIÓN (OCTAVE) ===\n');
printf('Diferencia Hacia Adelante O(h):   %.4f ms\n', t_fwd);
printf('Diferencia Centrada O(h^2):       %.4f ms\n', t_cent);
printf('Nativa gradient():                %.4f ms\n\n', t_native);

printf('Diferencia entre cada muestra (h): %.6f\n', h);
printf('Muestra de valores en el punto interior x(10):\n');
printf('  - Forward:  %.6f\n', df_fwd(10));
printf('  - Centrada: %.6f\n', df_cent(10));
printf('  - Gradient: %.6f\n', df_octave(10));

printf('MAE (Error Absoluto Medio): %.6f\n', mae);
printf('RMSE:                       %.6f\n', rmse);

% -------------------------------------------------------------------------
% F. GRÁFICAS
% -------------------------------------------------------------------------
figure('Name', 'Diferenciación Numérica en Octave');

subplot(2, 1, 1);
plot(x, y, 'b-', 'LineWidth', 1.5);
grid on;
title('Datos Experimentales y(x)');
xlabel('x'); ylabel('y(x)');

subplot(2, 1, 2);
plot(x, df_fwd, 'r--', 'LineWidth', 1.2, 'DisplayName', 'Diferencia Adelante O(h)');
hold on;
plot(x, df_cent, 'b-', 'LineWidth', 1.5, 'DisplayName', 'Diferencia Centrada O(h^2)');
plot(x, df_octave, 'k:', 'LineWidth', 1.2, 'DisplayName', 'Nativa gradient()');
grid on;
title('Aproximaciones de la Derivada dy/dx');
xlabel('x'); ylabel("f'(x)");
legend('Location', 'northeast');
