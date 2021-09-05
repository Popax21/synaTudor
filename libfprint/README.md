# libfprint patch
This folder contains a patched version of libfprint, which can interact with the tudor driver contained in this repository.

## Installation
1. Install fprintd and libfprint like regular
2. Make sure you installed the [Python prototype driver package](../pydrv) 
2. Uninstall libfprint, **WITHOUT REMOVING IT'S DEPENDENT PACKAGES** (e.g. on Arch Linux: `sudo pacman -Rdd libfprint`)
3. Go into the `libfprint` folder contained in this folder
4. Run `meson build` (NOTE: on Arch Linux, run `arch-meson build`)
5. `cd` into the newly created `build` directory
6. Run `sudo meson install`

## Paring
After you installed the libfprint patch, you still have to setup pairing data so that `fprintd` can find it. You can do this by executing the `fprintd_setup` command in the prototype shell, while the driver is initialized and pairing data is loaded.