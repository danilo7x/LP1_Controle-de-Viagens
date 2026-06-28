# compilar.ps1
# Script de compilacao para Windows (PowerShell).
# No Windows o servidor web precisa linkar com a biblioteca de sockets (ws2_32).
# Execute no terminal do VS Code com:  .\compilar.ps1

Write-Host "Compilando o modo TERMINAL..." -ForegroundColor Cyan
g++ -std=c++17 -Iinclude (Get-ChildItem src\*.cpp).FullName main.cpp -o controlador.exe
if ($LASTEXITCODE -eq 0) { Write-Host "  -> controlador.exe pronto" -ForegroundColor Green }

Write-Host "Compilando o modo DASHBOARD (web)..." -ForegroundColor Cyan
g++ -std=c++17 -Iinclude -Ilib -O2 (Get-ChildItem src\*.cpp).FullName servidor.cpp -o servidor.exe -lws2_32
if ($LASTEXITCODE -eq 0) { Write-Host "  -> servidor.exe pronto" -ForegroundColor Green }

Write-Host ""
Write-Host "Pronto! Para usar:" -ForegroundColor Yellow
Write-Host "  Terminal:  .\controlador.exe"
Write-Host "  Dashboard: .\servidor.exe   (depois abra http://localhost:8080 no navegador)"
