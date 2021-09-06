`umockdev` Tests
================
`umockdev` tests use fingerprint devices mocked by [`umockdev`
toolchain][umockdev].

This document describes how to create a 'capture' test: a test that
captures a picture of a fingerprint from the device (mocked by
`umockdev`) and compares it with the standard one.

Other kinds of `umockdev` tests could be created in a similar manner.


'Capture' Test Creation
-----------------------
A new 'capture' test is created by means of `capture.py` script:

1. Create (if needed) a directory for the driver under `tests`
   directory:

   `mkdir DRIVER`

2. Prepare your execution environment.

   In the next step a working and up to date libfprint is needed. This can be
   achieved by installing it into your system. Alternatively, you can set
   the following environment variables to run a local build:
   - `export LD_PRELOAD=<meson-build-dir>/libfprint/libfprint-2.so`
   - `export GI_TYPELIB_PATH=<meson-build-dir>/libfprint`

   Also, sometimes the driver must be adapted to the emulated environment
   (mainly if it uses random numbers, see `synaptics.c` for an example).
   Set the following environment variable to enable this adaptation:
   - `export FP_DEVICE_EMULATION=1`

   Run the next steps in the same terminal.

3. Find the real USB fingerprint device with `lsusb`, e.g.:

   `Bus 001 Device 005: ID 138a:0090 Validity Sensors, Inc. VFS7500 Touch Fingerprint Sensor`

   The following USB device is used in the example above:
   `/dev/bus/usb/001/005`.

   For the following commands, it is assumed that the user that's
   running the commands has full access to the device node, whether
   by running the commands as `root`, or changing the permissions for
   that device node.

4. Record information about this device:

   `umockdev-record /dev/bus/usb/001/005 > DRIVER/device`

5. Record interaction of `capture.py` (or other test) with the device:

   `umockdev-record -i /dev/bus/usb/001/005=DRIVER/capture.ioctl -- python3 ./capture.py DRIVER/capture.png`

   Files `capture.ioctl` and `capture.png` will be created as the
   result of this command.

6. Add driver's name to `drivers_tests` in the `meson.build`.
7. Check whether everything works as expected.

**Note.** To avoid submitting a real fingerprint, the side of finger,
arm, or anything else producing an image with the device can be used.


Possible Issues
---------------
`umockdev-record` aggressively groups URBs. In most cases, manual
intervention is unfortunately required. Often, drivers do a chain of
commands like: A then B each with a different reply. However,
`umockdev-record` could create a file like this:

    A
     reply 1
     reply 2
    B
     reply 1
     reply 2

In that case, records must be re-ordered:

    A
     reply 1
    B
     reply 1
    A
     reply 2
    B
     reply 2

Other changes may be needed to get everything working. For example the
`elan` driver relies on a timeout that is not reported correctly. In
this case the driver works around it by interpreting the protocol
error differently in the virtual environment (by means of
`FP_DEVICE_EMULATION` environment variable).


[umockdev]: https://github.com/martinpitt/umockdev
