CC = gcc
CFLAGS = -Wall -I../include -I../lib/comm_protocol -I../src -I./mocks -DUNIT_TEST
BUILD_DIR = build

# 所有源文件
TEST_SRCS = \
    unit/test_discipliner.c \
    unit/test_ruclock_utils.c \
    integration/test_main.c

MOCK_SRCS = \
    mocks/mock_csac_interfac.c

SRC_SRCS = \
    ../src/discipline/discipliner.c \
    ../src/utils/csac_utils.c

ALL_SRCS = $(TEST_SRCS) $(MOCK_SRCS) $(SRC_SRCS)

# 转换为构建目录下的 .o 文件
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(filter-out ../%,$(ALL_SRCS))) \
       $(patsubst ../src/%.c, $(BUILD_DIR)/src/%.o, $(filter ../src/%, $(ALL_SRCS)))

TARGET = test_ruclock

all: $(TARGET)
	@echo "[test] Running tests..."
	@./$(TARGET)
	@echo "[test] Tests completed"

$(TARGET): $(OBJS)
	@echo "[test] Linking $@..."
	@$(CC) -o $@ $^

# 规则：普通 test/ 下的文件，如 unit/, integration/, mocks/
$(BUILD_DIR)/%.o: %.c
	@echo "[test] Compiling $< -> $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# 规则：映射 ../src/... → build/src/...
$(BUILD_DIR)/src/%.o: ../src/%.c
	@echo "[test] Compiling $< -> $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[test] Cleaning build/ and binary..."
	@rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean