Explain how you placed the MPI Processes in different locations (node, socket, processor) to evaluate PingPong as well as latency of 4 byte message.

1. communication of processes on different cores in the same processor (core-to-core)

Executed program with command "mpiexec -genv I_MPI_DEBUG=4 -env I_MPI_PIN_PROCESSOR_LIST=0,2 -n 2"

[0] MPI startup(): Intel(R) MPI Library, Version 2019 Update 8  Build 20200624 (id: 4f16ad915)
[0] MPI startup(): Copyright (C) 2003-2020 Intel Corporation.  All rights reserved.
[0] MPI startup(): library kind: release
[0] MPI startup(): libfabric version: 1.10.1-impi
[0] MPI startup(): libfabric provider: psm2
[0] MPI startup(): Rank    Pid      Node name     Pin cpu
[0] MPI startup(): 0       185204   i05r11c06s08  0
[0] MPI startup(): 1       185205   i05r11c06s08  2

Starting Measurement with data-size 2^2
Data size : 2^2 (B), Time (ms) :     0.000812448, Bandwidth (MB/s) :     4.923389748


Here it can be seen that process runs on different cores on same socket on same node

2. same node but different processors (socket-to-socket)

Executed program with command "mpiexec -genv I_MPI_DEBUG=4 -env I_MPI_PIN_DOMAIN=socket -n 2"

[0] MPI startup(): Intel(R) MPI Library, Version 2019 Update 8  Build 20200624 (id: 4f16ad915)
[0] MPI startup(): Copyright (C) 2003-2020 Intel Corporation.  All rights reserved.
[0] MPI startup(): library kind: release
[0] MPI startup(): libfabric version: 1.10.1-impi
[0] MPI startup(): libfabric provider: psm2
[0] MPI startup(): Rank    Pid      Node name     Pin cpu
[0] MPI startup(): 0       2421200  i04r11c06s08  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,48,49,50,51,52,53,
                                    54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71}
[0] MPI startup(): 1       2421201  i04r11c06s08  {24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,72,73,74
                                    ,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95}

Starting Measurement with data-size 2^2
Data size : 2^2 (B), Time (ms) :     0.001348863, Bandwidth (MB/s) :     2.965460986


Here it can be seen that process runs on different cores on different sockets on same node.

3. and for processes on different nodes.

Executed program with command "mpiexec -genv I_MPI_DEBUG=4 -n 2" (node-to-node)

[0] MPI startup(): Intel(R) MPI Library, Version 2019 Update 8  Build 20200624 (id: 4f16ad915)
[0] MPI startup(): Copyright (C) 2003-2020 Intel Corporation.  All rights reserved.
[0] MPI startup(): library kind: release
[0] MPI startup(): libfabric version: 1.10.1-impi
[0] MPI startup(): libfabric provider: psm2
[0] MPI startup(): Rank    Pid      Node name     Pin cpu
[0] MPI startup(): 0       2017944  i04r09c05s01  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
                                    30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56
                                    ,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,8
                                    3,84,85,86,87,88,89,90,91,92,93,94,95}
[0] MPI startup(): 1       2391433  i04r10c03s10  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
                                    30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56
                                    ,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,8
                                    3,84,85,86,87,88,89,90,91,92,93,94,95}

Starting Measurement with data-size 2^2
Data size : 2^2 (B), Time (ms) :     0.003078929, Bandwidth (MB/s) :     1.299152987

Here it can be seen that process runs on different cores on different nodes.
