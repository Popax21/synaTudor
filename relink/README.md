# CLI Wrapper
This folder contains a simple CLI wrapper for the functionality of the relinked
driver. It can be used to enroll, verify and identify captured samples of fingerprints.

## Building
First, download and extract the driver DLLs by executing the command `make
extract` (you'll need a working internet connection). This requires
`innoextract` to be installed (https://pkgs.org/download/innoextract). Then,
compile the program by running `make`. 

Currently, the following build flags are defined:

Flag | Description
----- | ---------------------------
`DBGIMPORT=1` | Enable import debugging, which prints the name of unresolved function imports when they're called
`DBGWDF=1` | Enable WDF debugging, which prints the index of unresolved WDF functions when they're called

## Usage
Start the wrapper using `sudo out/tudor <path to data store file> <flags>`. The
data store file is a file where the drivers will store data like pairing data
and enrollment records, and has to be in a directory accessible by your own user
(the wrapper drops privileges before opening the file for security reasons).

Currently, the following flags are defined:

Flag | Description
----- | ---------------------------
`-v` | Increase the verbosity of the log output
`-q` | Decrease the verbosity of the log output
`-t` | Enable display of driver debug trace messages
`-V <vid>` | Set the sensor USB VID (default: 0x06cb)
`-P <pid>` | Set the sensor USB PID (default: 0x00be)

Once the program is running, after some time, a command prompt should appear.
All available commands are displayed there.

## Documentation
**TODO**