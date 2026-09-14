% Punto 2: Problema práctico - Ajuste de modelo lineal DC-DC

datos = csvread('datos_convertidor_realista.csv', 1, 0); % salta encabezado

V = datos(:,1);
I = datos(:,2);
T = datos(:,3);
P = datos(:,4);

n = length(P);
X = [ones(n,1), V, I, T];
y = P;

% --- 1. Coeficientes por mínimos cuadrados (forma estable con QR) ---
beta = X \ y;
fprintf("Coeficientes [beta0 beta1 beta2 beta3]:\n");
disp(beta');

% --- 2. Influencia relativa (usando el rango de cada variable) ---
impacto_V = beta(2) * (max(V) - min(V));
impacto_I = beta(3) * (max(I) - min(I));
impacto_T = beta(4) * (max(T) - min(T));
fprintf("\nImpacto relativo (beta_i * rango_i):\n");
fprintf("V: %.4f | I: %.4f | T: %.4f\n", impacto_V, impacto_I, impacto_T);

% --- 3. Residuales y ECM ---
y_pred = X * beta;
residual = y - y_pred;
ECM = mean(residual.^2);
fprintf("\nError Cuadratico Medio (ECM): %.4f\n", ECM);

% --- 5. Número de condición de X ---
condX = cond(X);
fprintf("\nNumero de condicion de X: %.4f\n", condX);
