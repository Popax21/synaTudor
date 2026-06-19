# Native Rust driver spec

The current synaTudor stack is a working bridge: Linux talks to fprintd, fprintd
loads a libfprint TOD module, the TOD module launches a sandboxed host, and the
host relinks the Synaptics Windows DLLs that know how to pair with the sensor,
capture images, create templates, and match fingers.

The Rust build is the path from that bridge to a native driver for this class
of Synaptics Tudor hardware. The bridge remains the reference implementation
until Rust can enroll and match without loading vendor DLLs.

## Objective

Build a native Rust implementation that can replace the relinked Windows driver
behind the existing fprintd integration.

The first complete Rust driver is successful when it can:

- probe and open the supported Tudor-family USB readers
- establish the same secure session and pairing state as the Windows driver
- capture samples, enroll templates, verify a requested GUID/finger, and
  identify with the same TOD/fprintd filtering behavior as the bridge
- delete individual records and clear the persistent template store
- survive cancel, timeout, restart, suspend/resume, sudo, and polkit flows
- run under the current host-launcher sandbox without weakening it
- use the existing fd-passed USB and IPC process boundary
- keep fprintd-visible behavior compatible with the current TOD module
- run without loading `synaFpAdapter*.dll` or `synaWudfBioUsb*.dll`

The near-term implementation target is a native host process that speaks the
same IPC protocol as `tudor-host`. Replacing the libfprint TOD module can come
later, after the driver behavior is stable.

## Current Reference Behavior

The current bridge proves the hardware path and defines the compatibility
contract Rust must preserve.

Reference stack:

- `libfprint-tod` talks to `tudor-host-launcher` over DBus.
- The launcher starts one sandboxed `tudor-host` for the active USB device.
- `tudor-host` loads `libtudor`, relinks the Windows DLLs, and owns the sensor.
- The host and TOD module communicate over the IPC messages in
  `tudor-host/inc/tudor/libfprint-proto.h`.
- v132 devices use the Synaptics native storage adapter. fprintd records carry
  GUID/finger metadata only, while template ownership and matching live behind
  the native storage/session protocol rather than in libfprint print payloads.
- The sandboxed host receives the USB device fd through `SCM_RIGHTS` on
  `IPC_MSG_INIT`, disables libusb device discovery, and wraps that inherited fd.
  It does not enumerate or open by path after the sandbox is active.
- The host sandbox forbids normal filesystem access: `open*` returns `EPERM`,
  file size is limited to zero, and the root namespace may be unmounted.
  Persistent state must therefore be brokered over IPC or provided as an already
  open fd before the sandbox clamps down.
- On the working v132 path, template storage and matching are sensor/native
  storage behavior. The host orchestrates the secure session and commands; it
  does not receive normal template payloads for native-storage records.

Known working receipt:

- Kensington VeriMark Desktop, `047d:00f2`
- Synaptics v132 Windows driver DLLs from driver `6.0.9.1132`
- enroll, verify, identify, sudo PAM, and polkit PAM can work through fprintd
- host stability depends on bounded Windows-style async callback stacks

## Hardware Scope

Initial profile: `tudor-v132`.

| VID:PID | Status | Notes |
| --- | --- | --- |
| `047d:00f2` | Working through bridge | Kensington VeriMark Desktop |
| `06cb:00be` | Advertised by TOD module | Needs confirmation captures |
| `06cb:00de` | Advertised by TOD module | Needs confirmation captures |

The driver must keep the hardware matrix explicit. New USB IDs need:

- marketing name
- firmware/driver version observed on Windows
- USB descriptor dump
- Windows capture for open, locate or identify, enroll, verify, and delete
- Linux bridge log for the same operation when the bridge works
- Rust replay fixture coverage before claiming support

## Non-Goals

The first Rust pass must not:

- replace fprintd
- invent a new desktop fingerprint UX
- support every Synaptics fingerprint reader family
- depend on a privileged always-on daemon
- weaken the current systemd, seccomp, or process-boundary model
- silently migrate or erase template databases it cannot fully understand
- log biometric samples, template bodies, pairing secrets, or session keys

## Hard Invariants

These are compatibility and safety rules, not preferences.

1. Keep the current C bridge as the reference until native Rust is feature
   complete for enroll, verify, identify, delete, and suspend/resume.
2. Keep the fprintd metadata model stable. A libfprint print may store only the
   template GUID and finger; native storage owns the biometric template body.
3. Preserve `IPC_MSG_CLEAR_RECORDS` versus `IPC_MSG_CLEAR_HOST_RECORDS`.
   `CLEAR_RECORDS` clears persistent enrolled templates. `CLEAR_HOST_RECORDS`
   clears transient host-side records only and must not wipe native storage.
4. Every parser is total: malformed captures return typed errors, never panics.
5. Every protocol discovery lands in a replay fixture before it becomes driver
   behavior.
6. The Rust host must run in the launcher sandbox with root-owned state and no
   broader permissions than the current host requires.
7. Matching failures must stay distinguishable from bad captures, cancelled
   operations, device errors, and storage corruption.
8. The production Rust USB backend wraps the inherited USB fd from
   `IPC_MSG_INIT`. Discovery and device selection stay outside the sandbox.
9. For v132, the architecture is sensor-side storage and sensor-side matching
   until captures prove otherwise. A host-side template database is an optional
   branch for non-native-storage devices, not the default path.
10. Host errors that cannot be reported in-band must close the session or exit
    so fprintd and the launcher can recover as they do with the C host.

## Architecture Overview

The Rust driver should be a workspace with small crates and narrow boundaries.

Suggested layout:

```text
rust/
  Cargo.toml
  crates/
    tudor-usb/
    tudor-trace/
    tudor-proto/
    tudor-session/
    tudor-storage/
    tudor-engine/
    tudor-host-rs/
    tudor-cli/
    libfprint-tudor/
```

The workspace can be added before the C bridge is removed. The first Rust
binary should be a debug CLI and replay harness, then `tudor-host-rs`.

## Crate Specs

### `tudor-usb`

Own the USB transport once a device has already been selected and handed to the
host.

Responsibilities:

- wrap the USB device fd received with `IPC_MSG_INIT`
- disable runtime device discovery in the production host backend
- expose descriptor details for logging and validation
- claim the correct interface without assuming a single fixed bus address
- expose vendor control transfers:
  - OUT `bmRequestType=0x40`, `bRequest=0x16`
  - IN `bmRequestType=0xc0`, `bRequest=0x17`
  - sideband OUT `0x19`, IN `0x1a`, OUT `0x1b`
- expose interrupt or bulk pipe reads for sensor event/status traffic
- support cancellation and bounded operation timeouts
- hide the concrete USB implementation behind an internal transport trait
- provide a fake transport that replays checked-in capture fixtures

Tooling outside the production host may still discover devices by VID/PID. The
sandboxed host must not.

Important observed behavior:

- v132 traffic is primarily vendor control traffic over endpoint zero.
- Payloads include TLS-looking records such as `17 03 03`.
- Bridge logs have observed small pipe reads, including endpoint `0x83`
  seven-byte reads, during verify/enroll flows.
- Older WDF paths may need bulk treatment for endpoints declared as interrupt.

Implementation constraint:

- libusb is the proven backend because the current seccomp profile allows the
  syscalls it uses, including `poll`, `ppoll`, `eventfd`, and `timerfd`.
- `nusb`, `rusb`, or any async runtime-backed USB library must be audited
  against the real seccomp allowlist before adoption. In particular, `epoll`,
  `io_uring`, file opens, sockets, and non-anonymous `mmap` are not available
  in the current host sandbox.

### `tudor-trace`

Own capture normalization and replay fixtures.

Responsibilities:

- parse exported Windows USBPcap traces and Linux bridge logs into one fixture
  format
- represent control OUT, control IN, pipe IN, pipe OUT, timing, status, and
  reset/re-enumeration events
- redact or omit biometric sample payloads when they are not required for parser
  tests
- preserve enough byte-level data to reproduce protocol framing bugs
- provide deterministic replay for unit and integration tests

Fixture format requirements:

- stable schema version
- device profile and USB ID
- capture source, operation kind, and driver version
- ordered operations with raw bytes encoded losslessly
- explicit redaction markers instead of silently shortened payloads
- expected high-level events when known

### `tudor-proto`

Own binary protocol types and parsers.

Responsibilities:

- define typed USB request descriptors and visible Tudor packet frames
- parse command IDs, status words, record lengths, and response envelopes
- represent TLS records separately from decrypted Tudor messages
- expose structured errors with offsets and operation context
- support fuzzing for every parser that touches capture bytes
- keep all serialization/deserialization independent from USB and fprintd

This crate should not own pairing, template matching, storage policy, or fprintd
semantics.

### `tudor-session`

Own device pairing and secure-channel state.

Responsibilities:

- model device states: unpaired, pairing, paired, opening, active, suspended,
  failed, and closed
- load and store pairing data by sensor identity
- drive the secure handshake learned from Windows captures and bridge traces
- perform key derivation and record protection only through audited crypto
  crates or narrowly wrapped primitives
- never log keys, nonces that are security-sensitive, or decrypted template data
- expose a transcript mode for tests that compares Rust traffic to reference
  traces
- avoid filesystem-backed certificate, root-store, or entropy assumptions inside
  the host sandbox

Current bridge equivalents:

- `tudor_get_pdata_fnc` and `tudor_set_pdata_fnc` provide pair-data callbacks.
- `IPC_MSG_LOAD_PDATA`, `IPC_MSG_RESP_LOAD_PDATA`, and
  `IPC_MSG_STORE_PDATA` are the host/TOD storage boundary.
- The Windows driver owns the TLS/session setup today.

Open work:

- identify which bytes are standard TLS records versus Tudor messages carried
  inside TLS
- identify what pair data is device-specific, host-specific, or user-specific
- document how device reset and re-enumeration affect pairing state
- decide whether an existing TLS record implementation can be used without
  system-root or socket assumptions, or whether the driver needs a narrow
  record-layer adapter around custom Tudor crypto

### `tudor-storage`

Own the persistent-state boundary. For v132, this is mostly metadata, pairing
data, and command semantics because the device/native adapter owns enrolled
templates and matching.

Responsibilities:

- model records by GUID and finger/subfactor
- distinguish fprintd-visible metadata from sensor-owned template data
- implement delete one, delete by finger, delete by GUID, and clear all
- distinguish persistent storage clear from transient host-record clear
- define how state is brokered through IPC or pre-opened fds under the sandbox
- provide a migration boundary for data created by the bridge, without touching
  opaque databases in place
- support an optional host-side template store only if a future device profile
  proves matching is host-side

Current v132 storage policy:

- Keep fprintd print data metadata-only: GUID plus finger.
- Treat sensor/native storage as the source of truth for templates.
- Accept `IPC_MSG_ADD_RECORD` metadata so fprintd can present candidate records.
- Delete and clear by issuing native storage commands over the secure session,
  not by editing a host-side template database.
- Do not attempt in-place conversion of `native-storage.dat` until its format
  and ownership model are documented and covered by tests.
- Do not add a normal file-backed Rust database inside `tudor-host-rs`; the
  current sandbox forbids it.

If a host-side store becomes necessary for a non-v132 or non-native-storage
profile, it needs a separate design decision first:

- where the store lives
- whether the launcher brokers reads/writes over IPC
- whether the host receives a pre-opened database fd before sandbox activation
- whether the store is encrypted or integrity-protected
- how fprintd metadata maps to host-side template rows
- how bridge and Rust stores avoid corrupting each other
- how re-enrollment, migration, and rollback work

Delete and clear scope semantics must be byte-for-byte boring:

| GUID | Finger | Meaning |
| --- | --- | --- |
| specific | specific | delete one record |
| specific | `TUDOR_FINGER_ANY` | delete all records for the GUID |
| null | specific | delete all records for the finger |
| null | `TUDOR_FINGER_ANY` | erase the persistent database |

`IPC_MSG_CLEAR_HOST_RECORDS` must never map to the last row for native-storage
devices.

### `tudor-engine`

Own the driver behavior that higher layers call.

Responsibilities:

- expose open, probe, close, suspend, resume, cancel, enroll, verify, identify,
  delete, and clear operations
- implement each long-running operation as an explicit async state machine
- map capture quality failures to retry results
- map no-match results separately from hard device errors
- keep storage and session state coherent across cancellation
- emit stable structured events for logs and replay comparison
- for v132, drive on-sensor/native-storage enroll, verify, identify, delete, and
  clear commands through the secure session

Current bridge operation shape:

- open builds a WinBio pipeline, resets or prepares the sensor, activates
  sensor/engine/storage, opens storage, refreshes cache, and queries status
- enroll clears contexts, creates enrollment, loops capture/update until enough
  data is collected, checks duplicate, then commits by GUID/finger
- verify captures one sample, pushes it into the engine, and matches against a
  requested GUID/finger
- identify captures one sample, asks the native store for a match, and lets the
  TOD/fprintd layer filter the result against its candidate records
- cancel acknowledges the caller after the async operation is cleaned up

The Rust engine should model those stages directly even though it will not use
WinBio internally.

A host-side matcher is not part of the default v132 design. It becomes a
separate engine profile only if captures show a supported device class that
returns host-owned templates and requires host-side comparison.

### `tudor-host-rs`

Own the first production integration point.

Responsibilities:

- speak the existing Unix-socket IPC protocol from
  `tudor-host/inc/tudor/libfprint-proto.h`
- accept launch by bus/address from `tudor-host-launcher`
- receive the USB device fd from `IPC_MSG_INIT` ancillary data
- return the same fprintd-visible retry, done, match, and no-match semantics
- use the same state directory conventions
- keep log names stable enough to compare C bridge and Rust behavior
- fail closed when storage, pairing, or protocol state is inconsistent
- run under the current seccomp and namespace sandbox

Sandbox requirements:

- no normal filesystem opens after sandbox activation
- no sockets beyond the inherited IPC socket
- no `epoll`, `io_uring`, or runtime reactor that depends on unavailable
  syscalls
- no non-anonymous memory maps
- no lazy runtime initialization after the sandbox that expects file access
- USB polling must use a syscall footprint compatible with the current libusb
  host model
- sandbox behavior should be reused through a shared C/FFI boundary or
  reimplemented only with a dedicated security review

Error contract:

- protocol violations, impossible states, and unrecoverable storage/session
  failures may be represented as typed Rust errors internally
- if the error cannot be reported through the current IPC response shape, the
  host must close the socket or exit so fprintd and the launcher observe the
  same recoverable failure mode as the C host

IPC compatibility requirements:

The IPC socket is `AF_UNIX` plus `SOCK_DGRAM`. Each message is one datagram.
The receiver must preserve the current peek/read discipline so a too-small
buffer never truncates and discards part of a datagram. Rust layout tests must
validate actual wire bytes, including C enum width, struct padding, flexible
array offsets, and the maximum message sizes.

Only `IPC_MSG_INIT` is expected to carry the USB fd. Pairing-data messages are
host-initiated and synchronous during open.

| Message | Direction | Rust host behavior |
| --- | --- | --- |
| `IPC_MSG_INIT` | TOD to host | initialize logging, receive USB fd, open device |
| `IPC_MSG_READY` | host to TOD | report open complete |
| `IPC_MSG_ACK` | either | acknowledge commands with no structured response |
| `IPC_MSG_PROBE` | TOD to host | return stable sensor name |
| `IPC_MSG_LOAD_PDATA` | host to TOD | request pairing data for sensor identity |
| `IPC_MSG_RESP_LOAD_PDATA` | TOD to host | return optional pairing data |
| `IPC_MSG_STORE_PDATA` | host to TOD | request atomic pairing data persistence |
| `IPC_MSG_ADD_RECORD` | TOD to host | accept GUID/finger metadata and optional legacy payload |
| `IPC_MSG_DEL_RECORD` | TOD to host | delete persistent template for GUID/finger |
| `IPC_MSG_CLEAR_RECORDS` | TOD to host | clear persistent templates |
| `IPC_MSG_CLEAR_HOST_RECORDS` | TOD to host | clear transient host records only |
| `IPC_MSG_ENROLL` | TOD to host | start enrollment and report retry/done states |
| `IPC_MSG_VERIFY` | TOD to host | verify requested GUID/finger and report match/no-match |
| `IPC_MSG_IDENTIFY` | TOD to host | ask sensor for an identify result |
| `IPC_MSG_CANCEL` | TOD to host | cancel the active operation and ACK after cleanup |
| `IPC_MSG_SHUTDOWN` | TOD to host | close device and exit cleanly |

Native-storage identify caveat:

The current IPC message for identify does not carry an explicit candidate set.
For native-storage devices, the sensor can return a matched GUID from its whole
database. Candidate filtering happens in the TOD/fprintd layer after the host
responds. If Rust moves filtering into the host, that is a deliberate behavior
change and needs separate tests, especially for multi-user systems.

### `libfprint-tudor`

Keep this thin at first.

The first Rust milestone should keep the existing C TOD module and replace only
the host implementation. A mostly Rust libfprint module is a later cleanup once
the native protocol is proven.

Longer-term options:

1. Keep a C/GObject TOD module that calls Rust through a small C ABI.
2. Build a mostly Rust TOD module if the libfprint TOD ABI and distro packaging
   are clean enough.

This layer must preserve fprintd behavior. It should not learn protocol details
that belong in `tudor-engine`.

### `tudor-cli`

Own developer bring-up and diagnostics.

Useful commands:

- `list`
- `probe --vid-pid <id>`
- `replay <fixture>`
- `open --bus <n> --addr <n>`
- `pair`
- `status`
- `capture`
- `enroll --finger <finger>`
- `verify --guid <guid> --finger <finger>`
- `identify`
- `delete --guid <guid> --finger <finger>`
- `clear`

The CLI may expose dangerous debug operations, but production fprintd behavior
must not depend on those debug paths.

## Runtime State Machines

### Device Lifecycle

```text
discovered
  -> claimed
  -> pair-data-loaded
  -> session-opening
  -> active
  -> suspended
  -> active
  -> closing
  -> closed
```

Any hard protocol, USB, or storage failure moves to `failed`. Recovery from
`failed` requires closing and reopening the device.

### Enrollment

```text
idle
  -> enrollment-created
  -> capture-started
  -> capture-finished
  -> sample-accepted
  -> needs-more-samples OR ready-to-commit
  -> duplicate-check
  -> committed
  -> idle
```

Bad capture returns `retry=true, done=false`. A successful partial sample
returns `retry=false, done=false`. A committed enrollment returns
`retry=false, done=true`.

### Verification

```text
idle
  -> requested-guid-loaded
  -> capture-started
  -> capture-finished
  -> sample-accepted
  -> matched OR no-match OR retry
  -> idle
```

No match is a successful operation with `did_match=false`. Bad capture is a
retry result. Device/protocol/storage failures are errors.

### Identification

```text
idle
  -> sensor-identify-requested
  -> capture-started
  -> capture-finished
  -> sample-accepted
  -> matched-guid OR no-match OR retry
  -> idle
```

Identify must only return GUID/finger pairs that were supplied by fprintd as
candidate records at the fprintd/TOD boundary. For native-storage devices, the
host may receive a broader sensor result and the TOD layer must filter it before
libfprint treats it as an accepted identity.

### Cancellation

```text
action-running
  -> cancel-requested
  -> transport-cancelled
  -> operation-cleaned
  -> cancel-acked
  -> idle
```

Cancel ACK must mean Rust has stopped touching the operation state that fprintd
asked it to cancel.

## Logging And Observability

The Rust path needs structured events that can be compared with bridge logs and
fixtures.

Suggested event names:

- `device.open.start`
- `device.open.ready`
- `device.open.failed`
- `usb.ctrl.out`
- `usb.ctrl.in`
- `usb.pipe.in`
- `session.pair.load`
- `session.pair.store`
- `session.handshake.start`
- `session.handshake.done`
- `storage.record.add`
- `storage.record.delete`
- `storage.clear.persistent`
- `storage.clear.transient`
- `enroll.capture.retry`
- `enroll.capture.accepted`
- `enroll.commit.done`
- `verify.match`
- `verify.no_match`
- `verify.retry`
- `identify.match`
- `identify.no_match`
- `operation.cancelled`

Logs may include GUID prefixes, finger IDs, USB request metadata, and protocol
status codes. Logs must not include raw biometric images, template payloads,
pairing secrets, private keys, or decrypted session traffic by default.

## Verification Plan

The Rust build is not real until it has repeatable checks at each layer.

Required automated checks:

- `cargo fmt --check`
- `cargo clippy --workspace --all-targets`
- `cargo test --workspace`
- parser fuzz targets for `tudor-proto`
- fixture replay tests for each supported operation shape
- persistent-state broker and storage corruption tests
- IPC compatibility tests against captured wire bytes
- IPC datagram tests for peek/read sizing and `SCM_RIGHTS` fd handling
- sandbox smoke tests for the production host syscall footprint
- wildcard delete tests for every GUID/finger scope combination
- regression tests proving orphan cleanup cannot clear persistent templates

Required fixture coverage:

- Windows open
- Windows locate or identify with touch
- Windows enroll
- Windows verify match
- Windows verify no-match, when available
- Linux bridge open
- Linux bridge enroll
- Linux bridge verify match
- Linux bridge delete and clear

Required hardware-in-loop checks before enabling the Rust host by default:

```sh
fprintd-list "$USER"
sudo fprintd-delete "$USER"
sudo fprintd-enroll "$USER"
fprintd-verify "$USER"
sudo -v
```

Additional manual checks:

- run `tudor-host-rs` under the unmodified host sandbox
- polkit authentication
- delete one finger, then verify that old GUID no longer matches
- clear all records, then verify no stale fprintd metadata can match
- native-storage identify on a multi-user or synthetic multi-GUID database
- restart `tudor-host-launcher` and `fprintd`
- unplug/replug
- suspend/resume
- repeated cancel during enroll and verify

## Migration Phases

### Phase 0: Freeze The Bridge Reference

Status: in progress.

Deliverables:

- stable bridge docs for build, install, fprintd, sudo, and polkit
- hardware matrix
- known-good logs for open, enroll, verify, identify, delete, and clear
- stable event names for USB and biometric operations
- clear distinction between persistent and transient storage cleanup

Verification:

- current C bridge can enroll and verify on the tested reader
- sudo and polkit can authenticate after plain fprintd verification works

### Phase 1: Rust Workspace And Fixture Corpus

Deliverables:

- Rust workspace skeleton under `rust/`
- capture fixture schema in `tudor-trace`
- importer for Windows USBPcap exports
- importer for bridge control-transfer logs
- redaction rules documented
- IPC wire fixture for `SOCK_DGRAM` messages and the INIT fd transfer
- tiny sandbox probe binary that proves the Rust runtime choices survive the
  current seccomp profile

Verification:

- fixtures load and round-trip without byte loss outside redacted fields
- replay tests can assert ordered USB operations for known captures
- sandbox probe runs under the existing host launcher profile without new
  syscalls

### Phase 2: Transport And Visible Framing

Deliverables:

- `tudor-usb` transport trait and inherited-fd libusb backend
- fake replay backend
- typed vendor control operation model
- first `tudor-proto` parsers for visible framing and TLS record envelopes

Verification:

- replay output matches reference request order for `0x16`, `0x17`, `0x19`,
  `0x1a`, and `0x1b`
- parser fuzz target runs without panics
- production backend wraps an already-open USB fd with discovery disabled

### Phase 3: Pairing And Secure Session

Deliverables:

- documented pair-data format or opaque pair-data storage contract
- session state machine
- handshake transcript comparison against Windows and bridge captures
- key material lifecycle and log redaction tests

Verification:

- Rust can open a session far enough to produce the same next USB traffic as
  the Windows driver for a replayed trace
- no secrets appear in logs under normal verbosity

### Phase 4: Status And Capture Bring-Up

Deliverables:

- sensor status operation
- capture start/finish flow
- bad-capture and timeout mapping
- `tudor-cli capture` for hardware experiments

Verification:

- CLI can wait for touch and produce the same high-level event sequence as the
  bridge
- cancel during capture returns to idle

### Phase 5: On-Sensor Match And Delete

Deliverables:

- verify against a requested GUID/finger through the sensor/native store
- identify through the sensor/native store, with TOD/fprintd candidate filtering
  documented
- delete one, wildcard delete, and clear commands
- no-match and retry mapping
- explicit answer for whether any supported profile needs host-side matching

Verification:

- fixture tests cover match, no-match, retry, and hard failure
- fixture tests cover every delete and clear scope
- hardware verify matches an enrolled finger through the CLI
- cross-user or synthetic multi-GUID identify behavior is documented and tested

### Phase 6: Enrollment And Persistent State

Deliverables:

- enrollment sample loop
- duplicate detection policy for v132 native storage
- on-sensor/native-storage template commit
- pairing-data persistence through existing IPC
- storage-broker design only if a non-v132 profile requires host-side templates

Verification:

- CLI can enroll, restart, and verify the same GUID/finger
- delete removes the template from the persistent sensor/native store
- clear removes all templates without touching unrelated bridge state
- host restart reloads pairing data without host filesystem access

### Phase 7: Host Swap Behind Existing fprintd Path

Deliverables:

- `tudor-host-rs` speaks the existing IPC protocol
- launcher can select C host or Rust host by build option or environment
- C TOD module works unchanged against Rust host
- fallback to C bridge remains easy during bring-up
- Rust host runs under the unmodified seccomp and namespace sandbox

Verification:

- `fprintd-enroll`, `fprintd-verify`, `fprintd-list`, and `fprintd-delete`
  work through the Rust host
- sudo and polkit authenticate through fprintd
- host restart, unplug/replug, and suspend/resume survive
- protocol/state failures close the session or exit cleanly enough for fprintd
  to recover

### Phase 8: Package And Retire DLL Path

Deliverables:

- Arch/CachyOS packaging for the Rust driver
- feature flag or separate package for legacy DLL bridge
- contributor capture guide
- support matrix updated from real reports

Verification:

- clean install on a second machine can enroll and verify using documented
  steps
- package does not redistribute vendor DLLs unless explicitly configured as a
  legacy/debug option

## Risk Register

| Risk | Impact | Mitigation |
| --- | --- | --- |
| Secure-session behavior is not standard TLS | Blocks native open | Keep transcript fixtures, isolate `tudor-session`, avoid assuming TLS library fit too early |
| Host sandbox forbids filesystem access | Breaks any file-backed Rust host DB | Broker state through IPC or pass pre-opened fds before sandbox activation |
| USB library uses unavailable syscalls | Host is killed by seccomp | Prove syscall footprint under sandbox before choosing runtime/backend |
| Native template format is opaque | Blocks migration from bridge DB | Keep bridge and Rust storage distinct; require re-enroll until format is documented |
| v132 matching is sensor-side | Wrong host-side matcher design | Treat host matching as a separate profile gated by captures |
| Capture payloads contain biometric data | Privacy and redistribution risk | Redact fixtures by default and keep raw captures out of git |
| Device firmware variants diverge | Broken support claims | Require per-device captures before advertising support |
| Native identify can surface GUIDs outside fprintd candidates | Multi-user privacy confusion | Keep TOD/fprintd filtering explicit and test multi-GUID behavior |
| libfprint TOD ABI resists Rust | Packaging friction | Keep C TOD and swap only host first |
| PAM/polkit failures look like driver failures | Debug confusion | Keep plain `fprintd-verify` as the first integration gate |

## Open Questions

- Which Tudor variants share the v132 command set and secure-session protocol?
- Which parts of the Windows flow are firmware requirements versus WinBio
  policy?
- Can `native-storage.dat` be documented enough for read or migration support?
- What quality metrics does the engine need to match Windows enroll behavior?
- Do any supported non-v132 profiles require host-side template comparison?
- Can the Rust implementation use an existing TLS stack, or does it need a
  record-layer adapter around a custom handshake?
- What is the minimal sensor/native-storage command set needed for verify,
  identify, delete, and clear?
- Should persistence beyond pairing data be brokered by the TOD module, the
  launcher, or a pre-opened fd if a future profile requires it?
- Which suspend/resume state must be rebuilt versus persisted?

## Good First Engineering Tasks

- Add a hardware matrix document with VID, PID, marketing name, Windows driver
  version, firmware clues, and bridge/Rust status.
- Define the capture fixture schema for `tudor-trace`.
- Add a replay test for the visible control transfers in the known identify
  trace.
- Add stable bridge log event names for USB, storage, enroll, verify, and
  identify operations.
- Create a Rust workspace skeleton with empty crates and CI-only checks.
- Write IPC layout tests from captured datagrams, including enum width, padding,
  flexible-array payloads, and `SCM_RIGHTS` fd handling.
- Add a sandbox syscall probe for the proposed Rust USB backend and runtime.
- Add regression fixtures for `CLEAR_RECORDS` versus `CLEAR_HOST_RECORDS` and
  wildcard delete scopes.

The bridge got the reader working. The Rust build should preserve that behavior
while deleting the proprietary runtime dependency one protocol layer at a time.
