# Script per instal·lar MinGW-w64 automàticament

Write-Host "=== Instal·lador de MinGW-w64 ===" -ForegroundColor Green
Write-Host ""

# Comprovar si ja està instal·lat
if (Test-Path "C:\msys64\mingw64\bin\g++.exe") {
    Write-Host "MinGW-w64 ja esta instal·lat!" -ForegroundColor Yellow
    Write-Host "Ruta: C:\msys64\mingw64\bin" -ForegroundColor Yellow

    # Afegir al PATH si no hi és
    $currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
    if ($currentPath -notlike "*C:\msys64\mingw64\bin*") {
        Write-Host "Afegint al PATH..." -ForegroundColor Cyan
        [Environment]::SetEnvironmentVariable("Path", "$currentPath;C:\msys64\mingw64\bin", "User")
        Write-Host "PATH actualitzat!" -ForegroundColor Green
        Write-Host "IMPORTANT: Reinicia Visual Studio Code per aplicar els canvis." -ForegroundColor Yellow
    }

    Write-Host ""
    g++ --version
    exit 0
}

# Descarregar MSYS2
Write-Host "Descarregant MSYS2..." -ForegroundColor Cyan
$msys2Url = "https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe"
$installerPath = "$env:TEMP\msys2-installer.exe"

try {
    Invoke-WebRequest -Uri $msys2Url -OutFile $installerPath -UseBasicParsing
    Write-Host "Descarrega completada!" -ForegroundColor Green
} catch {
    Write-Host "Error al descarregar MSYS2. Prova manualment:" -ForegroundColor Red
    Write-Host "https://www.msys2.org/" -ForegroundColor Yellow
    exit 1
}

# Instal·lar MSYS2 silenciosament
Write-Host ""
Write-Host "Instal·lant MSYS2..." -ForegroundColor Cyan
Write-Host "Aixo pot trigar uns minuts..." -ForegroundColor Yellow

try {
    Start-Process -FilePath $installerPath -ArgumentList "install", "--root", "C:\msys64", "--confirm-command" -Wait -NoNewWindow
    Write-Host "MSYS2 instal·lat correctament!" -ForegroundColor Green
} catch {
    Write-Host "Error durant la instal·lacio. Executant instal·lador manual..." -ForegroundColor Yellow
    Start-Process -FilePath $installerPath -Wait
}

# Esperar que es crei el directori
Start-Sleep -Seconds 3

# Actualitzar MSYS2 i instal·lar GCC
Write-Host ""
Write-Host "Instal·lant GCC i eines de compilacio..." -ForegroundColor Cyan

$msys2Bash = "C:\msys64\usr\bin\bash.exe"

if (Test-Path $msys2Bash) {
    # Actualitzar paquets
    & $msys2Bash -lc "pacman -Syu --noconfirm"
    Start-Sleep -Seconds 2

    # Instal·lar GCC
    & $msys2Bash -lc "pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-make"

    Write-Host "GCC instal·lat correctament!" -ForegroundColor Green
} else {
    Write-Host "MSYS2 no s'ha trobat a C:\msys64" -ForegroundColor Red
    Write-Host "Prova d'instal·lar-lo manualment des de: https://www.msys2.org/" -ForegroundColor Yellow
    exit 1
}

# Afegir al PATH
Write-Host ""
Write-Host "Configurant PATH..." -ForegroundColor Cyan

$mingwPath = "C:\msys64\mingw64\bin"
$currentPath = [Environment]::GetEnvironmentVariable("Path", "User")

if ($currentPath -notlike "*$mingwPath*") {
    [Environment]::SetEnvironmentVariable("Path", "$currentPath;$mingwPath", "User")
    Write-Host "PATH actualitzat correctament!" -ForegroundColor Green
} else {
    Write-Host "PATH ja conte MinGW!" -ForegroundColor Yellow
}

# Netejar
Remove-Item $installerPath -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "=== INSTAL·LACIO COMPLETADA ===" -ForegroundColor Green
Write-Host ""
Write-Host "IMPORTANT: Reinicia Visual Studio Code per aplicar els canvis." -ForegroundColor Yellow
Write-Host ""
Write-Host "Prova de compilar amb:" -ForegroundColor Cyan
Write-Host "  g++ -fopenmp -o main.exe src/main.cpp" -ForegroundColor White
Write-Host ""

# Verificar instal·lació
if (Test-Path "C:\msys64\mingw64\bin\g++.exe") {
    Write-Host "Verificacio:" -ForegroundColor Cyan
    & "C:\msys64\mingw64\bin\g++.exe" --version
} else {
    Write-Host "ATENCIO: No s'ha pogut verificar la instal·lacio." -ForegroundColor Yellow
    Write-Host "Pot ser que hagis de reiniciar el sistema." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Prem qualsevol tecla per tancar..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
