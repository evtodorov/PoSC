# Hybrid Parallelization of Heat Transfer Simulation

## Compilation

Compiling the application is different for MPI-only or MPI+OpenMP hybrid. The hybrid version can  also be run with `OMP_NUM_THREADS=1`, but this needs to be set explicitly. In the MPI-only mode, no omp parallelization is used,

### MPI-only mode

```
make clean
make heat
```

### Hybrid mode

```
make clean
make hybrid
```

## Usage

The example SLURM job configuration file job.scp illustrates the optimal way of running the application. Testing has been carried out using `mpiexec`, running with `srun` might produce different timings due to different allocation of processes between nodes and cores.

```
mpiexec -genv OMP_NUM_THREADS=$NUM_THREADS -genv I_MPI_PIN_DOMAIN=omp -n $NUM_DOMAINS ./heat $FILENAME $NUM_ROWS $NUM_COLS
```

* In the MPI-only mode, `OMP_NUM_THREADS` is ignored. The hybrid mode also supports `OMP_NUM_THREADS=1`, but this needs to be specified explicitly.
* `NUM_DOMAINS = NUM_ROWS * NUM_COLS`
* For oversubscription, `--ntasks` in the SLURM job config file can be different than `NUM_DOMAINS` (maximum `--ntasks` is 48x number of nodes), otherwise use `--ntasks=$NUM_DOMAINS` 
* The number of nodes is configure via `--nodes` in the SLURM job config file
