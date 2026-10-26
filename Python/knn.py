
import time
import numpy as np
import pandas as pd
from scipy.spatial import KDTree
from sklearn.neighbors import NearestNeighbors

# 1. Cargar datos (separador punto y coma)
df = pd.read_csv('Datos_MCEI.csv', sep=';')


# 2. Función opcional para reconstruir decimales eliminados por Excel
def reconstruir_decimales(df_in):
  df_rec = df_in.copy()
  df_rec['voltaje_V'] = df_in['voltaje_V'].apply(
      lambda v: v / 10.0 if v < 2000 else v / 100.0
  )
  df_rec['corriente_A'] = df_in['corriente_A'].apply(
      lambda i: i / 10.0 if i < 100 else i / 100.0
  )
  df_rec['potencia_W_aprox'] = df_in['potencia_W_aprox'].apply(
      lambda p: p / 10.0 if p < 10000 else p / 100.0
  )
  return df_rec


df_fisico = reconstruir_decimales(df)


# 3. Función que ejecuta KNN, kd-tree y compara métricas de distancia
def analizar_vecinos(
    dataframe, idx_anomalo=99, normalizar=True, nombre_escenario=''
):
  X = dataframe[['voltaje_V', 'corriente_A', 'potencia_W_aprox']].values.astype(
      float
  )
  if normalizar:
    X_proc = (X - X.mean(axis=0)) / X.std(axis=0)
  else:
    X_proc = X

  sensor_q = X_proc[idx_anomalo]
  resto_idx = np.delete(np.arange(len(X)), idx_anomalo)
  X_resto = X_proc[resto_idx]

  # Construcción del kd-tree (se realiza una sola vez para el conjunto de datos)
  t0 = time.perf_counter()
  arbol = KDTree(X_resto)
  t_build = (time.perf_counter() - t0) * 1e6

  print(f'==============================================================')
  print(f'=== {nombre_escenario} ===')
  print(
      f'Sensor evaluado: Muestra {dataframe.loc[idx_anomalo, "muestra"]} ->'
      f' Valores: {X[idx_anomalo]}'
  )
  print(f'Tiempo construcción kd-tree: {t_build:.2f} µs\n')

  # Diccionario de métricas a evaluar (norma p de Minkowski)
  metricas = {
      'Euclidiana (L2, p=2)': 2,
      'Manhattan  (L1, p=1)': 1,
      'Chebyshev  (L∞, p=∞)': np.inf,
  }

  for nombre_metrica, p_val in metricas.items():
    # Método 1: KNN Fuerza Bruta con norma ord=p_val
    t0 = time.perf_counter()
    dist_knn = np.linalg.norm(X_resto - sensor_q, ord=p_val, axis=1)
    orden = np.argsort(dist_knn)[:5]
    t_knn = (time.perf_counter() - t0) * 1e6
    vecinos_knn = dataframe.loc[resto_idx[orden], 'muestra'].values

    # Método 2: kd-tree consultando con la misma métrica p=p_val
    t0 = time.perf_counter()
    dist_kd, idx_kd = arbol.query(sensor_q, k=5, p=p_val, workers=-1)
    t_query = (time.perf_counter() - t0) * 1e6
    vecinos_kd = dataframe.loc[resto_idx[idx_kd], 'muestra'].values

    print(f'--- Métrica: {nombre_metrica} ---')
    print(f'5 Vecinos KNN (Muestras):     {vecinos_knn}')
    print(f'5 Vecinos kd-tree (Muestras): {vecinos_kd}')
    print(f'Distancias:                   {np.round(dist_kd, 4)}')
    print(
        f'Tiempos -> KNN: {t_knn:.2f} µs | Query kd-tree: {t_query:.2f} µs\n'
    )


# Ejecución para ambos escenarios sobre la Muestra 100 (índice 99)
analizar_vecinos(
    df_fisico,
    idx_anomalo=99,
    normalizar=True,
    nombre_escenario='Escenario A: Datos Reconstruidos (Z-Score)',
)
analizar_vecinos(
    df,
    idx_anomalo=99,
    normalizar=True,
    nombre_escenario='Escenario B: Datos Crudos CSV (Z-Score)',
)


