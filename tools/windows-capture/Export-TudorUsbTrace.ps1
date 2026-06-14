param(
    [Parameter(Mandatory = $true)]
    [string]$Pcap,

    [int]$DeviceAddress = 0,

    [string]$Vid = "047d",

    [string]$ProductId = "00f2",

    [string]$Output
)

$ErrorActionPreference = "Stop"

$tshark = Get-Command "tshark.exe" -ErrorAction SilentlyContinue
if($tshark) {
    $tsharkPath = $tshark.Source
} elseif(Test-Path "C:\Program Files\Wireshark\tshark.exe") {
    $tsharkPath = "C:\Program Files\Wireshark\tshark.exe"
} else {
    throw "Could not find tshark.exe"
}

$pcapPath = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Pcap)
if(-not (Test-Path $pcapPath)) {
    throw "Capture does not exist: $pcapPath"
}

if($DeviceAddress -le 0) {
    $descriptorFilter = "usb.idVendor == 0x$Vid && usb.idProduct == 0x$ProductId"
    $addressLines = @(& $tsharkPath -r $pcapPath -Y $descriptorFilter -T fields -e usb.device_address 2>$null)
    if($LASTEXITCODE -ne 0) {
        throw "tshark failed while locating VID_$Vid&PID_$ProductId in $pcapPath"
    }
    $address = $addressLines | Select-Object -First 1
    if(-not $address) {
        throw "Could not find VID_$Vid&PID_$ProductId in $pcapPath"
    }
    $DeviceAddress = [int]$address
}

if(-not $Output) {
    $dir = Split-Path $pcapPath -Parent
    $base = [IO.Path]::GetFileNameWithoutExtension($pcapPath)
    $Output = Join-Path $dir "$base.addr$DeviceAddress.tsv"
}

$outputPath = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Output)
New-Item -ItemType Directory -Force (Split-Path $outputPath -Parent) | Out-Null
$filter = "usb.device_address == $DeviceAddress && (usb.setup.bRequest || usb.data_fragment || usb.control.Response)"

$rows = @(& $tsharkPath -r $pcapPath -Y $filter -T fields `
    -E header=y `
    -E separator="`t" `
    -e frame.number `
    -e frame.time_relative `
    -e usb.endpoint_address `
    -e usb.control_stage `
    -e usb.bmRequestType `
    -e usb.setup.bRequest `
    -e usb.setup.wValue `
    -e usb.setup.wIndex `
    -e usb.setup.wLength `
    -e usb.data_len `
    -e usb.data_fragment `
    -e usb.control.Response)

if($LASTEXITCODE -ne 0) {
    throw "tshark export failed with exit code $LASTEXITCODE"
}

$rows | Set-Content $outputPath

$rowCount = [Math]::Max(0, $rows.Count - 1)
Write-Host "Exported $rowCount rows for USB address $DeviceAddress -> $outputPath"
