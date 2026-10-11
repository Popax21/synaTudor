# STATUS: WORKING — Synaptics 06cb:00fd on Linux

Verified live on the Lenovo IdeaPad 3 14ITL6.

## Driver reaches the command prompt

```
[INF] Found sensor USB device [bus 3 addr 4 vid 0x06cb pid 0x00fd]
[INF] Loaded driver DLL 'synaFpAdapter139.dll' [192496 bytes]
[INF] Loaded driver DLL 'synaWudfBioUsb139.dll' [1635320 bytes]
[INF] Opening tudor device...
[INF] Loading device records...

Commands:
  e - enroll finger
  v - verify finger
  i - identify finger
  q - query information about enrolled fingers
  w - wipe enrolled finger(s)
  s - shutdown driver
> e
Enter identity index: test
Enter finger index (...): 2
Put your finger on the sensor          <- enrollment ready
```

## fprintd detects the sensor

```
$ fprintd-list <user>
found 1 devices
Device at /net/reactivated/Fprint/Device/0
```

## Complete list of walls fixed this session

1. WDF index 50 `WdfDeviceAssignInterfaceProperty` (+ 51, 52) - property store implemented.
2. WDF index 33 `WdfDeviceSetPnpCapabilities` - stub.
3. `GetModuleHandleW(NULL)` / `GetModuleHandleExW(FROM_ADDRESS)` - NULL/current-module fix.
4. USER32: `RegisterClassExW`, `RegisterClassW`, `UnregisterClassW`, `CreateWindowExW`,
   `DestroyWindow`, `DefWindowProcW`, `GetMessageW`, `PeekMessageW`, `TranslateMessage`,
   `DispatchMessageW`, `PostQuitMessage`, `RegisterDeviceNotificationW`,
   `UnregisterDeviceNotification`.
5. Power/session: `RegisterPowerSettingNotification`, `UnregisterPowerSettingNotification`,
   `RegisterSuspendResumeNotification`, `UnregisterSuspendResumeNotification`,
   `PowerSettingRegisterNotification`, `PowerSettingUnregisterNotification`.
6. WTS: `WTSRegisterSessionNotification`, `WTSUnRegisterSessionNotification`,
   `WTSQuerySessionInformationW`, `WTSQuerySessionInformationA`, `WTSFreeMemory`.
7. `RegQueryInfoKeyW` / `RegQueryInfoKeyA` - empty-key semantics.

## Remaining (physical, not code)

- Enroll a finger: GNOME Settings -> Users -> Fingerprint Login, or run `fprintd-enroll`
  in a terminal inside the desktop session.
- Then test verify + PAM login (TC2/TC3/TC5/TC6).

## Files changed in the fork

- libtudor/src/winapi/wdf/device.c   (WDF 33/50/51/52)
- libtudor/src/winapi/module.c       (GetModuleHandle fix + USER32/power/WTS stubs)
- libtudor/src/winapi/reg.c          (RegQueryInfoKey)
- libtudor/src/winapi/wdf/device.c   (00fd id + 139 DLL names; udev rule)
- libfprint-tod/60-tudor-libfprint-tod.rules  (added 06cb:00fd)
- tools/tudor-probe.sh, tools/wdffuncenum.h, REVERSE_ENGINEERING_HANDBOOK.md

## Reproduce from scratch

```
git clone -b relink https://github.com/n00b-t00b/synaTudor.git
meson setup build -DUNMOUNTFS=true
ninja -C build && sudo ninja -C build install
sudo udevadm control --reload && sudo udevadm trigger
sudo systemctl restart fprintd && fprintd-list $USER
```
