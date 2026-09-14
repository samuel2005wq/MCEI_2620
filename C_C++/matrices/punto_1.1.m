% Solución al Problema 1.1 del Taller sobre Algebra lineal numérica
% Nombre de quienes lo desarrollaron: Samuel Acuña - Carolina García

% Matriz a realizar:
A = [1 0 2; 2 -1 5; 0 1 -1; 1 3 -1];

fprintf("Matriz con la que vamos a trabajar:\n");
disp(A);

% Se mira el rango de la matriz:
rA = rank(A);

% Obtenemos la matriz B = (A^T)*A:
At = A'; %<-- Inversa de A
B = At*A;

%Obtenemos la matriz C = Tr(B)I_3 - B:
trB = trace(B); %<-- La traza de B
I_3 = eye(3);   %<-- Matriz identidad 3x3 para este caso

C = (trB*I_3) - B;

%Obtenemos la pseudoinversa de la matriz A:
trCB = trace(C*B); %<-- La traza de CB
CAt = C*At; %<-- C* A^T

A_pseudoinversa = (rA/trCB)*CAt;

fprintf("Inversa de Moore Penrose calculada manualmente:\n");
disp(A_pseudoinversa);

% Se mira la inversa Moore Penrose con la función pinv:
A_inv = pinv(A);

fprintf("\nInversa de Moore Penrose con la función pinv:\n");
disp(A_inv);

%==================================================================
% PROPIEDADES
%==================================================================
%Nota: para mirar las propiedades, se va a usar A_inv (la calculada con la función pinv)

%Propiedad 1: A*A⁻*A = A
P_1 = A*A_inv*A;

fprintf("\nPropiedad No. 1: A*A⁻*A = A\n");
disp(P_1);

%Propiedad 2: A⁻*A*A⁻ = A⁻
P_2 = A_inv*A*A_inv;

fprintf("\nPropiedad No. 2: A⁻*A*A⁻ = A⁻\n");
disp(P_2);

%Propiedad 3: A*A⁻ es simétrica
P_3 = A*A_inv;
P_31 = P_3';

fprintf("\nPropiedad No. 3: A*A⁻ es simétrica\n");
fprintf("\nMatriz A*A⁻:\n");
disp(P_3);
fprintf("\nTranspuesta de A*A⁻:\n");
disp(P_31);
fprintf("\nComo se puede notar, la matriz A*A⁻ si es simétrica\n");

%Propiedad 4: A⁻*A es simétrica
P_4 = A_inv*A;
P_41 = P_4';

fprintf("\nPropiedad No. 4: A⁻A es simétrica\n");
fprintf("\nMatriz A⁻A:\n");
disp(P_4);
fprintf("\nTranspuesta de A⁻A:\n");
disp(P_41);
fprintf("\nComo se puede notar, la matriz A*A⁻ si es simétrica\n");
