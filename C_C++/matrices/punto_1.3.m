% ============================================================
%  punto3_condicionamiento.m
%
%  PARTE 3: Condicionamiento numérico y estabilidad
%     - Número de condición de A
%     - Solución con A original vs. A perturbada (perturbación pequeña)
%     - Análisis del error y discusión de estabilidad
%
%  Usa la misma matriz A y vector b del ejercicio anterior (10x10).
%
%  Ejecutar en Octave, parado en esta carpeta:
%       punto3_condicionamiento
% ============================================================
clc; format short;

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

b = [12; 18; 15; 20; 25; 14; 16; 22; 19; 17];
n = length(b);

printf('=====================================================\n');
printf(' PARTE 3: Condicionamiento numérico y estabilidad\n');
printf('=====================================================\n\n');

% --- Número de condición (norma 2, vía singular values) ---
condA = cond(A);
printf('Número de condición de A (norma 2): %e\n', condA);
if condA < 100
    printf('(cond(A) es moderado; A no está severamente mal\n');
    printf(' condicionada, pero sirve igual para ver el efecto de\n');
    printf(' una perturbación pequeña.)\n\n');
else
    printf('(cond(A) es alto: A está mal condicionada.)\n\n');
end

% --- Perturbación pequeña y estructurada de A ---
deltaA = zeros(n);
for i = 1:n
    for j = 1:n
        deltaA(i,j) = 1e-4 * (-1)^(i+j);
    end
end
A_pert = A + deltaA;

x_orig = A \ b;
x_pert = A_pert \ b;

disp('--- Solución con A original ---');
disp(x_orig');
disp('--- Solución con A perturbada (A + deltaA), deltaA ~ 1e-4 ---');
disp(x_pert');

err_A_rel = norm(deltaA) / norm(A);
err_x_rel = norm(x_pert - x_orig) / norm(x_orig);
amplif    = err_x_rel / err_A_rel;

printf('\n||deltaA|| / ||A||                 = %e  (perturbación relativa en A)\n', err_A_rel);
printf('||x_pert - x_orig|| / ||x_orig||   = %e  (error relativo resultante en x)\n', err_x_rel);
printf('Factor de amplificación observado  = %.4f\n', amplif);
printf('cond(A) (cota teórica superior)    = %.4f\n\n', condA);

printf([ ...
'Análisis:\n' ...
' - La teoría dice que ||dx||/||x|| <= cond(A) * ||dA||/||A||,\n' ...
'   es decir, cond(A) es una COTA del factor de amplificación del\n' ...
'   error, no el valor exacto.\n' ...
' - Aquí el factor observado (%.2f) es menor que cond(A) (%.2f),\n' ...
'   lo cual es consistente con la teoría.\n' ...
' - Si A estuviera muy mal condicionada (cond(A) >> 1), la misma\n' ...
'   perturbación de 1e-4 podría producir errores relativos enormes\n' ...
'   en la solución.\n' ...
' - Gauss y LU con pivoteo parcial son estables para matrices bien\n' ...
'   condicionadas como esta; QR es aún más estable porque usa\n' ...
'   transformaciones ortogonales que no amplifican el error de\n' ...
'   redondeo. La diferencia entre métodos se vuelve importante\n' ...
'   sobre todo cuando cond(A) es grande.\n'], amplif, condA);

