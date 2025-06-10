# test/Makefile
CC = gcc
CFLAGS = -Wall -I../include -I../lib/comm_protocol -I../src -I. -DUNIT_TEST

# 源文件路径
TEST_SRCS = \
    test_discipliner.c \
    test_ruclock_utils.c \
    test_main.c

MOCK_SRCS = mock_csac_interfac.c

SRC_SRCS = \
    ../src/discipline/discipliner.c \
    ../src/utils/csac_utils.c

# 所有源文件
ALL_SRCS = $(TEST_SRCS) $(MOCK_SRCS) $(SRC_SRCS)

# 生成目标文件列表
OBJS = $(patsubst %.c, %.o, $(ALL_SRCS))
TARGET = test_ruclock

all: $(TARGET)
	@echo "[test] Running tests..."
	@./$(TARGET)
	@echo "[test] Tests completed"
	@echo "[test] Cleaning intermediate object files..."
	@rm -f $(OBJS) ../src/discipline/*.o ../src/utils/*.o

$(TARGET): $(OBJS)
	@echo "[test] Linking test executable..."
	@$(CC) -o $@ $^

# 通用编译规则
%.o: %.c
	@echo "[test] Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# 特殊处理不同目录的源文件
../src/discipline/%.o: ../src/discipline/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

../src/utils/%.o: ../src/utils/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[test] Cleaning test artifacts..."
	@rm -f $(OBJS) $(TARGET)
	@rm -f ../src/discipline/*.o ../src/utils/*.o

.PHONY: all clean