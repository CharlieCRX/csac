TARGET_NAME=ruclock
PKG_VERSION:=1.3.1
PKG_BUILD_TIME=$(shell date "+%Y%m%d")
# CC      = /home/ctool/zlg_gcc/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-gcc
CROSS_COMPILE ?= arm-sigmastar-linux-uclibcgnueabihf-12.4.0-
# 编译器选项
ifeq ($(Bd), zlg)
    CC := arm-linux-gnueabihf-gcc
else
    CC := $(CROSS_COMPILE)gcc
endif

CFLAGS  := -static -Wall -std=gnu99 -fPIC -g -I. -I./utils

# 加入 debug 宏处理
# 默认不定义 DEBUG
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

# 根目录下的源码文件
SRC_FILES := $(wildcard *.c)
# utils目录下的源码文件
UTILS_SRC_FILES := $(wildcard utils/*.c)

# 生成对应的目标文件列表
OBJ_FILES := $(patsubst %.c, %.o, $(SRC_FILES))
UTILS_OBJ_FILES := $(patsubst utils/%.c, utils/%.o, $(UTILS_SRC_FILES))
# 合并所有目标文件
ALL_OBJS := $(OBJ_FILES) $(UTILS_OBJ_FILES)

# utils目录的.o规则
utils/%.o: utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_NAME):$(ALL_OBJS)
	@echo "[build] Linking target $(TARGET_NAME)..."
	$(CC) $(CFLAGS) $(ALL_OBJS) -o $(TARGET_NAME)
	@echo "[version] Embedding version info..."
	@echo "#define COLLECT_VERSION \"$(PKG_VERSION)-build$(PKG_BUILD_TIME)\"" > version.h
	@echo "$(PKG_VERSION)-build$(PKG_BUILD_TIME)" > ruclock_ver
	@echo "[auto-clean] Removing object files..."
	@rm -f $(ALL_OBJS)

clean:
	@echo "[clean] Cleaning all build artifacts..."
	rm -f $(ALL_OBJS) $(TARGET_NAME) version.h ruclock_ver

.PHONY: clean install
