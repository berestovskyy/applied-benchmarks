Applied Benchmark: Memory Latency
=================================

Benchmarking Kaby Lake and Haswell memory latency using lists.

Analysis
--------

Please see the [detailed analysis on Medium](https://medium.com/applied/applied-c-memory-latency-d05a42fe354e).

Compilation
-----------

    $ make
    c++ -I../benchmark/include -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith -Wcast-qual -Werror -std=c++17 -O3 -g   -c -o memory-loads.o memory-loads.cpp
    c++ -o memory-loads memory-loads.o -L../build/src -lbenchmark

Example Run on Intel Kaby Lake
------------------------------

    Run on (4 X 2500 MHz CPU s)
    CPU Caches:
      L1 Data 32K (x2)
      L1 Instruction 32K (x2)
      L2 Unified 262K (x2)
      L3 Unified 4194K (x1)
    Load Average: 2.06, 1.80, 1.63
    ---------------------------------------------------------------------------------------------------
    Benchmark                                   Time             CPU   Iterations      Nodes  Read Rate
    ---------------------------------------------------------------------------------------------------
    memory_latency_list/size KB:1            1.01 ns         1.01 ns    675000000         16 58.8502G/s
    memory_latency_list/size KB:2            1.02 ns         1.02 ns    682000000         32 58.4426G/s
    memory_latency_list/size KB:4            1.10 ns         1.10 ns    651000000         64 54.3245G/s
    memory_latency_list/size KB:8            3.32 ns         3.31 ns    202000000        128 17.9975G/s
    memory_latency_list/size KB:16           3.32 ns         3.32 ns    198000000        256 17.9732G/s
    memory_latency_list/size KB:32           3.33 ns         3.32 ns    214000000        512  17.965G/s
    memory_latency_list/size KB:64           5.33 ns         5.33 ns    131000000       1024 11.1826G/s
    memory_latency_list/size KB:128          8.58 ns         8.56 ns     81000000         2k 6.96042G/s
    memory_latency_list/size KB:256          13.9 ns         13.8 ns     50000000         4k 4.30956G/s
    memory_latency_list/size KB:512          14.2 ns         14.2 ns     50000000         8k  4.2061G/s
    memory_latency_list/size KB:1024         14.2 ns         14.2 ns     48000000        16k 4.19944G/s
    memory_latency_list/size KB:2048         17.3 ns         17.3 ns     41000000        32k 3.44318G/s
    memory_latency_list/size KB:4096         63.1 ns         63.0 ns     10000000        64k 968.924M/s
    memory_latency_list/size KB:8192         96.4 ns         96.2 ns      6000000       128k 634.472M/s
    memory_latency_list/size KB:16384         104 ns          104 ns      6000000       256k 585.991M/s
