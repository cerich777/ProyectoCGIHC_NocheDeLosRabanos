param([string]$OutputDir)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
if (!$OutputDir) { $OutputDir = Join-Path $repoRoot 'entrega' }
& (Join-Path $PSScriptRoot 'build.ps1') -Configuration Release -SmokeTest
$stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
$package = Join-Path ([IO.Path]::GetFullPath($OutputDir)) "NocheDeLosRabanos_Win32_$stamp"
if (Test-Path -LiteralPath $package) { throw 'El paquete ya existe.' }
New-Item -ItemType Directory -Path $package -Force | Out-Null
$release = Join-Path $repoRoot 'build\Win32\Release'
foreach ($name in @('NocheDeLosRabanos.exe','glew32.dll','glfw3.dll','assimp-vc140-mt.dll','Models','images','Shader')) {
    Copy-Item -LiteralPath (Join-Path $release $name) -Destination $package -Recurse
}
Copy-Item -LiteralPath (Join-Path $repoRoot 'docs\MANUAL_USUARIO.md') -Destination $package
# Ejecutar desde la copia empaquetada prueba las rutas relativas de todos los recursos.
& (Join-Path $package 'NocheDeLosRabanos.exe') --smoke-test
if ($LASTEXITCODE -ne 0) { throw 'El paquete no paso la prueba de arranque.' }
$zipPath = "$package.zip"
Compress-Archive -LiteralPath $package -DestinationPath $zipPath -CompressionLevel Optimal
Write-Output "Paquete verificado: $zipPath"
