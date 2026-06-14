param(
    [ValidateSet("Open", "Locate", "Identify", "Manual")]
    [string]$Mode = "Identify",

    [int]$Seconds = 30,

    [string]$UsbPcapDevice = "\\.\USBPcap1",

    [int]$DeviceAddress = 0,

    [string]$Output,

    [switch]$WinBioWorker
)

$ErrorActionPreference = "Stop"

function Find-Tool {
    param(
        [string]$Name,
        [string[]]$Candidates
    )

    $cmd = Get-Command $Name -ErrorAction SilentlyContinue
    if($cmd) { return $cmd.Source }

    foreach($candidate in $Candidates) {
        if(Test-Path $candidate) { return $candidate }
    }

    throw "Could not find $Name"
}

function Invoke-WinBioProbe {
    param([string]$ProbeMode)

    $src = @'
using System;
using System.Runtime.InteropServices;

public static class TudorWinBioProbe {
    const uint WINBIO_TYPE_FINGERPRINT = 0x00000008;
    const uint WINBIO_POOL_SYSTEM = 1;
    const uint WINBIO_FLAG_DEFAULT = 0;

    [DllImport("winbio.dll")]
    static extern int WinBioEnumBiometricUnits(uint Factor, out IntPtr UnitSchemaArray, out UIntPtr UnitCount);

    [DllImport("winbio.dll")]
    static extern int WinBioOpenSession(uint Factor, uint PoolType, uint Flags, IntPtr UnitArray, UIntPtr UnitCount, IntPtr DatabaseId, out IntPtr SessionHandle);

    [DllImport("winbio.dll")]
    static extern int WinBioLocateSensor(IntPtr SessionHandle, out uint UnitId);

    [DllImport("winbio.dll")]
    static extern int WinBioIdentify(IntPtr SessionHandle, out uint UnitId, IntPtr Identity, out byte SubFactor, out uint RejectDetail);

    [DllImport("winbio.dll")]
    static extern int WinBioCloseSession(IntPtr SessionHandle);

    [DllImport("winbio.dll")]
    static extern void WinBioFree(IntPtr Address);

    public static void Run(string mode) {
        IntPtr units;
        UIntPtr count;
        int hr = WinBioEnumBiometricUnits(WINBIO_TYPE_FINGERPRINT, out units, out count);
        Console.WriteLine("WinBioEnumBiometricUnits hr=0x{0:X8} count={1}", hr, count.ToUInt64());
        if(units != IntPtr.Zero) WinBioFree(units);
        if(hr < 0) return;

        IntPtr session;
        hr = WinBioOpenSession(WINBIO_TYPE_FINGERPRINT, WINBIO_POOL_SYSTEM, WINBIO_FLAG_DEFAULT, IntPtr.Zero, UIntPtr.Zero, IntPtr.Zero, out session);
        Console.WriteLine("WinBioOpenSession hr=0x{0:X8} session=0x{1:X}", hr, session.ToInt64());
        if(hr < 0) return;

        try {
            if(string.Equals(mode, "Locate", StringComparison.OrdinalIgnoreCase)) {
                uint unitId;
                hr = WinBioLocateSensor(session, out unitId);
                Console.WriteLine("WinBioLocateSensor hr=0x{0:X8} unit={1}", hr, unitId);
            } else if(string.Equals(mode, "Identify", StringComparison.OrdinalIgnoreCase)) {
                uint unitId;
                byte subFactor;
                uint rejectDetail;
                IntPtr identity = Marshal.AllocHGlobal(512);
                try {
                    for(int i = 0; i < 512; i++) Marshal.WriteByte(identity, i, 0);
                    hr = WinBioIdentify(session, out unitId, identity, out subFactor, out rejectDetail);
                    Console.WriteLine("WinBioIdentify hr=0x{0:X8} unit={1} subFactor={2} reject={3}", hr, unitId, subFactor, rejectDetail);
                } finally {
                    Marshal.FreeHGlobal(identity);
                }
            }
        } finally {
            Console.WriteLine("WinBioCloseSession hr=0x{0:X8}", WinBioCloseSession(session));
        }
    }
}
'@

    Add-Type -TypeDefinition $src
    [TudorWinBioProbe]::Run($ProbeMode)
}

function Get-UsbPcapCaptureProcessIds {
    param([string]$CapturePath)

    $all = @(Get-CimInstance Win32_Process -Filter "Name = 'USBPcapCMD.exe'" -ErrorAction SilentlyContinue)
    $roots = @($all | Where-Object { $_.CommandLine -and $_.CommandLine.Contains($CapturePath) })
    $ids = @($roots | ForEach-Object { [int]$_.ProcessId })

    do {
        $before = $ids.Count
        $children = @(
            $all | Where-Object {
                $ids -contains [int]$_.ParentProcessId -and
                $ids -notcontains [int]$_.ProcessId
            }
        )
        $ids += @($children | ForEach-Object { [int]$_.ProcessId })
        $ids = @($ids | Select-Object -Unique)
    } while($ids.Count -gt $before)

    return $ids
}

function Stop-UsbPcapCapture {
    param([string]$CapturePath)

    $ids = @(Get-UsbPcapCaptureProcessIds -CapturePath $CapturePath)
    if($ids.Count -eq 0) { return }

    Stop-Process -Id $ids -Force -ErrorAction SilentlyContinue
    Wait-Process -Id $ids -Timeout 5 -ErrorAction SilentlyContinue
}

if($WinBioWorker) {
    Invoke-WinBioProbe -ProbeMode $Mode
    exit 0
}

if($Seconds -lt 1) {
    throw "-Seconds must be at least 1"
}

$usbPcap = Find-Tool -Name "USBPcapCMD.exe" -Candidates @(
    "C:\Program Files\USBPcap\USBPcapCMD.exe",
    "C:\Program Files\Wireshark\USBPcapCMD.exe",
    "C:\Program Files (x86)\USBPcap\USBPcapCMD.exe",
    "C:\Program Files (x86)\Wireshark\USBPcapCMD.exe"
)

$tsharkPath = $null
$tshark = Get-Command "tshark.exe" -ErrorAction SilentlyContinue
if($tshark) {
    $tsharkPath = $tshark.Source
} elseif(Test-Path "C:\Program Files\Wireshark\tshark.exe") {
    $tsharkPath = "C:\Program Files\Wireshark\tshark.exe"
}

$captureDir = Join-Path (Get-Location) "captures"
New-Item -ItemType Directory -Force $captureDir | Out-Null

if(-not $Output) {
    $stamp = Get-Date -Format "yyyyMMdd-HHmmss"
    $Output = Join-Path $captureDir "$stamp-$($Mode.ToLowerInvariant()).pcapng"
}

$Output = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Output)
New-Item -ItemType Directory -Force (Split-Path $Output -Parent) | Out-Null
$baseName = [IO.Path]::GetFileNameWithoutExtension($Output)
$winBioOut = Join-Path (Split-Path $Output -Parent) "$baseName.winbio.out.txt"
$winBioErr = Join-Path (Split-Path $Output -Parent) "$baseName.winbio.err.txt"
Remove-Item $Output,$winBioOut,$winBioErr -Force -ErrorAction SilentlyContinue

$captureArgs = @("-d", $UsbPcapDevice, "--inject-descriptors", "-o", $Output, "-s", "65535", "-b", "1048576")
if($DeviceAddress -gt 0) {
    $captureArgs += @("--devices", "$DeviceAddress")
} else {
    $captureArgs += "-A"
}

$captureJob = Start-Job -ScriptBlock {
    param($exe, $argv)
    & $exe @argv
} -ArgumentList $usbPcap,$captureArgs

try {
    for($i = 0; $i -lt 40 -and -not (Test-Path $Output); $i++) {
        Start-Sleep -Milliseconds 250
    }

    if(-not (Test-Path $Output)) {
        throw "USBPcap did not create $Output"
    }

    Write-Host "Capturing $Mode for $Seconds seconds -> $Output"
    if($DeviceAddress -gt 0) {
        Write-Host "USBPcap filter: device address $DeviceAddress"
    } else {
        Write-Host "USBPcap filter: full root hub"
    }

    $worker = $null
    if($Mode -ne "Manual") {
        $pwsh = (Get-Process -Id $PID).Path
        $workerArgs = @(
            "-NoLogo",
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            $PSCommandPath,
            "-WinBioWorker",
            "-Mode",
            $Mode
        )

        $worker = Start-Process -FilePath $pwsh -ArgumentList $workerArgs -WindowStyle Hidden -PassThru -RedirectStandardOutput $winBioOut -RedirectStandardError $winBioErr
    } else {
        Write-Host "Manual mode: run the Windows Hello flow now."
    }

    Start-Sleep -Seconds $Seconds

    if($worker -and -not $worker.HasExited) {
        Stop-Process -Id $worker.Id -Force -ErrorAction SilentlyContinue
    }
} finally {
    Stop-UsbPcapCapture -CapturePath $Output
    Stop-Job $captureJob -ErrorAction SilentlyContinue
    Receive-Job $captureJob -ErrorAction SilentlyContinue | Out-Null
    Remove-Job $captureJob -Force -ErrorAction SilentlyContinue
}

$capture = Get-Item $Output
Write-Host "Capture complete: $($capture.Length) bytes"

if(Test-Path $winBioOut) {
    Write-Host "WinBio stdout: $winBioOut"
    Get-Content $winBioOut
}
if(Test-Path $winBioErr) {
    $errContent = Get-Content $winBioErr
    if($errContent) {
        Write-Host "WinBio stderr: $winBioErr"
        $errContent
    }
}

if($tsharkPath) {
    $frameCount = & $tsharkPath -r $Output 2>$null | Measure-Object | Select-Object -ExpandProperty Count
    Write-Host "tshark frames: $frameCount"
}
