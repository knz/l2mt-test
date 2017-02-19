This directory contains miscellaneous stress / benchmark tests.

Included programs:

- ca1d: computes iterations of a 1D cellular automaton.
- rgb2gray-int: computes a RGB to gray conversion using integer arithmetic.
- rgb2gray-fp: computes a RGB to gray conversion using floating point.

Each programs executes the computation across multiple input sizes,
and for each input sizes tests three strategies: pure sequential
execution, single-level parallel execution, two-level (blocked)
parallel execution. For each parallel strategy, the program also
executes using varying number of threads.

The performance measurements are reported on the UART output.
The program prints "Done!" at the end if the execution completed successfully.

The maximum input size that is tested is set by the `make` variable
MAXSIZE. By default this is set to 512. Change with e.g.:

   make MAXSIZE=100

The other parameter is the number of threads used to test the parallel
strategies.  For each size the program will try the sizes listed in
the parameter BLOCKSIZES. By default this is set to 1,2,4,8,16,30.
Change with e.g.:

   make BLOCKSIZES=1,2,4,8

The command used to compile is defined by the `make` variable SLC.
This is set by default to `slc -b l2mt_f`.
