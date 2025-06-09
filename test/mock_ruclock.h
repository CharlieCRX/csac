#ifndef MOCK_RUCLOCK_H
#define MOCK_RUCLOCK_H
#include <stdint.h>
#include <stdio.h>
#include "enums.h"

#define ERR_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define DEBUG_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define	MAX_REBUFF      135
#define	MAX_COMMAND_LENGTH  500	// 最大命令长度
#define	MAX_RESPONSE_LENGTH 500 // 最大响应长度
#define	COMMAND_LENGTH 20  // 命令字符串最大长度
#define	RESPONSE_LENGTH 20 // 单位响应字符串最大长度
#define	MAX_FIELDS 17  //接收‘>’里面数据的长度

#define	TELEMETRY_DATA          "!^\r\n"
#define SYNC_1PPS								"!S\r\n"
#define MAX_RESPONSE_SIZE 256

int WRITE_READ_RUCLOCK(const char *command, char *response);
int get_telemetry_data(T_ruclock_telemetry *telemetry);


// moc
void mock_set_telemetry_response(const char *resp);
void mock_set_1PPS_sync_response(const char *resp);
#endif