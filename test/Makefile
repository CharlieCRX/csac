CC = gcc
CFLAGS = -Wall -I../ -I. -I../discipline -DUNIT_TEST

SRCS = ../discipline/discipliner.c test_main.c mock_ruclock.c test_discipliner.c
OBJS = $(SRCS:.c=.o)
TARGET = test_ruclock

all: $(TARGET)
	 ./$(TARGET)  # 编译后立即运行

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f *.o $(TARGET)