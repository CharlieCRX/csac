#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "enums.h"

/**
 * @brief 设置指定模式的状态
 * 
 * @param mode 要设置的模式（单个模式位）
 * @param enable true=使能, false=禁用
 * @param[out] new_modes 设置成功后设备返回的新模式状态
 * @return int 0=成功, -1=通信失败, -2=解析失败, -3=设置未生效
 */
int set_mode(E_CSAC_operating_modes mode, bool enable, uint16_t* new_modes);

/**
 * @brief 查询当前所有模式的状态
 * 
 * @param[out] modes 当前模式位图
 * @return int 0=成功, -1=通信失败, -2=解析失败
 */
int query_modes(uint16_t* modes);

#endif