# Bootloader and uCore

This repository contains a small OS-style project with a bootloader and a `ucore` source tree. The bootloader loads an ELF kernel image and jumps to its entry point; the `ucore` tree contains kernel, glue, and tool sources for multiple architectures.

## Layout

- `bootloader/`: low-level bootloader code, including the ARM Pandaboard boot path.
- `ucore/src/bootloader/`: per-architecture bootloader sources.
- `ucore/src/glue-kern/`: kernel glue and trap interfaces.
- `ucore/src/kern-ucore/`: main kernel sources.
- `ucore/package/`: packaged support tools, including configuration utilities.

## Bootloader Notes

The ARM bootloader clears BSS, prints status over UART, validates the ELF magic, copies each ELF program segment into place, and then jumps to the kernel entry point. If the image is invalid, it prints an error and halts.

## Usage

Build and flash steps are target-specific. Use the project build system or cross compiler toolchain, flash the generated image to the target board, and connect a serial console to observe boot messages.
