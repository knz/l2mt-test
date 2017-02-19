This directory contains a set of stress tests / benchmarks for
LEON2-MT.

How to use
----------

The programs perform a computation and emit some performance results
to the UART output. Each program produces one line of output with the
following fields:

- the name of the algorithm
- the frame ratio (see below)
- the block size (see below)
- the number of instructions executed (w/ wraparound at 2^32 instructions)
- the number of clock cycles for the execution (w/ wraparound at 2^32 cycles)

Each program accepts two compile-time parameters:

- FRAME_RATIO: a size reduction ratio for the dataset processed by the
  algorithm. The base size is 2048; a frame ratio of e.g. 16
  corresponds to a frame size of 128x128 pixels. (This is incidentally
  the smallest size supported by the algorithms.)

- BLOCKSIZE: the number of concurrent threads used by the computation.

The enclosed `Makefile` produces test binaries for combinations of
ratios 1 to 16 and block sizes 1,2,4,8,16,30.

The enclosed `Makefile` rule `run` (make run) will run all these
programs in an order that makes it easier to copy-paste the output to
e.g. a spreadsheet program.

Origin / Copyright / Disclaimer
-------------------------------

The programs in this directory are **derived** from reference code
implemented by Airbus Defence and Space and funded by the European
Space Agency (as copyright holder) under the name
"preProcessingFixedPoint" and licensed under the GPL v3. The
algorithms contained therein are a simplified version of a part of the
software for the payload of the Euclid mission
(http://sci.esa.int/euclid/).

Authors of the original software:
 Andreas Jung (European Space Agency - andreas.jung@esa.int)
 Pierre-Elie Crouzet (European Space Agency - pierre-elie.crouzet@esa.int)
 Philippe Hyounet (Airbus Defence and Space - philippe.hyounet@airbus.com)

The software in this directory is a partial re-implementation (near
complete re-write) by Raphael 'kena' Poss, also funded by the European
Space Agency via ITI contract 4000111863. It is also licensed under
the GPL v3. However, neither the European Space Agency nor Airbus
Defence and Space endorses this derived software.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
