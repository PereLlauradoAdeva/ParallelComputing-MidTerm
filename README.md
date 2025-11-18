# Morphological Image Processing - Parallel Programming Project

## Descripció del Projecte

Implementació d'operacions morfològiques (erosion, dilation, opening, closing) sobre imatges en grayscale o binàries, amb versions seqüencial i paral·lela utilitzant OpenMP i tècniques de vectorització.

## Objectius

1. **Fase 1 - Implementació Seqüencial:**
   - Implementar operacions morfològiques bàsiques sense paral·lelització
   - Validar correcció de les operacions
   - Establir baseline de rendiment

2. **Fase 2 - Paral·lelització:**
   - Afegir OpenMP per paral·lelització
   - Aplicar tècniques de vectorització
   - Optimitzar rendiment

3. **Fase 3 - Anàlisi de Rendiment:**
   - Mesurar speedup amb diferents threads (1, 2, 4, 8, 16)
   - Provar diferents mides d'imatges
   - Comparar estratègies de scheduling
   - Analitzar escalabilitat

## Operacions Morfològiques a Implementar

- [ ] **Erosion:** Redueix regions brillants, elimina petits objectes
- [ ] **Dilation:** Expandeix regions brillants, omple forats petits
- [ ] **Opening:** Erosion seguida de dilation (elimina soroll)
- [ ] **Closing:** Dilation seguida d'erosion (omple forats)

## Especificacions Tècniques

### Input
- Imatges en grayscale (1 canal, valors 0-255)
- Format: preferiblement PNG, JPG o PGM
- Mides a provar: 512×512, 1024×1024, 2048×2048, 4096×4096

### Element Estructurant
- Forma: rectangular o circular
- Mides a provar: 3×3, 5×5, 7×7, 11×11

### Output
- Imatge processada en el mateix format
- Temps d'execució per cada operació
- Estadístiques de rendiment

## Estructura del Projecte

```
Projecte mid term/
├── README.md
├── src/
│   └── main.cpp              # Tot el codi en un sol fitxer
├── archive/                  # Base de dades d'imatges
├── images/
│   ├── input/                # Imatges d'entrada per testing
│   └── output/               # Resultats
├── results/
│   └── performance.csv       # Dades de rendiment
└── build.bat                 # Script de compilació
```

## Fases d'Implementació

### Fase 1: Versió Seqüencial (Setmana 1-2)
1. Càrrega i emmagatzematge d'imatges (biblioteca stb_image)
2. Implementació d'erosion seqüencial
3. Implementació de dilation seqüencial
4. Implementació d'opening i closing
5. Validació de correcció
6. Mesura de temps base

### Fase 2: Paral·lelització (Setmana 3)
1. Afegir OpenMP a les funcions principals
2. Paral·lelització a nivell de píxel
3. Provar diferents estratègies de scheduling
4. Gestionar correctament variables compartides/privades
5. Evitar race conditions

### Fase 3: Optimització (Setmana 4)
1. Vectorització amb flags del compilador
2. Optimització d'accés a memòria
3. Provar diferents mides de chunk
4. Minimitzar overhead de sincronització

### Fase 4: Experimentació i Anàlisi (Setmana 5)
1. Executar experiments amb diferents configuracions
2. Recollir dades (mínim 5 execucions per configuració)
3. Calcular speedup i efficiency
4. Generar gràfiques
5. Analitzar bottlenecks

### Fase 5: Documentació (Setmana 6)
1. Escriure informe tècnic
2. Preparar presentació
3. Documentar codi

## Compilació i Execució

### Requisits
- Compilador amb suport C++11 o superior
- OpenMP
- Biblioteca stb_image (header-only, ja inclosa al codi)

### Opció 1: MinGW-w64 (Recomanat)
1. Descarrega i instal·la MSYS2: https://www.msys2.org/
2. Obre MSYS2 i executa:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
3. Afegeix al PATH: `C:\msys64\mingw64\bin`

### Opció 2: Visual Studio Build Tools
1. Descarrega Build Tools: https://visualstudio.microsoft.com/downloads/
2. Selecciona "Build Tools for Visual Studio 2022"
3. Marca "Desktop development with C++"

### Compilar Versió Seqüencial
```bash
g++ -std=c++11 -O3 -o morphology_seq.exe src/main.cpp
```

### Compilar Versió Paral·lela
```bash
g++ -std=c++11 -O3 -fopenmp -o morphology_par.exe src/main.cpp
```

### Executar
```bash
# Versió seqüencial
./morphology_seq.exe input.png output.png erosion 5

# Versió paral·lela amb 8 threads
set OMP_NUM_THREADS=8
./morphology_par.exe input.png output.png dilation 7
```

## Mètriques de Rendiment

- **Temps d'execució:** Wall-clock time (omp_get_wtime())
- **Speedup:** T_seq / T_par(p)
- **Efficiency:** Speedup(p) / p
- **Throughput:** Píxels processats per segons

## Oportunitats de Paral·lelització

1. **Pixel-level parallelism:** Cada píxel es processa independentment
2. **Sliding window parallelism:** Cada finestra es processa en paral·lel
3. **Pipeline parallelism:** Per operacions compostes (opening/closing)

## Timeline Estimat

| Setmana | Tasques |
|---------|---------|
| 1-2 | Implementació seqüencial + validació |
| 3 | Paral·lelització amb OpenMP |
| 4 | Optimització i vectorització |
| 5 | Experimentació i anàlisi de dades |
| 6 | Informe i presentació |

## Referències

- Morphological operations: https://en.wikipedia.org/wiki/Mathematical_morphology
- OpenCV examples: https://docs.opencv.org/4.x/d9/d61/tutorial_py_morphological_ops.html
- OpenMP documentation: https://www.openmp.org/specifications/
- stb_image library: https://github.com/nothings/stb

## Extensions Recomanades per VSCode

- C/C++ (Microsoft)
- C/C++ Extension Pack (Microsoft)

## Autor

[El teu nom]

Universitat degli Studi di Firenze - Parallel Programming Course
