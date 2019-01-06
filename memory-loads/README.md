Applied Benchmarks: Memory Loads
==================================

TODO.

Analysis
--------

Please see the detailed analysis later.

Compilation
-----------

    $ make
    c++ -I../benchmark/include -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith -Wcast-qual -Werror -std=c++17 -O3 -g   -c -o memory-loads.o memory-loads.cpp
    c++ -o memory-loads memory-loads.o -L../build/src -lbenchmark

Example Run
-----------

    $ ./memory-loads
    Running ./memory-loads
    Run on (4 X 2600 MHz CPU s)
    CPU Caches:
    L1 Data 32K (x2)
    L1 Instruction 32K (x2)
    L2 Unified 262K (x2)
    L3 Unified 3145K (x1)
    Load Average: 1.30, 1.49, 1.53
    -----------------------------------------------------------------------------------------------------
    Benchmark                                                Time             CPU   Iterations     Stride
    -----------------------------------------------------------------------------------------------------
    misaligned_list/size KB:32/offset:0                   1.30 ns         1.30 ns    526000000         64
    misaligned_list/size KB:32/offset:1                   1.30 ns         1.30 ns    538000000         64
    misaligned_list/size KB:32/offset:63                  3.17 ns         3.16 ns    229000000         64
    misaligned_list/size KB:32/offset:4095                46.2 ns         46.0 ns     13000000         4k
    misaligned_list/size KB:128/offset:0                  1.79 ns         1.79 ns    375000000         64
    misaligned_list/size KB:128/offset:1                  1.95 ns         1.95 ns    385000000         64
    misaligned_list/size KB:128/offset:63                 4.34 ns         4.33 ns    180000000         64
    misaligned_list/size KB:128/offset:4095                130 ns          130 ns      5000000         4k
    misaligned_list/size KB:1024/offset:0                 2.48 ns         2.48 ns    310000000         64
    misaligned_list/size KB:1024/offset:1                 2.30 ns         2.30 ns    294000000         64
    misaligned_list/size KB:1024/offset:63                4.01 ns         4.01 ns    175000000         64
    misaligned_list/size KB:1024/offset:4095               148 ns          147 ns      4000000         4k
    misaligned_list/size KB:8192/offset:0                 6.88 ns         6.83 ns    115000000         64
    misaligned_list/size KB:8192/offset:1                 7.02 ns         6.99 ns     95000000         64
    misaligned_list/size KB:8192/offset:63                5.92 ns         5.92 ns    119000000         64
    misaligned_list/size KB:8192/offset:4095               151 ns          151 ns      4000000         4k
    -----------------------------------------------------------------------------------------------------
    Benchmark                                                Time             CPU   Iterations     Stride
    -----------------------------------------------------------------------------------------------------
    misaligned_array/size KB:32/offset:0                  1.90 ns         1.90 ns    371000000         64
    misaligned_array/size KB:32/offset:1                  1.90 ns         1.90 ns    371000000         64
    misaligned_array/size KB:32/offset:63                 1.95 ns         1.95 ns    353000000         64
    misaligned_array/size KB:32/offset:4095               5.20 ns         5.20 ns    138000000         4k
    misaligned_array/size KB:128/offset:0                 3.41 ns         3.41 ns    203000000         64
    misaligned_array/size KB:128/offset:1                 3.42 ns         3.42 ns    205000000         64
    misaligned_array/size KB:128/offset:63                3.54 ns         3.53 ns    201000000         64
    misaligned_array/size KB:128/offset:4095              24.7 ns         24.7 ns     29000000         4k
    misaligned_array/size KB:1024/offset:0                3.89 ns         3.87 ns    178000000         64
    misaligned_array/size KB:1024/offset:1                3.71 ns         3.71 ns    188000000         64
    misaligned_array/size KB:1024/offset:63               3.76 ns         3.76 ns    178000000         64
    misaligned_array/size KB:1024/offset:4095             28.7 ns         28.7 ns     25000000         4k
    misaligned_array/size KB:8192/offset:0                3.81 ns         3.80 ns    181000000         64
    misaligned_array/size KB:8192/offset:1                3.80 ns         3.80 ns    179000000         64
    misaligned_array/size KB:8192/offset:63               3.88 ns         3.87 ns    177000000         64
    misaligned_array/size KB:8192/offset:4095             91.4 ns         91.4 ns      6000000         4k
    -----------------------------------------------------------------------------------------------------
    Benchmark                                                Time             CPU   Iterations  List Size
    -----------------------------------------------------------------------------------------------------
    cache_associativity_list/ways:2                       1.30 ns         1.29 ns    542000000        128
    cache_associativity_list/ways:4                       1.29 ns         1.29 ns    541000000        256
    cache_associativity_list/ways:8                       1.29 ns         1.29 ns    539000000        512
    cache_associativity_list/ways:16                      3.88 ns         3.88 ns    181000000       1024
    cache_associativity_list/ways:32                      3.91 ns         3.90 ns    181000000         2k
    cache_associativity_list/ways:64                      6.93 ns         6.93 ns    102000000         4k
    cache_associativity_list/ways:128                     13.7 ns         13.7 ns     51000000         8k
    cache_associativity_list/ways:256                     13.8 ns         13.8 ns     50000000        16k
    cache_associativity_list/ways:512                     22.8 ns         22.7 ns     29000000        32k
    cache_associativity_list/ways:1024                    36.3 ns         36.2 ns     20000000        64k
    cache_associativity_list/ways:2048                    68.0 ns         68.0 ns      9000000       128k
    cache_associativity_list/ways:8192                    76.4 ns         76.3 ns      9000000       512k
    -----------------------------------------------------------------------------------------------------
    Benchmark                                                Time             CPU   Iterations  Read Rate
    -----------------------------------------------------------------------------------------------------
    hardware_prefetch_list/size KB:32/stride:64           1.29 ns         1.29 ns    545872896 46.2127G/s
    hardware_prefetch_list/size KB:32/stride:128          2.21 ns         2.20 ns    294003200 27.0425G/s
    hardware_prefetch_list/size KB:32/stride:192          1.32 ns         1.32 ns    532700672 45.2619G/s
    hardware_prefetch_list/size KB:32/stride:320          2.62 ns         2.62 ns    252616704 22.7606G/s
    hardware_prefetch_list/size KB:32/stride:448          3.39 ns         3.38 ns    220414464 17.6128G/s
    hardware_prefetch_list/size KB:32/stride:704          1.72 ns         1.72 ns    407916032 34.6505G/s
    hardware_prefetch_list/size KB:32/stride:832          1.80 ns         1.80 ns    391207936 33.2048G/s
    hardware_prefetch_list/size KB:32/stride:4096         22.9 ns         22.9 ns     30406656 2.60804G/s
    hardware_prefetch_list/size KB:32/stride:4160         3.78 ns         3.77 ns    187106816 15.7953G/s
    hardware_prefetch_list/size KB:32/stride:8192         22.1 ns         22.1 ns     38528000 2.69404G/s
    hardware_prefetch_list/size KB:128/stride:64          1.65 ns         1.65 ns    429570048 36.1258G/s
    hardware_prefetch_list/size KB:128/stride:128         3.29 ns         3.29 ns    228775936 18.1071G/s
    hardware_prefetch_list/size KB:128/stride:256         10.9 ns         10.9 ns     64618496 5.48022G/s
    hardware_prefetch_list/size KB:128/stride:512         11.3 ns         11.2 ns     61140992 5.29871G/s
    hardware_prefetch_list/size KB:128/stride:1024        18.5 ns         18.5 ns     37513216  3.2199G/s
    hardware_prefetch_list/size KB:128/stride:2048        32.8 ns         32.7 ns     21190656  1.8226G/s
    hardware_prefetch_list/size KB:128/stride:4096        67.1 ns         67.1 ns     12036096 909.657M/s
    hardware_prefetch_list/size KB:128/stride:4160        9.70 ns         9.70 ns     72237056 6.14557G/s
    hardware_prefetch_list/size KB:128/stride:8192        63.7 ns         63.6 ns     10024960  959.17M/s
    hardware_prefetch_list/size KB:1024/stride:64         2.16 ns         2.16 ns    327417856 27.5809G/s
    hardware_prefetch_list/size KB:1024/stride:128        4.88 ns         4.87 ns    135069696 12.2321G/s
    hardware_prefetch_list/size KB:1024/stride:256        27.6 ns         27.6 ns     24870912 2.15917G/s
    hardware_prefetch_list/size KB:1024/stride:512        52.9 ns         52.8 ns     13549568   1.129G/s
    hardware_prefetch_list/size KB:1024/stride:1024       57.0 ns         57.0 ns     12156928 1070.43M/s
    hardware_prefetch_list/size KB:1024/stride:2048       65.1 ns         65.1 ns     10354688 937.271M/s
    hardware_prefetch_list/size KB:1024/stride:4096       76.0 ns         75.9 ns      9175040 803.826M/s
    hardware_prefetch_list/size KB:1024/stride:4160       21.3 ns         21.3 ns     33652736 2.80456G/s
    hardware_prefetch_list/size KB:1024/stride:8192       77.8 ns         77.7 ns      8716288 785.196M/s
    hardware_prefetch_list/size KB:8192/stride:64         4.45 ns         4.45 ns    165412864 13.3903G/s
    hardware_prefetch_list/size KB:8192/stride:128        16.2 ns         16.2 ns     42205184 3.68963G/s
    hardware_prefetch_list/size KB:8192/stride:256        50.9 ns         50.9 ns     13238272 1.17122G/s
    hardware_prefetch_list/size KB:8192/stride:512        68.8 ns         68.8 ns      9830400 887.754M/s
    hardware_prefetch_list/size KB:8192/stride:1024       58.2 ns         58.1 ns     11272192 1050.01M/s
    hardware_prefetch_list/size KB:8192/stride:2048       66.1 ns         66.1 ns      9306112 923.203M/s
    hardware_prefetch_list/size KB:8192/stride:4096       78.9 ns         78.8 ns      7864320 774.257M/s
    hardware_prefetch_list/size KB:8192/stride:4160       73.4 ns         73.3 ns      8257536 832.446M/s
    hardware_prefetch_list/size KB:8192/stride:8192       83.3 ns         83.3 ns      7077888 733.144M/s
    -----------------------------------------------------------------------------------------------------
    Benchmark                                                Time             CPU   Iterations  Read Rate
    -----------------------------------------------------------------------------------------------------
    cache_hierarchy_list/size KB:8                        1.29 ns         1.29 ns    542000000 46.0761G/s
    cache_hierarchy_list/size KB:16                       1.29 ns         1.29 ns    542000000 46.0677G/s
    cache_hierarchy_list/size KB:32                       1.31 ns         1.31 ns    537000000 45.6396G/s
    cache_hierarchy_list/size KB:64                       3.89 ns         3.89 ns    181000000 15.3342G/s
    cache_hierarchy_list/size KB:128                      3.89 ns         3.88 ns    180000000 15.3423G/s
    cache_hierarchy_list/size KB:256                      4.52 ns         4.52 ns    156000000 13.1899G/s
    cache_hierarchy_list/size KB:512                      13.9 ns         13.8 ns     52000000 4.30439G/s
    cache_hierarchy_list/size KB:1024                     13.9 ns         13.8 ns     52000000 4.30485G/s
    cache_hierarchy_list/size KB:2048                     17.5 ns         17.4 ns     43000000 3.41793G/s
    cache_hierarchy_list/size KB:4096                     37.4 ns         37.4 ns     19000000 1.59566G/s
    cache_hierarchy_list/size KB:8192                     61.9 ns         61.9 ns     12000000 986.496M/s
    cache_hierarchy_list/size KB:16384                    77.7 ns         77.7 ns      9000000 785.796M/s
    cache_hierarchy_array/size KB:8                       1.35 ns         1.35 ns    517000000 44.1412G/s
    cache_hierarchy_array/size KB:16                      1.13 ns         1.13 ns    624000000  52.795G/s
    cache_hierarchy_array/size KB:32                      1.06 ns         1.06 ns    663000000 56.0227G/s
    cache_hierarchy_array/size KB:64                      1.03 ns         1.03 ns    680000000 57.8788G/s
    cache_hierarchy_array/size KB:128                     1.01 ns         1.00 ns    699000000  59.349G/s
    cache_hierarchy_array/size KB:256                     1.15 ns         1.15 ns    611000000 52.0129G/s
    cache_hierarchy_array/size KB:512                     1.65 ns         1.65 ns    421000000 36.1116G/s
    cache_hierarchy_array/size KB:1024                    1.66 ns         1.65 ns    416000000 36.0237G/s
    cache_hierarchy_array/size KB:2048                    2.68 ns         2.67 ns    260000000 22.2943G/s
    cache_hierarchy_array/size KB:4096                    5.07 ns         5.06 ns    100000000 11.7808G/s
    cache_hierarchy_array/size KB:8192                    9.34 ns         9.34 ns     76000000 6.38384G/s
    cache_hierarchy_array/size KB:16384                   10.2 ns         10.2 ns     70000000  5.8678G/s
    -----------------------------------------------------------------------------------------------------
    Benchmark                                                Time             CPU   Iterations  List Size
    -----------------------------------------------------------------------------------------------------
    tlb_cache_list/pages:16                              0.376 ns        0.376 ns   1000000000       1024
    tlb_cache_list/pages:32                              0.553 ns        0.553 ns   1000000000         2k
    tlb_cache_list/pages:64                              0.860 ns        0.859 ns    815622720         4k
    tlb_cache_list/pages:128                              3.03 ns         3.03 ns    230596992         8k
    tlb_cache_list/pages:256                              3.46 ns         3.46 ns    201953792        16k
    tlb_cache_list/pages:512                              4.05 ns         4.04 ns    173795840        32k
    tlb_cache_list/pages:1024                             6.35 ns         6.34 ns    110594048        64k
    tlb_cache_list/pages:2048                             9.87 ns         9.87 ns     67710976       128k
    tlb_cache_list/pages:4096                             17.6 ns         17.6 ns     40783872       256k
    tlb_cache_list/pages:8192                             20.8 ns         20.8 ns     34004992       512k
    -----------------------------------------------------------------------------------------------------
    Benchmark                                                Time             CPU   Iterations  Read Rate
    -----------------------------------------------------------------------------------------------------
    cache_conflicts_list/size KB:32/stride:4032           3.70 ns         3.70 ns    187971072 16.1285G/s
    cache_conflicts_list/size KB:32/stride:4096           19.8 ns         19.8 ns     48081408 3.01021G/s
    cache_conflicts_list/size KB:32/stride:4160           3.75 ns         3.75 ns    186110976 15.8816G/s
    cache_conflicts_list/size KB:128/stride:4032          9.57 ns         9.56 ns     73207808 6.23594G/s
    cache_conflicts_list/size KB:128/stride:4096          60.1 ns         60.0 ns      9465856  1016.9M/s
    cache_conflicts_list/size KB:128/stride:4160          9.69 ns         9.69 ns     70776832 6.15249G/s
    cache_conflicts_list/size KB:1024/stride:4032         20.8 ns         20.8 ns     32980992 2.86254G/s
    cache_conflicts_list/size KB:1024/stride:4096         75.8 ns         75.8 ns      9191424 805.375M/s
    cache_conflicts_list/size KB:1024/stride:4160         21.4 ns         21.4 ns     33423360 2.79012G/s
    cache_conflicts_array/size KB:32/stride:4032         0.910 ns        0.910 ns    770814976 65.5299G/s
    cache_conflicts_array/size KB:32/stride:4096          4.14 ns         4.13 ns    163661312 14.4148G/s
    cache_conflicts_array/size KB:32/stride:4160         0.917 ns        0.915 ns    766216704 65.1146G/s
    cache_conflicts_array/size KB:128/stride:4032         7.65 ns         7.65 ns     91437056 7.79264G/s
    cache_conflicts_array/size KB:128/stride:4096         8.82 ns         8.82 ns     80138240  6.7593G/s
    cache_conflicts_array/size KB:128/stride:4160         7.77 ns         7.77 ns     89384960 7.67108G/s
    cache_conflicts_array/size KB:1024/stride:4032        8.56 ns         8.55 ns     82493440 6.97148G/s
    cache_conflicts_array/size KB:1024/stride:4096        10.1 ns         10.1 ns     69763072  5.9259G/s
    cache_conflicts_array/size KB:1024/stride:4160        8.68 ns         8.67 ns     80986112 6.87126G/s
