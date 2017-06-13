@echo off

@echo Compiling...
mbed compile -DMBED_HEAP_STATS_ENABLED=1 -DMBED_STACK_STATS_ENABLED=1
@echo Done.

@echo Flashing to %1...
python.exe tools\nxpprog.py --baud=230400 --cpu=lpc1768 --oscfreq=120000 %1 BUILD\LPC1768\GCC_ARM\WDY-firmware.bin
@echo Done.


