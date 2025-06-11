TARGET_NAME := ruclock
PKG_VERSION := 1.3.1
PKG_BUILD_TIME = $(shell date "+%Y%m%d")

# 编译器配置
# CC      = /home/ctool/zlg_gcc/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-gcc
CROSS_COMPILE ?= arm-sigmastar-linux-uclibcgnueabihf-12.4.0-
ifeq ($(Bd), zlg)
    CC := arm-linux-gnueabihf-gcc
else
    CC := $(CROSS_COMPILE)gcc
endif

# 编译器选项
CFLAGS  := -static -Wall -std=gnu99 -fPIC -g
CFLAGS += -I./include -I./lib/comm_protocol

# 加入 debug 宏处理
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

# 源文件路径
SRC_DIR = src
LIB_DIR = lib/comm_protocol

# 查找所有源文件
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
UTILS_SRC := $(wildcard $(SRC_DIR)/utils/*.c)
TELEMETRY_SRC := $(wildcard $(SRC_DIR)/telemetry/*.c)
COMM_PROTOCOL_SRC := $(wildcard $(LIB_DIR)/*.c)

# 所有源文件列表
ALL_SRC_FILES := $(SRC_FILES) $(UTILS_SRC) $(COMM_PROTOCOL_SRC) $(TELEMETRY_SRC)

# 生成对应的目标文件列表
OBJ_FILES := $(patsubst %.c, %.o, $(ALL_SRC_FILES))

# 构建目标
$(TARGET_NAME): $(OBJ_FILES)
	@echo "[build] Linking target $(TARGET_NAME)..."
	$(CC) $(CFLAGS) $^ -o $@
	@echo "[version] Embedding version info..."
	@echo "[build] Target $(TARGET_NAME) built successfully!"
	@rm -f $(OBJ_FILES)
	@echo "[build] Cleaning up object files..."
	@echo "[build] Build complete."

# 模式规则编译所有源文件
%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[clean] Cleaning all build artifacts..."
	rm -f $(OBJ_FILES) $(TARGET_NAME)
	rm -f test/$(TARGET_NAME) test/*.o

.PHONY: clean install