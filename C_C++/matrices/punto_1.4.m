% ============================================================
%  punto4_inversa_qr_svd.m
%
%  PARTE 4: QR y SVD para inversa de matrices
%     - Inversa por método directo (inv)
%     - Inversa vía factorización QR
%     - Inversa vía SVD
%     - Comparación de resultados y estabilidad numérica
%
%  Usa la misma matriz A del ejercicio anterior (10x10).
%
%  Ejecutar en Octave, parado en esta carpeta:
%       punto4_inversa_qr_svd
% ============================================================
clc; format short;

% ============================================================
% FUNCIÓN AUXILIAR (subfunción de este mismo archivo)
% ============================================================
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

n = size(A,1);

printf('=====================================================\n');
printf(' PARTE 4: QR y SVD para inversa de matrices\n');
printf('=====================================================\n\n');

% ---- 1) Método directo (inversa estándar) ----
tic;
Ainv_direct = inv(A);
t_direct = toc;

% ---- 2) Inversa vía factorización QR:  A = Q*R  =>  A^-1 = R^-1 * Q' ----
tic;
[Q, R] = qr_householder(A);
Ainv_qr = R \ (Q');
t_qr = toc;

% ---- 3) Inversa vía SVD:  A = U*S*V'  =>  A^-1 = V*S^-1*U' ----
tic;
[U, S, V] = svd(A);
Ainv_svd = V * diag(1 ./ diag(S)) * U';
t_svd = toc;

disp('--- Inversa por método directo (inv) ---');
disp(Ainv_direct);
disp('--- Inversa vía factorización QR ---');
disp(Ainv_qr);
disp('--- Inversa vía SVD ---');
disp(Ainv_svd);

res_direct = norm(A*Ainv_direct - eye(n));
res_qr     = norm(A*Ainv_qr    - eye(n));
res_svd    = norm(A*Ainv_svd   - eye(n));

diff_qr_direct  = norm(Ainv_qr  - Ainv_direct);
diff_svd_direct = norm(Ainv_svd - Ainv_direct);

printf('\nValores singulares de A (diag de S):\n');
disp(diag(S)');
printf('cond(A) via SVD (sigma_max/sigma_min) = %e\n\n', S(1,1)/S(n,n));

printf('===================================================================\n');
printf(' Método    | Tiempo (s)  | ||A*Ainv - I||  | ||Ainv - Ainv_directo||\n');
printf('-------------------------------------------------------------------\n');
printf(' Directo   | %10.6f  | %10.3e  |     ---\n',        t_direct, res_direct);
printf(' QR        | %10.6f  | %10.3e  |  %10.3e\n', t_qr,    res_qr,  diff_qr_direct);
printf(' SVD       | %10.6f  | %10.3e  |  %10.3e\n', t_svd,   res_svd, diff_svd_direct);
printf('===================================================================\n\n');

printf([ ...
'Análisis:\n' ...
' - Los tres métodos dan prácticamente la misma inversa para esta\n' ...
'   matriz A (bien condicionada); diferencias del orden de la\n' ...
'   precisión de máquina (~1e-13 a 1e-15).\n' ...
' - Estabilidad numérica, de más a menos robusto ante mal\n' ...
'   condicionamiento:\n' ...
'     1) SVD:    la más estable; muestra explícitamente qué tan\n' ...
'        cerca está A de ser singular (sigma_min pequeño).\n' ...
'     2) QR:     estable por usar reflexiones ortogonales.\n' ...
'     3) Directo (inv, basado en LU): más rápido y barato, pero el\n' ...
'        que más amplifica errores si A está mal condicionada.\n' ...
' - Costo: Directo/LU ~ O(n^3), QR ~ 2x ese costo, SVD es el más\n' ...
'   caro; por eso en la práctica SVD se usa solo cuando la matriz\n' ...
'   es sospechosa de estar mal condicionada o casi singular.\n']);



