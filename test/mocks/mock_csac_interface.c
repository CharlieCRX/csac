#include "mock_csac_interface.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
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

// 添加模式控制相关变量
static uint16_t current_modes = MODE_NONE; // 当前模式状态
static bool mock_comm_failure = false;     // 模拟通信失败
static bool mock_parse_failure = false;    // 模拟解析失败
static uint16_t mock_failure_mask = 0;     // 模拟设置失败的模式掩码

int csac_send_command(const char *command, char *response)
{
  // 如果模拟通信失败，直接返回错误
  if (mock_comm_failure) {
    return -1;
  }
  
  // 如果模拟解析失败，返回无效响应
  if (mock_parse_failure) {
    strcpy(response, "?");
    return strlen(response);
  }


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
  
  
  // 模拟模式配置命令
  else if (command[0] == 'M') {
    char cmd = command[1];
    uint16_t new_mode = current_modes;
    
    // 处理命令
    switch(cmd) {
      // 使能命令 (大写)
      case 'A': 
        if (!(mock_failure_mask & MODE_ANALOG_TUNING))
          new_mode |= MODE_ANALOG_TUNING; 
        break;
      case 'M': 
        if (!(mock_failure_mask & MODE_1PPS_PHASE_MEASUREMENT)) {
          new_mode &= ~MUTEX_MODES_MASK;
          new_mode |= MODE_1PPS_PHASE_MEASUREMENT;
        }
        break;
      case 'S': 
        if (!(mock_failure_mask & MODE_1PPS_AUTO_SYNC)) {
          new_mode &= ~MUTEX_MODES_MASK;
          new_mode |= MODE_1PPS_AUTO_SYNC;
        }
        break;
      case 'D': 
        if (!(mock_failure_mask & MODE_DISCIPLINE)) {
          new_mode &= ~MUTEX_MODES_MASK;
          new_mode |= MODE_DISCIPLINE;
        }
        break;
      case 'U': 
        if (!(mock_failure_mask & MODE_ULTRA_LOW_POWER))
          new_mode |= MODE_ULTRA_LOW_POWER; 
        break;
      case 'C': 
        if (!(mock_failure_mask & MODE_CHECKSUM_REQUIRED))
          new_mode |= MODE_CHECKSUM_REQUIRED; 
        break;
      
      // 禁用命令 (小写)
      case 'a': 
        if (!(mock_failure_mask & MODE_ANALOG_TUNING))
          new_mode &= ~MODE_ANALOG_TUNING; 
        break;
      case 'm': 
        if (!(mock_failure_mask & MODE_1PPS_PHASE_MEASUREMENT))
          new_mode &= ~MODE_1PPS_PHASE_MEASUREMENT; 
        break;
      case 's': 
        if (!(mock_failure_mask & MODE_1PPS_AUTO_SYNC))
          new_mode &= ~MODE_1PPS_AUTO_SYNC; 
        break;
      case 'd': 
        if (!(mock_failure_mask & MODE_DISCIPLINE))
          new_mode &= ~MODE_DISCIPLINE; 
        break;
      case 'u':
        if (!(mock_failure_mask & MODE_ULTRA_LOW_POWER))
          new_mode &= ~MODE_ULTRA_LOW_POWER; 
        break;
      case 'c':
        // TODO disable checksum through mode register -> Command: !Mc*2E[CRLF]
        if (!(mock_failure_mask & MODE_CHECKSUM_REQUIRED))
          new_mode &= ~MODE_CHECKSUM_REQUIRED; 
        break;
      case '?':
        // 查询当前模式
        snprintf(response, MAX_RESPONSE_LENGTH, "0x%04X", current_modes);
        return strlen(response);
      
      default: 
      // 无效命令
      strcpy(response, "?");
      return strlen(response);
    }
    
    // 更新当前模式
    current_modes = new_mode;
    
    // 返回更新后的模式
    snprintf(response, MAX_RESPONSE_LENGTH, "0x%04X", current_modes);
    return strlen(response);
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


void mock_set_mode_response(const char *resp) {
  // 直接设置当前模式值
  if (strncmp(resp, "0x", 2) == 0) {
    current_modes = (uint16_t)strtoul(resp, NULL, 16);
  }
}

// 设置模拟通信失败
void mock_set_comm_failure(bool fail) {
  mock_comm_failure = fail;
}

// 设置模拟解析失败
void mock_set_parse_failure(bool fail) {
  mock_parse_failure = fail;
}

// 设置模拟操作失败的模式掩码
void mock_set_operation_failure_mask(uint16_t mask) {
  mock_failure_mask = mask;
}

// 获取当前模拟的模式状态
uint16_t mock_get_current_modes(void) {
  return current_modes;
}


// 重置模拟器状态
void mock_reset(void) {
  current_modes = MODE_NONE;
  mock_comm_failure = false;
  mock_parse_failure = false;
  mock_failure_mask = 0;
  
  // 重置默认响应
  strcpy(mock_telemetry_response, "0,0x0000,2203CS77980,0x0000,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.10");
  strcpy(mock_1PPS_sync_response, "S");
  strcpy(mock_time_constant_response, "10");
  strcpy(mock_phase_threshold_response, "20");
}