# Intel compiler
CC =  icc
CFLAGS = -O2

MPICC = mpicc

all: heat

best: papiO3

papiO2: heat

papiO3: CFLAGS = -O3 -xhost -fno-alias
papiO3: heat

heat : heat.o input.o misc.o timing.o relax_gauss.o relax_jacobi.o
	$(CC) $(CFLAGS) $(PAPI_INC) -o $@ $+ -lm 

%.o : %.c heat.h timing.h input.h
	$(CC) $(CFLAGS) -c -o $@ $< $(PAPI_LIB)

clean:
	rm -f *.o heat *~ *.ppm

remake : clean all
