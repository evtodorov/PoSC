To build the code with PAPI and compiler flags `-O2`:

```
[module load papi] #if not done
make clean
make papiO2
```

To build the code with PAPI and compiler flags `-O3 -xhost -fno-alias`:

```
[module load papi] #if not done
make clean
make papiO3
```
