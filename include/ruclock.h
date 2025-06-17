#ifndef RUCLOCK_H
#define RUCLOCK_H
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include "csac_enums.h"

/** 外部程序交互的结构体 */
typedef enum {
  DISCIPLINE_START_OK,
  DISCIPLINE_START_ERR_CSAC_NOT_READY,
  DISCIPLINE_START_ERR_1PPS_NOT_READY,
  DISCIPLINE_START_ERR_SET_PHASE_FAIL,
  DISCIPLINE_START_ERR_SET_TIME_CONSTANT_FAIL,
  DISCIPLINE_START_ERR_ENABLE_FAIL
} DisciplineStartStatus;

typedef struct {
  bool success;
  DisciplineStartStatus code;
  const char* message;  // 可以前端直接弹出
} DisciplineStartResult;


/**
 * @brief 启动训练过程
 * 
 * 该函数执行训练前的准备工作并启动训练过程。具体步骤包括：
 * 1. 检查设备是否处于可执行训练的状态
 * 2. 执行1PPS同步
 * 3. 设置相位阈值和时间常数
 * 4. 启用训练模式
 * 
 * @param ns_threshold 相位阈值（纳秒）范围 [1, 255]
 * @param time_constant 训练时间常数（秒）范围 [10, 10000]
 * @return bool 训练是否成功启动
 */
DisciplineStartResult ruclock_discipliner_start_training(uint8_t ns_threshold, uint16_t time_constant);

/**
 * @brief 更新训练状态并决定是否停止训练
 *
 * 该函数通过读取遥测数据判断当前铷钟的训练状态（DiscOK 和 Phase），
 * 决定是否终止训练过程，并打印相应信息：
 *
 * - 若 DiscOK 为 1，则说明训练成功，调用 discipliner_stop_training() 停止训练。
 * - 若 DiscOK 为 2 或 Phase 漂移超过 1000ns，说明训练失败，停止训练并报告错误原因。
 * - 否则，继续训练并打印当前状态信息。
 *
 * @return true 表示训练成功，false 表示训练未成功（可能正在进行或失败）。
 */
bool ruclock_discipliner_update_training_status();




/* 频率调整*/
int get_current_frequency(int *freq);
int set_absolute_frequency(int abs_freq, int *now_freq);
int adjust_relative_frequency(int offset_freq, int *now_freq);
int latch_current_frequency();

/* 操作模式*/
int set_operating_modes(Telemetry_Modes_type type);

#endif