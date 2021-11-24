# PoS_21_group12

Assignments for Programming of Supercomunter WiSe 21-22 - Group 12

2.1 Gprof

Compile with "-pg" for use with gprof. Run the application normally. Application run will produce a file "gmon.out". 

View profiling results with "gprof ./heat gmon.out". 

1. Copy the output of gprof (relevant parts) into your git repo. 

	Refer directory "gprof" inside "pos_21_group12/assignment2/heat_base"

2. Which functions take most of time?

	The functions that take most of the time are "relax_jacobi" and "residual_jacobi"

3. Is the measured execution time of the application affected by gprof?

	It will run a bit slower than normal because it spents time in collecting and writing the data. For the present task, it had a very minor impact on the performance.

4. Can gprof analyze the loops (for, while, do-while, etc.) of the application?

	The application must be compiled with "-g" and "-a" in addition to "-pg" to count how many times it executed each iteration of each 'for', 'while', 'do-while' loop.

5. Is gprof capable of analyzing parallel applications?

	gprof is poorly suited for parallel applications. There is a possibility to use it in MPI applications as per LRZ documentation (https://www.lrz.de/services/compute/supermuc/tuning/gprof/)

6. Compile the application using gcc with -O3 in CFLAGS (first do module unload intel-mpi/2019-intel then module unload intel. Then load gcc using module load gcc. use gcc only in this step. For all the other tasks, you should only use icc). Run the application, generate and save the profiling output. Similarly, Recompile with icc and -O3 Flag (Do a make clean and make). Run the application and generate the output again. Did you see any change in output?

	With gcc compiler, only Flat Profile is listed under output.
	With icc compiler, output contains both Flat Profile and Call Graph