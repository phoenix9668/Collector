"""Fail when the firmware outgrows its memory budget.

usage: check_size.py <Collector.elf> [--ram-max BYTES] [--flash-max BYTES] [--size arm-none-eabi-size]

RAM = .data + .bss (includes the FreeRTOS task stacks, the MSP and the heap
reservation); FLASH = .text + .data. Defaults keep 1 KB of the STM32L051's
8 KB RAM and 16 KB of its 64 KB flash in reserve.
"""
import argparse
import subprocess
import sys


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("elf")
    ap.add_argument("--ram-max", type=int, default=7 * 1024 + 200)
    ap.add_argument("--flash-max", type=int, default=48 * 1024)
    ap.add_argument("--size", default="arm-none-eabi-size")
    args = ap.parse_args()

    out = subprocess.run([args.size, args.elf], check=True, capture_output=True, text=True).stdout
    text, data, bss = (int(v) for v in out.splitlines()[1].split()[:3])
    ram, flash = data + bss, text + data
    print("RAM   %5d / %5d B" % (ram, args.ram_max))
    print("FLASH %5d / %5d B" % (flash, args.flash_max))
    if ram > args.ram_max or flash > args.flash_max:
        print("memory budget exceeded")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
