CC = gcc
CFLAGS = -g -Wall

OUT = disk
OBJ = common.o disk.o

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(CFLAGS) $(OBJ)

clean:
	$(RM) $(OUT) $(OBJ)
