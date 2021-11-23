## Task 1

## Task 2

### Setup

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

### Solutions

#### 2.1
![Graph 1](Graph_1.png)

#### 2.2
![Graph 2](Graph_2.png)


#### 2.3
![Graph 3](Graph_3.png)

### 2.4

The MFLOP rates computed by the program and HW counters are very similar. Small differences are observed for the smallest resolution. A possible explanations stems from the fact that the hardware counter initialization is after the start of the software clock, which results in a slight overhead which is noticeable in the lowest resolution.

#### 2.5

#### 2.6 
