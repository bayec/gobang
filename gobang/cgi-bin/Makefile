CC = g++
OBJ += gobang.o
TARGET = gobang

CFLAGS = -Wall
CFLAGS += -lpthread
CFLAGS += -static

$(TARGET):$(OBJ)  
	$(CC) $^ -o $@ $(CFLAGS) 

%.o:%.c
	$(CC) -c $^ -o $@ $(CFLAGS)

.PHONY:clean
clean:
	rm gobang *.o -rfv	


