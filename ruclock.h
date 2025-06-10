#ifndef RUCLOCK_H
#define RUCLOCK_H
#include <stdint.h>
#include <pthread.h>
#include "enums.h"
int ruclock_init();
int get_current_value(const char *command, char *response);

// 比较字符串末尾两个字符和 CRLF 的函数
int is_last_two_chars_crlf(const char* str, int len);
int remove_end_crlf_in_string(const char *src, int len, char *dest);

/**				接口信息     */
/* 遥测数据*/
int get_telemetry_data(T_ruclock_telemetry *telemetry);
int get_telemetry_headers(char *telemetry_name);

/* 频率调整*/
int get_current_frequency(int *freq);
int set_absolute_frequency(int abs_freq, int *now_freq);
int adjust_relative_frequency(int offset_freq, int *now_freq);
int latch_current_frequency();

/* 操作模式*/
int set_operating_modes(Telemetry_Modes_type type);

#endif