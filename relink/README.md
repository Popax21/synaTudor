# CLI Wrapper
This folder contains a simple CLI wrapper for the functionality of the relinked
driver. It can be used to enroll, verify and identify captured samples of fingerprints.

## Building
First, download and extract the driver DLLs by executing the command `make
extract` (you'll need a working internet connection). This requires
`innoextract` to be installed (https://pkgs.org/download/innoextract). Then,
compile the program by running `make`. 

## Usage
Start the wrapper using `sudo out/tudor <path to data store file> <flags>`. The
data store file is a file where the drivers will store data like pairing data
and enrollment records, and has to be in a directory accessible by your own user
(the wrapper drops privileges before opening the file for security reasons).
Currently, only the following flags are defined:

Flag | Description
----- | ---------------------------
`-v` | Increase the verbosity of the log output
`-t` | Enable display of driver debug trace messages

Once the program is running, after some time, a command prompt should appear.
All available commands are displayed there.

## Documentation
**TODO**