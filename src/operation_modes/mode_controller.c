#include "mode_controller.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "enums.h"
#include "csac_macros.h"

#ifdef UNIT_TEST
#include <mock_csac_interface.h>
#else
#include <csac_interface.h>
#endif
int set_mode(E_CSAC_operating_modes mode, bool enable, uint16_t* new_modes){
  char cmd[8] = {0};
  char response[MAX_RESPONSE_LENGTH] = {0};
  // 构造命令
  switch (mode) {
    case MODE_ANALOG_TUNING:          strcpy(cmd, enable ? "MA" : "Ma"); break;
    case MODE_1PPS_PHASE_MEASUREMENT: strcpy(cmd, enable ? "MM" : "Mm"); break;
    case MODE_1PPS_AUTO_SYNC:         strcpy(cmd, enable ? "MS" : "Ms"); break;
    case MODE_DISCIPLINE:             strcpy(cmd, enable ? "MD" : "Md"); break;
    case MODE_ULTRA_LOW_POWER:        strcpy(cmd, enable ? "MU" : "Mu"); break;
    case MODE_CHECKSUM_REQUIRED:      strcpy(cmd, enable ? "MC" : "Mc"); break; // TODO Command: !Mc*2E[CRLF]
    default: return false;
  }
  // 发送命令并获取响应
  int response_length = csac_send_command(cmd, response);
  if (response_length < 0) {
    return -1; // 通信失败
  }
  // 解析响应 例如'?'等
  if (response_length < 5 || response[0] != '0' || response[1] != 'x') {
    return -2; // 响应格式错误
  }

  // 提取新模式
  uint16_t new_mode = 0;
  if (sscanf(response, "0x%04hx", &new_mode) != 1) {
    return -2; // 解析失败
  }
  // 检查是否设置成功
  bool success = enable ? (new_mode & mode) : !(new_mode & mode);
  if (success) {
    *new_modes = new_mode; // 返回新模式
    return 0; // 成功
  } else {
    return -3; // 设置未生效
  }
}
int query_modes(uint16_t* modes) {
  char response[MAX_RESPONSE_LENGTH] = {0};
  // 发送查询命令
  int response_length = csac_send_command("M?", response);
  if (response_length < 0) {
    return -1; // 通信失败
  }
  // 解析响应
  if (response_length < 5 || response[0] != '0' || response[1] != 'x') {
    return -2; // 响应格式错误
  }
  
  // 提取当前模式
  uint16_t current_modes = 0;
  if (sscanf(response, "0x%04hx", &current_modes) != 1) {
    return -2; // 解析失败
  }
  
  *modes = current_modes; // 返回当前模式
  return 0; // 成功
}