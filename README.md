# Parallel Computing - MidTerm Project

Projecte de l'assignatura de Parallel Computing.

## Descripció

Implementació de l'operació morfològica d'apertura (erosió + dilatació) sobre imatges en escala de grisos. Compara la versió seqüencial amb la paral·lela (OpenMP) i calcula el speedup.

## Compilar

```bash
g++ -fopenmp -O2 -std=c++17 main.cpp sequential.cpp parallel.cpp -o main.exe
```

O executar `compile.bat`

## Executar

```bash
./main.exe [max_imatges] [num_threads]
```

## Arxius

- `main.cpp` - Programa principal
- `sequential.cpp/h` - Operacions seqüencials
- `parallel.cpp/h` - Operacions amb OpenMP
- `libs/` - Llibreries STB per imatges
- `compile.bat` - Script de compilació
