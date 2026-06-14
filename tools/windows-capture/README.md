# Windows USB capture notes

This project needs Windows-side USB traces for the Kensington VeriMark DT /
Synaptics Tudor path:

- USB device: `VID_047D&PID_00F2`
- Biometric interface: `USB\VID_047D&PID_00F2&MI_01`
- Windows driver seen here: Synaptics `6.0.9.1132` via `oem84.inf`
- USBPcap interface seen here: `\\.\USBPcap1`

Use the helper from an elevated PowerShell when possible:

```powershell
pwsh -ExecutionPolicy Bypass -File tools/windows-capture/Start-TudorUsbCapture.ps1 -Mode Identify -Seconds 30
```

Touch the reader while the capture is running. If `Identify` does not produce
traffic, use a manual Windows Hello flow instead:

```powershell
pwsh -ExecutionPolicy Bypass -File tools/windows-capture/Start-TudorUsbCapture.ps1 -Mode Manual -Seconds 90
start ms-settings:signinoptions
```

For reverse engineering, keep these capture shapes separate:

- `open`: `WinBioEnumBiometricUnits` + `WinBioOpenSession`
- `locate`: `WinBioLocateSensor`, touch the sensor during the window
- `identify`: `WinBioIdentify`, touch the sensor during the window
- `manual`: Windows Settings / Hello enrollment flow

The helper defaults to capturing the full USB root hub because the sensor can
reset or re-enumerate during real flows. Use the exporter to discover the live
USBPcap address, then pass that value with `-DeviceAddress` for a narrow
capture. Do not trust the Windows PnP address for capture filtering.

Export the relevant control-transfer sequence from a root-hub capture with:

```powershell
pwsh -ExecutionPolicy Bypass -File tools/windows-capture/Export-TudorUsbTrace.ps1 -Pcap captures/identify-touch-root.pcapng
```

The first useful capture showed the live reader at USB address `30`, even
though Windows PnP reported address `4`. The captured transport is vendor
control traffic over endpoint zero:

- OUT: `bmRequestType=0x40`, `bRequest=0x16`, `wValue=0x0002` or `0x0006`
- IN: `bmRequestType=0xc0`, `bRequest=0x17`, `wValue=0x0000`
- Small sideband: OUT `0x19`, OUT `0x1b`, IN `0x1a` returning `01`
- Payloads begin with TLS record bytes such as `17 03 03`, so the Tudor command
  stream is wrapped inside the device TLS session.

`Locate` and `Identify` workers are stopped at timeout so a blocked WinBio call
does not strand the capture. If Windows reports the biometric unit as busy, wait
a few seconds before starting the next run.
