# CLI Wrapper
This folder contains a simple CLI wrapper for the functionality of the relinked
driver. It can be used to enroll, verify and identify captured samples of fingerprints.

**WARNING:** Even though the CLI employs sandboxing, its security is in no way
comparable to the one found in the libfprint integration. A malicious driver
could take over your local user account! This CLI is only intended to be used
for debugging and/or small scale tests.

## Usage
Start the wrapper using `sudo ../out/tudor_cli <path to data store file> <flags>`. The
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