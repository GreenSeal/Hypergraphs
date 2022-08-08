# Hypergraphs
This repository contains implementation of Fiduccia-Mettheisys algorithm - algorithm for dividing a hypergraph into two parts with minimization the number of edges crossing the boudary.
To get an executable file with an algorithm, you need to create `build` directory and run the following commands:
```
cmake ..
make FM
```
After that you can run executable with 2 variants. All input hypergraphs must store in hMetis format.
1. `FM hypergraph.hgr`
2. `FM hypergraph.hgr -m`

The first option runs the algorithm without modifications. 
The second option runs the algorithm with the following modification: when recalculating the gain, the moved vertex is added not to the beginning of the corresponding list in the bucket, but to the end. Experiments show that this allows for a reduction in the final size of the cut.
Executable generate 2 files: *.part.2 and .result. The first contains representation of final graph. The format is hMetis output format. The second file contains useful runtime metrics, such as start and final cut size, relative cut size reduce and execution time. This file is used later in the script scripts/collect_statistics.py for collecting benchmark statistics.

## Benchmarks
For testing this programm I used benchmarks from ... . Benchmarks run through CMakeLists.txt using BENCHMARKS_RUN build target. 
At the moment I've been running benchmarks starting with `ISPD98_ibm` and `dac2012_superblue`. 
If you want to change that, just change file regular expression at CMakeLists.txt:11.
Benchmarks must be contained in the folder benchmark_set in repository directory.

## Useful links
Fiduccia-Mattheisys article:
Benchmarks:
hMetis format:
