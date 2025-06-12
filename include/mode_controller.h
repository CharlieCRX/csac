#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "enums.h"

// 设置模式：true 启用，false 禁用
bool set_mode(E_CSAC_operating_modes mode, bool enable);

// 查询当前启用模式位
uint16_t query_current_modes(void);

// 解析当前模式位中是否启用了指定模式
bool is_mode_enabled(uint16_t mode_flags, E_CSAC_operating_modes mode);

#endif