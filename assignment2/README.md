To build the code with PAPI and compiler flags `-O2`:

```
[module load papi] #if not done
make clean
make papiO2 # to measure using flags for HL interface
# or
make papiO2_rate # to measure using rate functions
```

To build the code with PAPI and compiler flags `-O3 -xhost -fno-alias`:

```
[module load papi] #if not done
make clean
make papiO3 # to measure using flags for HL interface
# or
make papiO3_rate # to measure using rate function
```

To execute on SUPERMUC to measure with flags use:
```
sbatch job_cache.scp # for cache misses
# or 
sbatch job_ops.scp # for FLOPS
```

For measuring using the rate functions, either .scp can be used.


