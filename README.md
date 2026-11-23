# Betriebssystempraktikum

[![Checks](https://github.com/Robertsvg/betriebssystempraktikum/actions/workflows/checks.yml/badge.svg)](https://github.com/Robertsvg/betriebssystempraktikum/actions/workflows/checks.yml)

An educational ARMv7 kernel for the Raspberry Pi 2 B, written in freestanding
C23 and ARM assembly. The project develops a small operating system step by step
through TU Berlin's Betriebssystempraktikum, with the course-patched QEMU build
as the execution reference.

## Implemented features

- PL011 UART input and output through memory-mapped registers.
- A streaming `kprintf` with signed and unsigned decimal, hexadecimal, character,
  string, pointer, and fixed-width formatting.
- Separate stacks for System/User, IRQ, Abort, Undefined, and Supervisor modes.
- An exception vector table and assembly trampolines that preserve registers
  and processor status around C handlers.
- Exception reports with fault addresses, fault-status decoding, and register snapshots.
- Interrupt-driven UART reception with a 128-byte ring buffer.
- Periodic system-timer interrupts and a keyboard-controlled demonstration.
- The official course register checker and QEMU shutdown after fatal exceptions.

## Assignment milestones

| Version | Scope | Status |
| --- | --- | --- |
| [`aufgabe-1`](https://github.com/Robertsvg/betriebssystempraktikum/tree/aufgabe-1) | PL011 UART driver and `kprintf` | Final revised source checkpoint |
| [`aufgabe-2`](https://github.com/Robertsvg/betriebssystempraktikum/tree/aufgabe-2) | Exceptions, interrupts, buffered input, and timer | Final revised source; 60/60 course tests reported from ISIS |

`main` contains the latest completed kernel and repository documentation.
Assignment tags preserve the source used for each milestone. Future assignments
will extend this kernel on development branches and receive their own tags after validation.

## Build and run

A Linux environment is required; Ubuntu under WSL2 is supported. Install the
**course toolchain**, following the README in the course's `toolchain.tar.gz`.
It includes QEMU patches required for the assignment; an arbitrary system QEMU
installation is not an equivalent test environment. The toolchain and course
downloads are not bundled in this repository.

With the tools installed at the Makefile's default prefix,
`$HOME/.local/bin/bsprak`:

```bash
git clone https://github.com/Robertsvg/betriebssystempraktikum.git
cd betriebssystempraktikum
make
make qemu
```

For a different installation prefix, pass `BSPRAK_PREFIX=/path/to/bsprak` to
`make`. See [development](docs/development.md) for verified tool versions,
Windows/WSL commands, debugging, and submission preparation.

The current kernel starts with:

```text
=== Betriebssystem gestartet ===
Kernel Tests sind korrekt eingebunden!
!
!
```

Tap keys individually, without pressing Enter:

| Key | Action |
| --- | --- |
| `d` | Toggle IRQ register dumps. |
| `a` | Trigger a Data Abort through an unaligned word access. |
| `p` | Trigger a Prefetch Abort through a breakpoint instruction. |
| `s` | Trigger a Supervisor Call. |
| `u` | Trigger an Undefined Instruction exception. |
| `c` | Run the updated official register checker. |
| `e` | Enter the repeated-character printing routine. |

The timer prints `!` periodically. After `e`, characters are printed
`PRINT_COUNT` times while incoming characters are buffered by the UART interrupt
handler. Fatal exception commands print a register report, send EOT, and exit
QEMU. To leave a running interactive session, press **Ctrl+A**, release, then **X**.

## Validation

The revised Aufgabe 2 submission passed **60/60** tests on the course's ISIS
autograder, as reported from the submitted archive. Local checks cover a clean
build, all four exception paths and fault addresses, EOT and QEMU shutdown,
timer interrupts, IRQ debug toggling, buffered input, and all four official
register checks with interrupts active.

GitHub Actions checks formatting and patch hygiene. It does **not** run the
course autograder or claim to emulate the patched hardware environment. Runtime
verification uses the installed course toolchain. See the
[validation procedure](docs/development.md#validation).

## Layout

```text
arch/bsp/       PL011 UART, interrupt controller, and system-timer drivers
arch/cpu/       Startup, exception vectors, trampolines, and exception handling
include/       Interfaces and supplied course headers
kernel/        Initialization, runtime configuration, and demonstration loop
lib/           kprintf and supplied low-level support
static/        Required course-provided test libraries
tests/         Official register checker
user/          Course scaffold for later user-space work
docs/          Architecture, development, and provenance
kernel.lds     Linker layout and reserved mode stacks
```

## Background

Maintained by [Yousef Mousa](https://github.com/Robertsvg) as an ongoing TU Berlin
Betriebssystempraktikum project for winter semester 2026/27. This repository
retains the course scaffold and supplied test components alongside the
assignment implementations. See [architecture](docs/architecture.md) for the
execution flow and [provenance](docs/provenance.md) for the supplied components
and source revisions.
