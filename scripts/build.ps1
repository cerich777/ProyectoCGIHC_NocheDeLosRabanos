param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [switch]$SmokeTest,
    [string]$CaptureDir
)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (!(Test-Path -LiteralPath $vswhere)) { throw 'Instala Visual Studio 2022 con Desarrollo para el escritorio con C++.' }
$msbuild = & $vswhere -latest -version '[17.0,18.0)' -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (!$msbuild) { throw 'Falta MSBuild o la carga de trabajo de C++ de Visual Studio 2022.' }
& $msbuild (Join-Path $repoRoot 'NocheDeLosRabanos.sln') /m /nologo "/p:Configuration=$Configuration" /p:Platform=Win32 /verbosity:minimal
if ($LASTEXITCODE -ne 0) { throw 'La compilacion fallo.' }
if ($SmokeTest -or $CaptureDir) {
    $testArguments = @('--smoke-test')
    if ($CaptureDir) { $testArguments += @('--capture-dir', [IO.Path]::GetFullPath($CaptureDir)) }
    & (Join-Path $repoRoot "build\Win32\$Configuration\NocheDeLosRabanos.exe") @testArguments
    if ($LASTEXITCODE -ne 0) { throw 'La prueba de inicio fallo.' }
}
