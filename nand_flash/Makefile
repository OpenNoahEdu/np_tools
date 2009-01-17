CC     = gcc

#specify your custmized compile flags here
CFLAGS  = -Wall  -g -I ../
LDFLAGS = -lconfuse

CFLAGS += -o3



pavo_nandflash:pavo_nandflash.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


clean:
	rm -rf pavo_nandflash pavo_nandflash.o 
