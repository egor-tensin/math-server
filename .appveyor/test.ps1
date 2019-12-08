param(
    [string] $BuildDir = $null,
    [string] $Platform = $null
)

$ErrorActionPreference = "Stop";
Set-PSDebug -Strict

function Invoke-Exe {
    param(
        [ScriptBlock] $Cmd,
        [int[]] $AllowedExitCodes = @(0)
    )
 
    $backupErrorActionPreference = $script:ErrorActionPreference
    $script:ErrorActionPreference = 'Continue'

    try {
        & $Cmd
        if ($AllowedExitCodes -notcontains $LastExitCode) {
            throw "External command failed with exit code ${LastExitCode}: $Cmd"
        }
    } finally {
        $script:ErrorActionPreference = $backupErrorActionPreference
    }
}

function Test-AppVeyor {
    return Test-Path env:APPVEYOR
}

function Get-AppVeyorBuildDir {
    return 'C:\Projects\build'
}

function Set-AppVeyorDefaults {
    $script:BuildDir = Get-AppVeyorBuildDir
    $script:Configuration = $env:CONFIGURATION
}

function Run-ProjectTests {
    param(
        [Parameter(Mandatory=$true)]
        [string] $BuildDir,
        [Parameter(Mandatory=$true)]
        [string] $Configuration
    )

    $unit_tests_dir = "$BuildDir\test\unit_tests\$Configuration"
    cd $unit_tests_dir

    Invoke-Exe { .\unit_tests.exe --log_level=all }
}

function Run-ProjectTestsAppVeyor {
    if (Test-AppVeyor) {
        Set-AppVeyorDefaults
        $appveyor_cwd = pwd
    }

    try {
        Run-ProjectTests               `
            -BuildDir $script:BuildDir `
            -Configuration $script:Configuration
    } finally {
        if (Test-AppVeyor) {
            cd $appveyor_cwd
            Set-PSDebug -Off
        }
    }
}

Run-ProjectTestsAppVeyor
