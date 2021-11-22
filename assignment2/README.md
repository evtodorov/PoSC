To build the code with PAPI and compiler flags `-O2`:

```
[module load papi] #if not done
make clean
make papiO2 # to measure cache misses
# or
make papiO2_rate # to measure FLOPS
```

To build the code with PAPI and compiler flags `-O3 -xhost -fno-alias`:

```
[module load papi] #if not done
make clean
make papiO3 # to measure cache misses
# or
,ale papiO3_rate # to measure FLOPS
```
