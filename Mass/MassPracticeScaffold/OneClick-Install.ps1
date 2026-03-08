param(
    [Parameter(Mandatory = $true)]
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$SourcePluginPath = Join-Path $ScriptRoot "Plugins\MassPractice"

if (-not (Test-Path $ProjectRoot)) {
    throw "ProjectRoot does not exist: $ProjectRoot"
}

if (-not (Test-Path $SourcePluginPath)) {
    throw "Scaffold plugin folder not found: $SourcePluginPath"
}

$ProjectRootResolved = (Resolve-Path $ProjectRoot).Path
$TargetPluginsDir = Join-Path $ProjectRootResolved "Plugins"
$TargetPluginPath = Join-Path $TargetPluginsDir "MassPractice"

if (-not (Test-Path $TargetPluginsDir)) {
    New-Item -ItemType Directory -Path $TargetPluginsDir | Out-Null
}

Write-Host "[1/2] Copying plugin scaffold to: $TargetPluginPath"
Copy-Item -Path $SourcePluginPath -Destination $TargetPluginsDir -Recurse -Force

Write-Host "[2/2] Done."
Write-Host ""
Write-Host "Next steps:"
Write-Host "  1) Regenerate project files for your project."
Write-Host "  2) Open Unreal Editor, ensure plugin 'MassPractice' is enabled."
Write-Host "  3) Create a Mass Entity Config Asset and add trait: 'MassPractice Trait'."
Write-Host "  4) (Optional) Add 'DistanceLODCollector' + 'SimulationLOD' traits for variable tick demo."
Write-Host "  5) Place AMassPracticeSpawnerActor in level and assign the config asset."

