#ifndef RUCLOCK_H
#define RUCLOCK_H
#include <stdint.h>
#include <pthread.h>
#include "enums.h"

#ifdef DEBUG
#define ERR_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define DEBUG_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define ERR_LOG(fmt, ...) do {} while (0)
#define DEBUG_LOG(fmt, ...) do {} while (0)
#endif
#define STRING_CONCAT(x, y) x ## y

#define	UART2           2
#define	TIMEOUT_SECONDS 5 // 超时时间，单位为秒
#define	CRLF			  "\n\n"
#define	MAX_REBUFF      135
#define	MAX_COMMAND_LENGTH  500	// 最大命令长度
#define	MAX_RESPONSE_LENGTH 500 // 最大响应长度
#define	COMMAND_LENGTH 20  // 命令字符串最大长度
#define	RESPONSE_LENGTH 20 // 单位响应字符串最大长度
#define	MAX_FIELDS 17  //接收‘>’里面数据的长度

#define	TELEMETRY_HEAD          "!6\r\n"
#define	TELEMETRY_DATA          "!^\r\n"
#define	TELEMETRY_SET_FREQ_A    "FA"
#define	TELEMETRY_SET_FREQ_D    "FD"
#define	TELEMETRY_
#define	TELEMETRY_GET_FREQ      "!F?\r\n"
#define	TELEMETRY_LATCH_FREQ    "!FL\r\n"
#define	TELEMETRY_LATCH_DC      "!DCL\r\n"


#define TELEMETRY_SYNC          "!S\r\n"
#define TELEMETRY_HELP          "!?\r\n"



// for new function
#define SYNC_1PPS								"!S\r\n"
#define QUERY_1PPS_TIME_CONST		"!D?\r\n"
#define QUERY_1PPS_CABLE_LEN		"!DC?\r\n"
#define LATCH_1PPS_CABLE_LEN		"!DCL\r\n"

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

/* 1PPS */
int synchronize_1pps_interface();
int set_1pps_disciplining_time_constant(unsigned int seconds);
int query_1pps_disciplining_time_constant();
int set_1pps_cable_length_compensation(int compensation_in_100ps);
int query_1pps_cable_length_compensation();
int latch_1pps_compensation_to_powerup_default();

#endif