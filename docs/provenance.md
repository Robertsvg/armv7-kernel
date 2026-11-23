# Provenance

Maintained by Yousef Mousa (`Robertsvg`) for TU Berlin's Betriebssystempraktikum,
winter semester 2026/27. The project targets the Raspberry Pi 2 B / BCM2836 and
uses the course-patched QEMU as its execution reference.

## Source checkpoints

The existing local development history is preserved without rewriting its
authors or timestamps.

| Milestone | Original source commit |
| --- | --- |
| Initial E0 scaffold | `4405f6a` |
| Stage 1 (Aufgabe 1), final revised source | `2876e9b5980c989baf24bcf9f11ebdd94d224e23` |
| Stage 2 (Aufgabe 2), updated official checker | `388977b6cced23f577750c9c0c752e0e104d65cf` |

The Aufgabe 2 source matches the final course archive whose SHA-256 is
`ed199a6fe680e2210a582f5be0d83bc4ccead70fbc86bcfba6ffa2e8567e3fdb`.
That archive's 60/60 ISIS result was confirmed after the checker update.
The annotated tags `stage-1` and `stage-2` mark the final revised source
checkpoints. Repository documentation and CI were added afterward.

## Assignment implementation

The assignment work includes the PL011 UART driver and `kprintf`, interrupt
controller and system-timer drivers, exception vectors and trampolines,
exception reporting, mode-register snapshots, and the required startup and
demonstration integration. It builds on the supplied scaffold and examples;
the repository is not presented as a wholly original implementation of every
included component.

## Supplied course components

- The initial project layout, Makefile, linker-script foundation, boot/Hyp-mode
  setup, powerdown helper, yellow LED driver, string/UBSan support, configuration
  files, and user scaffold originate in the course Vorgabe.
- `arch/cpu/entry.S`, `kernel.lds`, and the Makefile were extended for the
  assignments. `kernel/start.c` follows the required demonstration examples.
- `include/lib/ringbuffer.h` is the supplied ring-buffer implementation.
- `tests/regcheck.c`, `tests/regcheck_asm.S`, and `include/tests/regcheck.h`
  are the revised official register checker downloaded on 19 November 2026.
  They are retained byte-for-byte from that update.
- `static/libktest.a`, `static/libutest.a`, and their headers are supplied test
  interfaces/libraries, not a reimplementation of the course's grading system.
- The `.clang-format` configuration is supplied with the course scaffold and
  retains its existing SPDX notice.

The alternative Aufgabe 1 binary solution (`libe1.a`) is not included or linked.
Course PDFs, downloaded archives, installation caches, local helper scripts,
inspection reports, and generated build/submission artifacts are excluded.

## Distribution

The repository starts private while the course is in progress. Confirm the
course's publication and redistribution terms before making solutions or
supplied components public. Existing notices remain in place. No repository-wide
license is asserted over third-party course material.
