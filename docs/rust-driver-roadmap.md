# Native Rust driver roadmap

The current synaTudor stack is a bridge: it relinks the Synaptics Windows DLLs
and lets fprintd use them from Linux. That proved the important parts:

- the USB transport can be driven from Linux
- enrollment and matching work through the v132 Synaptics engine
- templates are tied to the native Synaptics storage format
- fprintd can expose the device cleanly once the host process owns the driver

The next goal is a native Rust driver for this class of hardware.

## Why Rust

Rust is a good fit for the next stage because this driver needs:

- careful binary parsing
- explicit state machines for pairing, TLS/session setup, capture, enroll, and
  verify
- safe ownership around USB transfers and long-running worker tasks
- fuzzable parsers for packet formats learned from captures
- a small C ABI surface for libfprint or host integration

The current C bridge should remain as the reference implementation until the
Rust path can enroll and match without the Windows DLLs.

## Proposed Crates

`tudor-usb`

Owns USB discovery, interface claiming, endpoint zero control transfers, and
interrupt/bulk reads. It should use `nusb` or `rusb` behind a small internal
trait so tests can replay captures.

`tudor-proto`

Defines packet framing, command IDs, status words, parse errors, and typed
request/response structures. This crate should be fuzzed aggressively.

`tudor-session`

Owns pairing, device TLS/session setup, key derivation, and any secure-channel
state learned from the Windows driver and USB traces.

`tudor-engine`

Implements capture, enroll, verify, identify, delete, and storage operations as
async state machines. It should expose a driver-neutral API rather than fprintd
types.

`tudor-storage`

Owns the durable template database format. The first version can interoperate
with the native Synaptics database if we fully document it. If that format stays
opaque, this crate becomes the migration boundary for a Linux-native template
store.

`tudor-host-rs`

Runs the native driver behind the same host-launcher boundary used today. This
lets the fprintd side stay stable while the implementation behind it changes.

`libfprint-tudor`

Thin libfprint TOD integration. This may remain C/GObject code that calls a
Rust static library through C ABI, or become a mostly Rust module if the build
and ABI story is clean enough.

## Migration Phases

1. Stabilize the bridge

   Keep the current C bridge working. Reduce log noise, document supported
   devices, package it, and collect known-good enroll/verify logs.

2. Build replay fixtures

   Turn Windows and Linux USB traces into checked-in redacted fixtures. The
   parser and transport state machine should be testable without hardware.

3. Model the transport

   Implement command framing and status handling in Rust. Replay traces until
   packet-level parse output matches the known Windows and bridge logs.

4. Model secure session setup

   Extract the pairing and TLS/session behavior currently hidden inside the
   Windows DLL. This is the hardest boundary. The native driver is not real
   until this can run without the DLL.

5. Implement read-only flows

   Start with open, status, locate, and identify-like capture flows. Keep them
   behind a separate debug CLI until they are repeatable.

6. Implement enroll and verify

   Add template creation, storage, and match semantics. Compare against the C
   bridge with the same finger and device.

7. Swap the host implementation

   Keep the DBus launcher and libfprint module shape, but launch
   `tudor-host-rs`. This makes rollback simple and preserves fprintd behavior.

8. Retire the Windows DLL path

   Once native enroll, verify, identify, suspend/resume, and delete work across
   the hardware matrix, move the relinked driver behind a debug or legacy
   feature flag.

## Open Questions

- Which Tudor variants share the same command set and secure-channel protocol?
- Can the native Synaptics storage database be documented enough for migration,
  or should Linux use a clean new template store?
- Which pieces are device firmware behavior versus Windows-driver policy?
- How much of WinBio's enroll quality and duplicate detection must be
  replicated?
- Can libfprint accept a mostly Rust TOD module cleanly, or should Rust stay
  behind a C/GObject shim?

## Good First Milestones

- Add a hardware matrix with VID, PID, marketing name, driver DLL version, and
  current status.
- Add capture replay tests for control transfers `0x16`, `0x17`, `0x19`,
  `0x1a`, and `0x1b`.
- Write a typed parser for the visible Tudor packet framing.
- Split debug logging into stable event names that can be compared across C and
  Rust implementations.
- Package the current bridge for Arch/CachyOS so more testers can contribute
  traces and logs.

## Non-Goals For The First Rust Pass

- replacing fprintd
- inventing a new desktop fingerprint UX
- supporting every Synaptics fingerprint reader family
- weakening the host sandbox to make development easier

The bridge driver gave us a working path. The Rust driver should preserve that
behavior while deleting the proprietary Windows-runtime dependency one protocol
layer at a time.
