CC     = gcc

CFLAGS  = -Wall -O3
LDFLAGS = 

mknandflash: mknandflash.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf mknandflash mknandflash.o 
