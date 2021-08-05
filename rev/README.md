# Reverse engineering
This folder contains the reverse engineering part of the project. Here's a documentation of all files

## Driver reference
All driver binarys beeing reverse engineered are sourced from [here](https://download.lenovo.com/pccbbs/mobiles/r17fq02w.exe).

## Files
File | Description
--- | ---
rev.txt | Raw reverse engineering documentation of the Windows driver. You'll find lots of low level details here, but no clear protocol documentation. (Ctrl+F is your friend)
proto.txt |High level protocol description. This will give you a good first overview, but you'll probably need to look up details in `rev.txt`
SensorAdapterIOCTL.txt | IOCTLs which the Windows SensorAdapter uses
EngineAdapterIOCTL.txt | IOCTLs which the Windows EngineAdapter uses
*.gzf | Ghidra exports (see below)

## Importing into Ghidra
Reverse engineering was done in Ghidra. However, a feature is needed which currently isn't merged, so you have to use [this pull request's version](https://github.com/NationalSecurityAgency/ghidra/pull/2189). After that, you should be able to import the `.gzf` files from this directory into a new project.
