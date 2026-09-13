% ============================================================
%  Hecho por Carolina Garcìa y Samuel Acuña
%  Solución de A*x = b (10x10) por tres métodos:
%     1) Eliminación de Gauss con pivoteo parcial
%     2) Factorización LU (PA = LU) con pivoteo parcial
%     3) Factorización QR (Householder)
%  y comparación de estabilidad (residuo/error) y eficiencia (tiempo)
% ============================================================
clear; clc; format short;

% ============================================================
% FUNCIONES AUXILIARES
% ============================================================
function [L, U, P] = lu_manual(A)
    % Factorización LU con pivoteo parcial: P*A = L*U
    n = size(A,1);
    L = eye(n);
    U = A;
    P = eye(n);
    for k = 1:n-1
        [~, idx] = max(abs(U(k:n,k)));
        p = idx + k - 1;
        if p ~= k
            U([k p],:) = U([p k],:);
            P([k p],:) = P([p k],:);
            if k > 1
                L([k p],1:k-1) = L([p k],1:k-1);
            end
        end
        for i = k+1:n
            L(i,k) = U(i,k)/U(k,k);
            U(i,k:n) = U(i,k:n) - L(i,k)*U(k,k:n);
        end
    end
end

function [Q, R] = qr_householder(A)
    % Factorización QR mediante reflexiones de Householder
    [m, n] = size(A);
    R = A;
    Q = eye(m);
    for k = 1:n
        x = R(k:m, k);
        s = 1;
        if x(1) < 0
            s = -1;
        end
        alpha = -s * norm(x);
        e1 = zeros(length(x),1); e1(1) = 1;
        v = x - alpha*e1;
        nv = norm(v);
        if nv < eps
            continue;
        end
        v = v / nv;
        R(k:m, :) = R(k:m,:) - 2*v*(v'*R(k:m,:));
        Q(:, k:m) = Q(:,k:m) - 2*(Q(:,k:m)*v)*v';
    end
end

function y = forward_sub(L, b)
    n = length(b);
    y = zeros(n,1);
    y(1) = b(1)/L(1,1);
    for i = 2:n
        y(i) = (b(i) - L(i,1:i-1)*y(1:i-1)) / L(i,i);
    end
end

function x = back_sub(U, y)
    n = length(y);
    x = zeros(n,1);
    x(n) = y(n)/U(n,n);
    for i = n-1:-1:1
        x(i) = (y(i) - U(i,i+1:n)*x(i+1:n)) / U(i,i);
    end
end

A = [ 2 1 0 3 2 1 0 2 1 4;
      1 3 2 0 1 4 2 1 0 2;
      0 2 4 1 3 0 1 2 4 1;
      3 0 1 5 2 1 3 0 2 1;
      2 1 3 2 6 2 1 4 0 3;
      1 4 0 1 2 5 2 1 3 0;
      0 2 1 3 1 2 4 0 2 1;
      2 1 2 0 4 1 0 5 3 2;
      1 0 4 2 0 3 2 3 6 1;
      4 2 1 1 3 0 1 2 1 5 ];

% <<<<<< Vector b de ejemplo (cámbialo si tu ejercicio trae uno) >>>>>>
b = [12; 18; 15; 20; 25; 14; 16; 22; 19; 17];

n = length(b);

printf('=====================================================\n');
printf(' Sistema 10x10 -- Comparación Gauss / LU / QR\n');
printf('=====================================================\n\n');

% ---------------------------------------------------------------
% 1) ELIMINACIÓN DE GAUSS CON PIVOTEO PARCIAL
% ---------------------------------------------------------------
tic;
Ab = [A b];
for k = 1:n-1
    [~, idx] = max(abs(Ab(k:n, k)));
    p = idx + k - 1;
    if abs(Ab(p,k)) < eps
        error('Matriz singular: no hay pivote en la columna %d', k);
    end
    if p ~= k
        Ab([k p], :) = Ab([p k], :);
        printf('Gauss: Intercambio F%d <-> F%d\n', k, p);
    end
    for i = k+1:n
        m = Ab(i,k) / Ab(k,k);
        Ab(i, k:end) = Ab(i, k:end) - m * Ab(k, k:end);
    end
end
x_gauss = zeros(n,1);
x_gauss(n) = Ab(n, end) / Ab(n, n);
for i = n-1:-1:1
    x_gauss(i) = ( Ab(i,end) - Ab(i, i+1:n) * x_gauss(i+1:n) ) / Ab(i,i);
end
t_gauss = toc;

disp(' ');
disp('--- Solución por Eliminación de Gauss ---');
disp(x_gauss');

% ---------------------------------------------------------------
% 2) FACTORIZACIÓN LU  (PA = LU) CON PIVOTEO PARCIAL
% ---------------------------------------------------------------
tic;
[L, U, P] = lu_manual(A);
y = forward_sub(L, P*b);
x_lu = back_sub(U, y);
t_lu = toc;

disp(' ');
disp('--- Factorización LU ---');
disp('L ='); disp(L);
disp('U ='); disp(U);
disp('--- Solución por Factorización LU ---');
disp(x_lu');
printf('Verificación ||P*A - L*U|| = %e\n', norm(P*A - L*U));

% ---------------------------------------------------------------
% 3) FACTORIZACIÓN QR (Householder)
% ---------------------------------------------------------------
tic;
[Q, R] = qr_householder(A);
x_qr = back_sub(R, Q' * b);
t_qr = toc;

disp(' ');
disp('--- Factorización QR (Householder) ---');
disp('Q ='); disp(Q);
disp('R ='); disp(R);
disp('--- Solución por Factorización QR ---');
disp(x_qr');
printf('Verificación ||A - Q*R|| = %e\n', norm(A - Q*R));

% ---------------------------------------------------------------
% COMPARACIÓN DE ESTABILIDAD Y EFICIENCIA
% ---------------------------------------------------------------
x_exact = A\b;   % referencia de alta precisión (LAPACK)

r_gauss = norm(A*x_gauss - b);
r_lu    = norm(A*x_lu - b);
r_qr    = norm(A*x_qr - b);

e_gauss = norm(x_gauss - x_exact);
e_lu    = norm(x_lu - x_exact);
e_qr    = norm(x_qr - x_exact);

printf('\n=====================================================\n');
printf(' Método   | Tiempo (s)  | ||Ax-b||   | ||x - x_exacta||\n');
printf('-----------------------------------------------------\n');
printf(' Gauss    | %10.6f  | %9.3e | %9.3e\n', t_gauss, r_gauss, e_gauss);
printf(' LU       | %10.6f  | %9.3e | %9.3e\n', t_lu, r_lu, e_lu);
printf(' QR       | %10.6f  | %9.3e | %9.3e\n', t_qr, r_qr, e_qr);
printf('=====================================================\n\n');

printf('Número de condición de A: %e\n\n', cond(A));

printf([ ...
'Comentario:\n' ...
' - Gauss y LU realizan esencialmente la misma eliminación\n' ...
'   (costo ~2n^3/3 flops); LU es más conveniente cuando hay que\n' ...
'   resolver el sistema para varios vectores b distintos, porque\n' ...
'   la factorización se hace una sola vez y luego solo se hacen\n' ...
'   sustituciones hacia adelante/atrás (O(n^2) cada una).\n' ...
' - QR (Householder) cuesta cerca del doble (~4n^3/3 flops) pero\n' ...
'   es más estable: al usar reflexiones ortogonales no amplifica\n' ...
'   errores de redondeo, por lo que es preferible en sistemas mal\n' ...
'   condicionados o problemas de mínimos cuadrados.\n' ...
' - Con esta matriz A (razonablemente bien condicionada) los tres\n' ...
'   métodos producen prácticamente la misma solución; las\n' ...
'   diferencias de precisión se notan sobre todo cuando cond(A)\n' ...
'   es grande.\n']);


