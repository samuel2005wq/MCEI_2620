% ejercicio1_octave_tabla.m
% ------------------------------------------------------------
% Ejercicio 1 - Parte B (GNU Octave)
% f(x) = e^{-0.4x}(1 + 0.5 sin(3x)),  0 <= x <= 8
% Salida: tablas con columnas  n | Integral aproximada | Error | Tiempo
% ------------------------------------------------------------
clear; clc; close all;

%% Funcion, antiderivada analitica y referencia
f = @(x) exp(-0.4*x) .* (1 + 0.5*sin(3*x));
F = @(x) -2.5*exp(-0.4*x) + (0.5/9.16)*exp(-0.4*x).*(-0.4*sin(3*x) - 3*cos(3*x));

a = 0;  b = 8;
I_ref = F(b) - F(a);

n_vals = [10 20 50 100 500 1000];
R = 2000;            % repeticiones para promediar el tiempo (tic/toc es muy ruidoso)

%% Funciones auxiliares (definidas antes de usarse)
function I = trapecio_compuesto(f, a, b, n)
  x = linspace(a, b, n+1);
  y = f(x);
  h = (b - a)/n;
  I = h * (sum(y) - 0.5*(y(1) + y(end)));   % h*[f0/2 + f1 + ... + f_{n-1} + fn/2]
end

function I = trapecio_trapz(f, a, b, n)
  x = linspace(a, b, n+1);
  I = trapz(x, f(x));                       % funcion nativa de Octave
end

function t = medir(fun, R)
  tic;
  for r = 1:R
    fun();
  end
  t = toc / R;                              % tiempo promedio por llamada (s)
end

%% Calculo
nN = numel(n_vals);
I1 = zeros(nN,1); e1 = zeros(nN,1); t1 = zeros(nN,1);
I2 = zeros(nN,1); e2 = zeros(nN,1); t2 = zeros(nN,1);

for k = 1:nN
  n = n_vals(k);

  I1(k) = trapecio_compuesto(f, a, b, n);
  e1(k) = abs(I1(k) - I_ref);
  t1(k) = medir(@() trapecio_compuesto(f, a, b, n), R);

  I2(k) = trapecio_trapz(f, a, b, n);
  e2(k) = abs(I2(k) - I_ref);
  t2(k) = medir(@() trapecio_trapz(f, a, b, n), R);
end

%% Referencia adaptativa de Octave (independiente de n)
tol_abs = 1e-10;  tol_rel = 1e-10;
[I_q, err_q] = quadgk(f, a, b, 'AbsTol', tol_abs, 'RelTol', tol_rel);
t_q = medir(@() quadgk(f, a, b, 'AbsTol', tol_abs, 'RelTol', tol_rel), 200);

%% Impresion de tablas
function imprimir_tabla(titulo, n_vals, I, e, t)
  fprintf('\n%s\n', titulo);
  fprintf('%s\n', repmat('-', 1, 62));
  fprintf('%6s %22s %14s %14s\n', 'n', 'Integral aproximada', 'Error', 'Tiempo (s)');
  fprintf('%s\n', repmat('-', 1, 62));
  for k = 1:numel(n_vals)
    fprintf('%6d %22.12f %14.4e %14.4e\n', n_vals(k), I(k), e(k), t(k));
  end
  fprintf('%s\n', repmat('-', 1, 62));
end

fprintf('Referencia analitica: I_ref = %.14f\n', I_ref);
imprimir_tabla('Tabla 1: Regla del trapecio compuesta (implementacion propia)', n_vals, I1, e1, t1);
imprimir_tabla('Tabla 2: Funcion trapz de Octave', n_vals, I2, e2, t2);

fprintf('\nTabla 3: Cuadratura adaptativa quadgk (no depende de n)\n');
fprintf('  Integral     = %.14f\n', I_q);
fprintf('  Error real   = %.4e\n', abs(I_q - I_ref));
fprintf('  Error estim. = %.4e\n', err_q);
fprintf('  AbsTol = %.1e, RelTol = %.1e\n', tol_abs, tol_rel);
fprintf('  Tiempo       = %.4e s\n', t_q);


%% Grafica de convergencia
figure;
loglog(n_vals, e1, '-o', 'LineWidth', 1.5); hold on;
loglog(n_vals, e2, '-s', 'LineWidth', 1.5);
loglog(n_vals, e1(1)*(n_vals(1)./n_vals).^2, 'k--');   % pendiente teorica O(1/n^2)
xlabel('n'); ylabel('Error absoluto');
title('Convergencia del trapecio compuesto');
legend('Trapecio propio', 'trapz', 'O(1/n^2)', 'Location', 'southwest');
grid on;
