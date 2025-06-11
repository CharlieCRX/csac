#include "mock_csac_interfac.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "csac_macros.h"
// 定义字段名数组（按你提供的顺序）
const char *field_keys[] = {
  "Status", "Alarm", "SN", "Mode", "Contrast",
  "laserI", "tcxo", "heatP", "Sig", "temp",
  "steer", "aTune", "phase", "disOK",
  "TOD", "LTime", "Ver"
};

// 保存测试设置的模拟响应
static char mock_telemetry_response[MAX_RESPONSE_LENGTH] = "0,0x0000,2203CS77980,0x0000,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.10";
static char mock_1PPS_sync_response[MAX_RESPONSE_LENGTH] = "S";
static char mock_time_constant_response[MAX_RESPONSE_LENGTH] = "10";
static char mock_phase_threshold_response[MAX_RESPONSE_LENGTH] = "20";

int csac_send_command(const char *command, char *response)
{
    // 模拟返回数据（可按不同命令做匹配）
    if (strcmp(command, TELEMETRY_DATA) == 0) {
      strcpy(response, mock_telemetry_response);
      return strlen(mock_telemetry_response);  // 返回 response 长度;
    }

    // 1PPS同步
    else if (strcmp(command, SYNC_1PPS_MANUAL) == 0) {
      strcpy(response, mock_1PPS_sync_response);
      return strlen(mock_1PPS_sync_response);
    }

    // 查询时间常数
    else if (strcmp(command, DISC_QUERY_TIME_CONST) == 0) {
      strcpy(response, mock_time_constant_response);
      return strlen(mock_time_constant_response);
    }
    
    // D100, D300, D999 等模拟设置训练时间常数
    else if (command[0] == 'D') {
      char src[MAX_RESPONSE_LENGTH];
      strcpy(src, command);

      int seconds = atoi(&src[1]);  // 提取数字部分
      snprintf(response, MAX_RESPONSE_LENGTH, "%d", seconds);

      strncpy(mock_time_constant_response, response, MAX_RESPONSE_LENGTH - 1);
      return strlen(mock_time_constant_response);
    }

    // 查询相位阈值
    else if (strcmp(command, DISC_QUERY_PHASE_THRESHOLD) == 0) {
      strcpy(response, mock_phase_threshold_response);
      return strlen(mock_phase_threshold_response);
    }

    // m10, m100, m200, 等模拟设置相位阈值
    else if (command[0] == 'm') {
      char src[MAX_RESPONSE_LENGTH];
      strcpy(src, command);
      int ns_threshold = atoi(&src[1]);  // 提取数字部分
      snprintf(response, MAX_RESPONSE_LENGTH, "%d", ns_threshold);

      strncpy(mock_phase_threshold_response, response, MAX_RESPONSE_LENGTH - 1);
      return strlen(mock_phase_threshold_response);
    }
    return -1;
}


void mock_set_telemetry_response(const char *resp) {
  strncpy(mock_telemetry_response, resp, MAX_RESPONSE_LENGTH - 1);
  mock_telemetry_response[MAX_RESPONSE_LENGTH - 1] = '\0';
}


void mock_set_1PPS_sync_response(const char *resp) {
  strncpy(mock_1PPS_sync_response, resp, MAX_RESPONSE_LENGTH - 1);
  mock_1PPS_sync_response[MAX_RESPONSE_LENGTH - 1] = '\0';
}