# Synaptics Tudor Driver Relinking Project
This project attempts to dynamically relink the driver of the Synaptics Tudor
family of fingerprint sensors at run time, allowing them to run and provide
their functionality on Linux-based x86-64 systems. It split off from the reverse
engineering branch after it hit multiple dead ends, and because it showed the
potential of quickly allowing for the creation of *something* which can at least
allow users to use the sensors installed in their hardware.

THIS PROJECT IS PROVIDED AS IS, WITHOUT WARRANTY OR LIABILITY OF ANY KIND, OR
FOR ANY RISKS OR SIDE EFFECTS WHICH MIGHT OCCUR FROM USAGE OF ANYTHING PROVIDED
AS PART OF THIS PROJECT, INCLUDING, BUT NOT LIMITED TO, BRICKED SENSORS,
CORRUPTED FIRMWARE, BYPASSES OF HOST SECURITY, AND VULNERABILITIES IN THE CODE.
USE AT YOUR OWN RISK.

**NOTE: The project should be fully functional right now, contrary to its earlier state. If there are any issues, please report them.**

## Structure
This project is split over multiple folders, all providing different parts of
the functionality:
- [libtudor](libtudor/README.md): Contains the common library code handling
  relinking and interfacing with the driver.
- [cli](cli/README.md): Contains a simple CLI wrapper for the relinked driver.
- [tudor-host](tudor-host/README.md): Contains the host application for
  libtudor, used by the libfprint module.
- [tudor-host-launcher](tudor-host-launcher/README.md): Contains the systemd
  service which launches the tudor host processes. This extra step is needed to
  bypass the strict fprintd sandboxing, which messes with the host's even
  stricter sandboxing.
- [libfprint-tod](libfprint-tod/README.md): Contains the libfprint module, to be
  loaded by the libfprint TOD fork.

## Building / Installation
The same build system used by libfprint, meson, is used for this project.
During the first build, the Windows driver is automatically downloaded and
extracted. `innoextract` has to be installed for this.
To build and install all contained parts, execute:
```sh
meson build
cd build
ninja
sudo ninja install
```
(for Arch Linux specifically, you might want to use `arch-meson` instead of `meson`)

For documentation about build options etc., see the individual parts.

For the libfprint module to be picked up and work, you'll need to have a
`libfprint-tod` fork of libfprint installed. Most Linux distributions have a
seperate package which you can install instead of the regular libfprint one
(e.g. Arch Linux: AUR `libfprint-tod-git`).

~~**NOTE:** Currently libfprint-tod has a bug which can cause fprintd to lock up.
It's recommended to use [this
fork](https://gitlab.freedesktop.org/Popax21/libfprint/-/tree/tod) for now, at least
until it's merged into the base repository.~~
The regular upstream libfprint-tod repository should work now
