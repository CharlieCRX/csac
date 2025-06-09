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

CFLAGS  := -static -Wall -std=gnu99 -fPIC -g -I.

# 加入 debug 宏处理
# 默认不定义 DEBUG
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

SRC_FILES := $(wildcard *.c)
OBJ_FILES := $(patsubst %.c, %.o, $(SRC_FILES))

$(TARGET_NAME):$(OBJ_FILES)
	@echo "[build] Linking target $(TARGET_NAME)..."
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(TARGET_NAME)
	@echo "[version] Embedding version info..."
	@echo "#define COLLECT_VERSION \"$(PKG_VERSION)-build$(PKG_BUILD_TIME)\"" > version.h
	@echo "$(PKG_VERSION)-build$(PKG_BUILD_TIME)" > ruclock_ver
	@echo "[auto-clean] Removing object files..."
	@rm -f $(OBJ_FILES)

clean:
	@echo "[clean] Cleaning all build artifacts..."
	rm -f $(OBJ_FILES) $(TARGET_NAME) version.h ruclock_ver

.PHONY: clean install
