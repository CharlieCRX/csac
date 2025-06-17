#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <mock_csac_interface.h>
#include <assert.h>
#include <stdbool.h>
#include <csac_macros.h>
#include <csac_enums.h>

void test_mode_operations() {
  char response[MAX_RESPONSE_LENGTH];
  
  // 初始状态
  assert(csac_send_command("M?", response) > 0);
  assert(strcmp(response, "0x0000") == 0);
  
  // 设置模拟调谐
  assert(csac_send_command("MA", response) > 0);
  assert(strcmp(response, "0x0001") == 0);
  
  // 设置1PPS相位测量
  assert(csac_send_command("MM", response) > 0);
  assert(strcmp(response, "0x0005") == 0); // 0x0001 + 0x0004
  
  // 切换到Discipline模式（应清除M模式）
  assert(csac_send_command("MD", response) > 0);
  assert(strcmp(response, "0x0011") == 0); // 只保留D模式和A模式
  
  // 模拟通信失败
  mock_set_comm_failure(true);
  assert(csac_send_command("MU", response) == -1);
  mock_set_comm_failure(false);
  
  // 验证遥测中的模式字段
  assert(csac_send_command("M?", response) > 0);
  assert(strstr(response, "0x0011") != NULL);
  
  // 重置模拟器
  mock_reset();
  assert(csac_send_command("M?", response) > 0);
  assert(strcmp(response, "0x0000") == 0);

  printf("✅ UC1: test_mode_operations: PASS\n");
}


void test_all_mock_command() {
  printf("\n====================[ Mock interface TEST START ]====================\n");
  test_mode_operations();
  printf("====================[ Mock interface TEST PASSED ]====================\n");
}