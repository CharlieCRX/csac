#ifndef DISCIPLINER_H
#define DISCIPLINER_H
#include <stdbool.h>
#include <stdint.h>
/**
 * @brief 检查是否可以进行驯服
 * 满足的条件：
 *   1. 铷钟状态稳定
 *   2. 存在GPS 1PPS输入 
 * @return true = 可以 ，false = 不可以
 */
bool discipliner_is_ready_to_execute();

/**
 * @brief 启用或禁用 GPS disciplining 模式
 * @param enable true = 启用 (发送 !M=0x0010)，false = 禁用 (!M=0x0000)
 */
bool discipliner_enable(bool on);


/**
 * @brief 设置 disciplining 的时间常数（单位：秒）
 * @param seconds 范围 [10, 10000]
 */
bool discipliner_set_time_constant(uint16_t sec);

/**
 * @brief 获取 disciplining 的时间常数（单位：秒）
 * @return seconds 范围 [10, 10000]
 */
uint16_t discipliner_get_time_constant();


/**
 * @brief 设置判断1PPS同步成功的相位阈值（单位：ns）
 * @param ns_threshold 范围 [1, 255]
 */
bool discipliner_set_phase_threshold(uint8_t ns);


/**
 * @brief 获取1PPS同步成功的相位阈值（单位：ns）
 * @return ns_threshold 范围 [1, 255]
 */
uint8_t discipliner_get_phase_threshold();


bool discipliner_force_reinitialize();  // 强制重新开始 discipline（用于信号源恢复）
#endif