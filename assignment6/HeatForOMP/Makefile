CC =  icc
CFLAGS = -O3 -xCORE-AVX512 -qopt-zmm-usage=high -fno-alias  

MPICC = mpicc

all: heat 

heat : heat.o input.o misc.o timing.o relax_jacobi.o
	$(MPICC) $(CFLAGS) -o heat $+ -lm  

%.o : %.c %.h
	$(MPICC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o heat *~ *.ppm

remake : clean all
