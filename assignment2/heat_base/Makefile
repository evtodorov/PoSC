# Intel compiler
CC =  icc
CFLAGS = -O2

MPICC = mpicc

all: heat

best: papiO3

papiO2: heat

papiO2_rate: CFLAGS += -DRATE
papiO2_rate: heat

papiO3: CFLAGS = -O3 -xhost -fno-alias
papiO3: heat

papiO3_rate: CFLAGS = -O3 -xhost -fno-alias -DRATE
papiO3_rate: heat

heat : heat.o input.o misc.o timing.o relax_gauss.o relax_jacobi.o
	$(CC) $(CFLAGS) -o $@ $+ -lm $(PAPI_LIB)

%.o : %.c heat.h timing.h input.h
	$(CC) $(CFLAGS) $(PAPI_INC) -c -o $@ $< 

clean:
	rm -f *.o heat *~ *.ppm

remake : clean all
