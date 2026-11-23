# ARMv7 Kernel

[![Checks](https://github.com/Robertsvg/armv7-kernel/actions/workflows/checks.yml/badge.svg)](https://github.com/Robertsvg/armv7-kernel/actions/workflows/checks.yml)

A bare-metal ARMv7 kernel for the Raspberry Pi 2 B, written in freestanding
C23 and ARM assembly. Development progresses from serial I/O and formatted
output to exception handling, buffered input, and timer interrupts.

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
- Register-preservation checks and QEMU shutdown after fatal exceptions.

## Development stages

| Stage | Scope | Status |
| --- | --- | --- |
| [Stage 1 — Serial I/O and Formatting](https://github.com/Robertsvg/armv7-kernel/tree/stage-1) | PL011 UART driver and `kprintf` | Complete |
| [Stage 2 — Exceptions and Interrupts](https://github.com/Robertsvg/armv7-kernel/tree/stage-2) | Exception reporting, register preservation, buffered input, and timer | Complete |

`main` contains the latest completed kernel and repository documentation.
Stage tags preserve the final revised source for each milestone. Future stages
will extend this kernel on development branches and receive their own tags after validation.

## Build and run

A Linux environment is required; Ubuntu under WSL2 is supported. The verified
runtime uses a specific ARM toolchain and patched QEMU build. Follow the README
in the original `toolchain.tar.gz` distribution; see
[development prerequisites](docs/development.md#runtime-prerequisites) for its
source and verified versions. The toolchain is not bundled in this repository.

With the tools installed at the Makefile's default prefix,
`$HOME/.local/bin/bsprak`:

```bash
git clone https://github.com/Robertsvg/armv7-kernel.git
cd armv7-kernel
make
make qemu
```

For a different installation prefix, pass `BSPRAK_PREFIX=/path/to/bsprak` to
`make`. See [development](docs/development.md) for verified tool versions,
Windows/WSL commands, debugging, and validation.

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
| `c` | Run the register-preservation checker. |
| `e` | Enter the repeated-character printing routine. |

The timer prints `!` periodically. After `e`, characters are printed
`PRINT_COUNT` times while incoming characters are buffered by the UART interrupt
handler. Fatal exception commands print a register report, send EOT, and exit
QEMU. To leave a running interactive session, press **Ctrl+A**, release, then **X**.

## Validation

Stage 2 has been checked with a clean build, all four exception paths and fault
addresses, EOT and QEMU shutdown, timer interrupts, IRQ debug toggling, buffered
input, and all four register checks with interrupts active.

GitHub Actions checks formatting and patch hygiene. Runtime verification uses
the installed ARM toolchain and patched QEMU rather than the hosted CI runner.
See the
[validation procedure](docs/development.md#validation).

## Layout

```text
arch/bsp/       PL011 UART, interrupt controller, and system-timer drivers
arch/cpu/       Startup, exception vectors, trampolines, and exception handling
include/       Driver, kernel, and test interfaces
kernel/        Initialization, runtime configuration, and demonstration loop
lib/           kprintf and supplied low-level support
static/        Supplied test libraries
tests/         Register-preservation checker
user/          Scaffold for later user-space work
docs/          Architecture, development, and provenance
kernel.lds     Linker layout and reserved mode stacks
```

## Documentation

Maintained by [Yousef Mousa](https://github.com/Robertsvg). See
[architecture](docs/architecture.md) for the execution flow,
[development](docs/development.md) for build and debugging instructions, and
[provenance](docs/provenance.md) for the project's course origins, supplied
components, and original source revisions.
