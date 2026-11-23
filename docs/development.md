# Development

## Runtime prerequisites

Use a Linux environment with GNU Make and m4, plus the tools built from the
course's `toolchain.tar.gz`. Follow that archive's README for installation and
host dependencies. The course downloads require course access and are not redistributed here.

The verified local setup is Ubuntu 24.04 under WSL2, with:

| Tool | Verified version |
| --- | --- |
| ARM GCC | 16.2.0 |
| GNU binutils | 2.47.20260726 |
| GDB | 17.2 |
| Course-patched QEMU | 11.0.3 |
| clang-format | 22.1.8 |

The Makefile defaults to `$HOME/.local/bin/bsprak`. On another installation:

```bash
make BSPRAK_PREFIX=/path/to/bsprak
make qemu BSPRAK_PREFIX=/path/to/bsprak
```

The supplied static test libraries are retained because the default course
targets link them. The binary replacement for Aufgabe 1 (`libe1.a`) is not used.

## Common commands

```bash
make                 # Build the ELF and disassembly
make qemu            # Build and run with the course QEMU settings
make clean           # Remove generated build files
make kernel_only     # Verify that the kernel links independently
make user_only       # Verify that the supplied user scaffold links independently
```

For the Windows checkout at `C:\Betriebssystempraktikum\repository`, the current
local WSL setup can run it directly from PowerShell:

```powershell
wsl -d Ubuntu -u root --cd /mnt/c/Betriebssystempraktikum/repository --exec make qemu
```

The distro, user, and checkout path are local choices; adapt them for another
machine. Keep compilation inside Linux rather than invoking Windows build tools.

## Debugging

In one Linux terminal:

```bash
make qemu_debug
```

In a second terminal, in the same checkout:

```bash
make gdb
```

The Makefile connects GDB to port 1234. Useful starting points include
`start_kernel`, `exception_handler`, and the exception-trigger functions. The
course notes identify `VBAR_S` as the useful GDB register for checking the vector
base; GDB's plain `VBAR` view can be misleading on this setup.

## Validation

Use the default `make qemu` timing with the updated register checker.

1. Confirm the startup marker and the course test-library message. Periodic
   `!` lines should follow without IRQ register dumps.
2. Type `G` and expect `Unknown input: G`.
3. Type `c`. The return-address, general-register, CPSR, and SPSR checks should
   run with timer interrupts active and finish with
   `Fertig! Es _scheint_ so, als wuerde es funktionieren.`
4. Toggle `d` on and off. IRQ dumps should appear only while it is enabled.
5. Type `e`, then `A` and `B` while output is busy. With the supplied configuration,
   each letter is printed five times, in order, with timer ticks interspersed.
6. In separate runs, test `a`, `p`, `s`, and `u`. Each should print the matching
   exception and register report, send EOT, and exit QEMU successfully.

Tap keys without Enter. Leave an interactive run with Ctrl+A, release, then X.
The `e` routine does not return to the command loop; restart QEMU for another mode.

Additional local verification checked all reported fault instruction addresses
against the ELF symbols, the five aligned 16 KiB stacks, an aligned vector table,
runtime timer-interval configuration, a full 128-byte receive buffer, overflow
handling, and a clean build/run of the extracted submission archive. The final
revised Aufgabe 2 archive was reported to pass 60/60 tests on ISIS.

The hosted `Checks` workflow runs clang-format 22.1.8 on maintained C/header
files and checks patch whitespace. `.clang-format-ignore` leaves supplied course
source unchanged. CI does not have the authenticated course toolchain or the
ISIS grader, so its green status represents source checks, not a runtime grade.

## Stage workflow

Develop each next stage on its own branch and commit meaningful changes as
they are made. Run the build and relevant runtime checks before merging
the completed work into `main`. Tag each completed milestone as `stage-N`.

To inspect a completed historical version:

```bash
git switch --detach stage-1
make clean
make qemu
git switch main
```

The existing tags point to the original final source commits. The original
development history remains intact, including revisions and the shutdown fix.

## Course submission

The course Makefile requires a clean, committed checkout on `main`:

```bash
git switch main
git status --short
make submission
```

Upload the resulting `submission.tar.gz` without renaming it. Keep generated
submission archives outside Git, and recheck a newly submitted archive on ISIS.
The tags identify code checkpoints; the original course submission files remain
separate from this repository.
