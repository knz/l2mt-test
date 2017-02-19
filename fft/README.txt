This directory contains a copy of the FFT benchmark.

The program executes the FFT kernel across different FFT sizes, and
using different numbers of threads each time.

The performance measurements are reported on the UART output.
The program prints "Done!" at the end if the execution completed successfully.

The maximum FFT size that is tested is set by the `make`
variable TABLE_SIZE. By default this is set to 10, which means
the max FFT size is 1024 (2^10). Change with e.g.:

   make TABLE_SIZE=8

The other parameter is the number of threads used to test.  For each
size the program will try the sizes listed in the parameter
BLOCKSIZES. By default this is set to 1,2,4,8,16,30.
Change with e.g.:

   make BLOCKSIZES=1,2,4,8

The command used to compile is defined by the `make` variable SLC.
This is set by default to `slc -b l2mt_f`.

