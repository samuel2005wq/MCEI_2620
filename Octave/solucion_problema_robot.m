%Hecho por: Samuel Acuña y Carolina García


% Parte 1 - GNU Octave: diferencias centrales explícitas
clear; clc; close all;
set(0, "defaultfigurevisible", "off");  % sin ventana: WSL sin X11

% Cargar datos (saltar 1 fila de encabezado, columna 0)
archivo = "/home/sag/MCEI_2620/trayectoria_robot.csv";
txt = fileread(archivo);
txt = strrep(txt, ",", ".");            % coma decimal -> punto
txt = strrep(txt, ";", " ");            % separador ; -> espacio
txt = txt(find(txt == "\n", 1) + 1:end);  % quitar la fila de encabezado
data = sscanf(txt, "%f", [3 Inf])';     % matriz N x 3
t = data(:,1);
x = data(:,2);
y = data(:,3);

N = length(t);
h = t(2) - t(1);          % paso uniforme

% --- Derivadas por diferencias centrales ---
xd = zeros(N,1);
yd = zeros(N,1);
for i = 2:N-1
  xd(i) = (x(i+1) - x(i-1)) / (2*h);
  yd(i) = (y(i+1) - y(i-1)) / (2*h);
end
% Extremos: diferencias hacia adelante / atrás (no hay vecino a ambos lados)
xd(1) = (x(2) - x(1)) / h;      xd(N) = (x(N) - x(N-1)) / h;
yd(1) = (y(2) - y(1)) / h;      yd(N) = (y(N) - y(N-1)) / h;

% --- Velocidad lineal y orientación ---
v = sqrt(xd.^2 + yd.^2);
theta = atan2(yd, xd);
theta = unwrap(theta);          % desenvolvimiento angular ANTES de derivar

% --- Velocidad angular por diferencias centrales ---
omega = zeros(N,1);
for i = 2:N-1
  omega(i) = (theta(i+1) - theta(i-1)) / (2*h);
end
omega(1) = (theta(2) - theta(1)) / h;
omega(N) = (theta(N) - theta(N-1)) / h;

% --- Gráficas ---
figure(1);
plot(x, y, "b-"); axis equal; grid on;
xlabel("x"); ylabel("y"); title("Trayectoria y vs. x");

figure(2);
plot(t, v, "r-"); grid on;
xlabel("t"); ylabel("v"); title("Velocidad lineal v vs. t");

figure(3);
plot(t, theta, "g-"); grid on;
xlabel("t"); ylabel("\\theta (rad)"); title("Orientación \\theta vs. t");

figure(4);
plot(t, omega, "m-"); grid on;
xlabel("t"); ylabel("\\omega (rad/s)"); title("Velocidad angular \\omega vs. t");

% --- Guardar las figuras como PNG (en la carpeta actual) ---
nombres = {"fig1_trayectoria", "fig2_v", "fig3_theta", "fig4_omega"};
for k = 1:4
  print(figure(k), [nombres{k} ".png"], "-dpng", "-r120");
end
disp("Listo: se guardaron fig1..fig4 como PNG.");
