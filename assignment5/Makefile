CC =  mpicc
CFLAGS = -O3

all: pingpong 

pingpong : pingpong.o
	$(CC) $(CFLAGS) -o pingpong $+ -lm  

%.o : %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o pingpong *~

remake : clean all
