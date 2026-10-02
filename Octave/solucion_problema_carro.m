%Hecho por: Samuel Acuña y Carolina García

% Parámetros del problema
m = 20.0;
k = 20.0;
c_valores = [5, 40, 200];
h = 0.2; % Tamaño de paso temporal en segundos
t = 0:h:15;
n_pasos = length(t);

figure; hold on;
colores = {'b', 'r', 'g'};
etiquetas = {'Subamortiguado (c=5)', 'Crítico (c=40)', 'Sobreamortiguado (c=200)'};

for j = 1:3
    c = c_valores(j);

    % Matriz para guardar los resultados de estado: Y = [posición; velocidad]
    Y = zeros(2, n_pasos);
    Y(:,1) = [1.0; 0.0]; % Condiciones iniciales: x=1 m, v=0 m/s

    % Definir la función vectorial del sistema de EDOs
    f = @(t, Y) [Y(2); -(c/m)*Y(2) - (k/m)*Y(1)];

    % Implementación manual del método RK de quinto orden de Butcher
    for i = 1:(n_pasos-1)
        t_i = t(i);
        y_i = Y(:,i);

        % Cálculo de las 6 pendientes de exploración
        k1 = f(t_i, y_i);
        k2 = f(t_i + h/4, y_i + h*(k1/4));
        k3 = f(t_i + h/4, y_i + h*(k1/8 + k2/8));
        k4 = f(t_i + h/2, y_i + h*(-k2/2 + k3));
        k5 = f(t_i + 3*h/4, y_i + h*(3*k1/16 + 9*k4/16));
        k6 = f(t_i + h, y_i + h*(-3*k1/7 + 2*k2/7 + 12*k3/7 - 12*k4/7 + 8*k5/7));

        % Predicción del siguiente valor de solución
        Y(:,i+1) = y_i + (h/90) * (7*k1 + 32*k3 + 12*k4 + 32*k5 + 7*k6);
    end

    % Graficar únicamente el desplazamiento (primera fila de Y)
    plot(t, Y(1,:), 'Color', colores{j}, 'LineWidth', 2, 'DisplayName', etiquetas{j});
end

title('Método de Butcher: Desplazamiento vs. Tiempo');
xlabel('Tiempo t (s)');
ylabel('Desplazamiento x (m)');
grid on;
legend('show');
hold off;
