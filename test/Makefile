CC = gcc
CFLAGS = -Wall -I../ -I. -I../discipline -DUNIT_TEST

SRCS = ../discipline/discipliner.c 	../ruclock_utils.c	test_discipliner.c test_ruclock_utils.c mock_ruclock.c test_main.c 
OBJS = $(SRCS:.c=.o)
TARGET = test_ruclock

all: $(TARGET)
	@ rm -f $(OBJS) # 清理中间文件
	@ ./$(TARGET)  # 编译后立即运行

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)