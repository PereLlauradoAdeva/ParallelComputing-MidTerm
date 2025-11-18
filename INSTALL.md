# Instruccions d'Instal·lació i Compilació

## Pas 1: Descarregar les biblioteques STB

Les biblioteques `stb_image` són header-only i cal descarregar-les manualment.

### Opció A: Descàrrega manual

1. Crea una carpeta `include` al projecte:
```bash
mkdir include
```

2. Descarrega aquests dos fitxers:
   - **stb_image.h**: https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
   - **stb_image_write.h**: https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

3. Guarda'ls a la carpeta `include/`

### Opció B: Usar curl (des de terminal)

```bash
cd "C:\Users\Lenovo\Desktop\UNIFI\Parallel\Projecte mid term"
mkdir include
cd include
curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
```

### Opció C: Usar PowerShell

```powershell
New-Item -ItemType Directory -Force -Path "include"
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" -OutFile "include/stb_image.h"
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h" -OutFile "include/stb_image_write.h"
```

## Pas 2: Actualitzar main.cpp

Després de descarregar les biblioteques, has de reemplaçar les línies 22-52 del `main.cpp` amb:

```cpp
// ============================================================================
// STB_IMAGE LIBRARY (Header-only)
// ============================================================================
#include "../include/stb_image.h"
#include "../include/stb_image_write.h"
```

## Pas 3: Compilar

### Amb MinGW (Recomanat)

```bash
# Versió seqüencial
g++ -std=c++11 -O3 -o morphology_seq.exe src/main.cpp

# Provar compilació
./morphology_seq.exe
```

### Amb MSVC (Visual Studio)

```cmd
cl /EHsc /O2 /Fe:morphology_seq.exe src\main.cpp
```

## Pas 4: Provar el programa

### Preparar imatges de prova

1. Crea les carpetes necessàries:
```bash
mkdir images
mkdir images\input
mkdir images\output
```

2. Copia algunes imatges del teu dataset `archive` a `images/input/`

### Executar

```bash
# Erosion
./morphology_seq.exe images/input/test.png images/output/test_erosion.png erosion 5

# Dilation
./morphology_seq.exe images/input/test.png images/output/test_dilation.png dilation 5

# Opening (elimina soroll)
./morphology_seq.exe images/input/test.png images/output/test_opening.png opening 7

# Closing (omple forats)
./morphology_seq.exe images/input/test.png images/output/test_closing.png closing 7
```

## Resolució de problemes

### Error: "stbi_load is not defined"

- Assegura't que has descarregat les biblioteques STB correctament
- Verifica que la ruta `../include/` és correcta

### Error: "Cannot open file"

- Comprova que la imatge d'entrada existeix
- Verifica que les carpetes de sortida existeixen

### L'imatge es veu mal

- El programa converteix automàticament a grayscale
- Si vols mantenir els canals RGB, hauràs de modificar el codi

### Compilació lenta

- És normal la primera vegada per les biblioteques STB
- Compilacions següents seran més ràpides

## Estructura final del projecte

Després de seguir aquests passos:

```
Projecte mid term/
├── README.md
├── INSTALL.md            (aquest fitxer)
├── src/
│   └── main.cpp
├── include/              (nou)
│   ├── stb_image.h      (descarregat)
│   └── stb_image_write.h (descarregat)
├── images/               (nou)
│   ├── input/
│   └── output/
├── archive/              (dataset existent)
└── build.bat
```

## Següents passos

Un cop funcioni la versió seqüencial:

1. ✅ Validar que les operacions morfològiques funcionen correctament
2. ✅ Mesurar temps d'execució base
3. ✅ Provar amb diferents mides d'imatge i kernels
4. 🔜 Passar a la Fase 2: Paral·lelització amb OpenMP

