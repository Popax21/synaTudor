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

## Structure
This project is split over multiple folders, all providing different parts of
the functionality:
- [relink](relink/README.md): Currently the only part. Contains a simple CLI
  wrapper for the relinked drivers