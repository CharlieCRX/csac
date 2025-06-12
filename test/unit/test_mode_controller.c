#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <mock_csac_interface.h>
#include <mode_controller.h>
#include "enums.h"

void test_set_mode_success() {
  uint16_t new_modes;
  
  // 测试使能非互斥模式
  assert(set_mode(MODE_ANALOG_TUNING, true, &new_modes) == 0);
  assert(new_modes == MODE_ANALOG_TUNING);
  
  // 测试使能互斥模式
  assert(set_mode(MODE_1PPS_PHASE_MEASUREMENT, true, &new_modes) == 0);
  assert(new_modes == (MODE_1PPS_PHASE_MEASUREMENT | MODE_ANALOG_TUNING));
  
  // 测试禁用模式
  assert(set_mode(MODE_ANALOG_TUNING, false, &new_modes) == 0);
  assert(new_modes == MODE_1PPS_PHASE_MEASUREMENT);
  
  // 测试互斥模式自动清除
  assert(set_mode(MODE_DISCIPLINE, true, &new_modes) == 0);
  assert(new_modes == MODE_DISCIPLINE);  // 应清除前一个互斥模式

  assert(set_mode(MODE_1PPS_PHASE_MEASUREMENT, true, &new_modes) == 0);
  assert(new_modes == MODE_1PPS_PHASE_MEASUREMENT);  // 应清除前一个互斥模式

  printf("✅ UC1: test_set_mode_success: PASS\n");
}


void test_set_mode_errors() {
  uint16_t new_modes;
  
  // 通信失败
  mock_set_comm_failure(true);
  assert(set_mode(MODE_ANALOG_TUNING, true, &new_modes) == -1);
  mock_set_comm_failure(false);
  
  // 解析失败
  mock_set_mode_response("?");
  assert(set_mode(MODE_ULTRA_LOW_POWER, true, &new_modes) == -2);
  mock_set_mode_response("0x0000"); // 恢复默认
  
  // 设置未生效
  mock_set_operation_failure_mask(MODE_ANALOG_TUNING);
  int ret = set_mode(MODE_ANALOG_TUNING, true, &new_modes);
  assert(ret == -3);
  assert(new_modes == MODE_NONE); // 设备返回的状态
  mock_set_operation_failure_mask(MODE_NONE); // 恢复
  
  // 无效模式位
  assert(set_mode(0x0002, true, &new_modes) == -2);
  assert(set_mode(0x00FF, true, &new_modes) == -2);

  printf("✅ UC2: test_set_mode_errors: PASS\n");
}


void test_query_modes_success() {
  uint16_t modes;
  
  // 初始状态查询
  assert(query_modes(&modes) == 0);
  assert(modes == MODE_NONE);
  
  // 设置后查询
  set_mode(MODE_ULTRA_LOW_POWER, true, &modes);
  assert(query_modes(&modes) == 0);
  assert(modes == MODE_ULTRA_LOW_POWER);
  
  // 组合模式查询
  set_mode(MODE_ANALOG_TUNING, true, &modes);
  set_mode(MODE_CHECKSUM_REQUIRED, true, &modes);
  assert(query_modes(&modes) == 0);
  assert(modes == (MODE_ANALOG_TUNING | MODE_ULTRA_LOW_POWER | MODE_CHECKSUM_REQUIRED));

  printf("✅ UC3: test_query_modes_success: PASS\n");
}


void test_query_modes_errors() {
  uint16_t modes;
  
  // 通信失败
  mock_set_comm_failure(true);
  assert(query_modes(&modes) == -1);
  mock_set_comm_failure(false);
  
  // 解析失败
  mock_set_mode_response("00s1111");
  assert(query_modes(&modes) == -2);
  mock_set_mode_response("0x0000"); // 恢复默认

  printf("✅ UC4: test_query_modes_errors: PASS\n");
}



void test_mutex_modes() {
  mock_reset();
  uint16_t new_modes;
  
  // 互斥模式设置顺序测试
  assert(set_mode(MODE_1PPS_PHASE_MEASUREMENT, true, &new_modes) == 0);
  assert(new_modes == MODE_1PPS_PHASE_MEASUREMENT);
  
  assert(set_mode(MODE_1PPS_AUTO_SYNC, true, &new_modes) == 0);
  assert(new_modes == MODE_1PPS_AUTO_SYNC); // 应覆盖前一个模式
  
  // 禁用互斥模式后设置另一个
  assert(set_mode(MODE_1PPS_AUTO_SYNC, false, &new_modes) == 0);
  assert(set_mode(MODE_DISCIPLINE, true, &new_modes) == 0);
  assert(new_modes == MODE_DISCIPLINE);
  
  // 互斥模式与非互斥模式共存
  assert(set_mode(MODE_ULTRA_LOW_POWER, true, &new_modes) == 0);
  assert(new_modes == (MODE_DISCIPLINE | MODE_ULTRA_LOW_POWER));

  printf("✅ UC5: test_mutex_modes: PASS\n");
}


void test_boundary_conditions() {
  mock_reset();
  uint16_t new_modes;
  
  // 重复设置相同状态
  assert(set_mode(MODE_CHECKSUM_REQUIRED, true, &new_modes) == 0);
  assert(set_mode(MODE_CHECKSUM_REQUIRED, true, &new_modes) == 0); // 应成功
  assert(new_modes & MODE_CHECKSUM_REQUIRED);
  
  // 禁用未启用的模式
  assert(set_mode(MODE_ANALOG_TUNING, false, &new_modes) == 0); // 应成功
  assert(!(new_modes & MODE_ANALOG_TUNING));
  
  // 最大模式组合
  uint16_t all_modes = MODE_ANALOG_TUNING | MODE_1PPS_PHASE_MEASUREMENT |
                       MODE_ULTRA_LOW_POWER | MODE_CHECKSUM_REQUIRED;
  // 注意：互斥模式只能同时启用一个，所以这里使用MODE_1PPS_PHASE_MEASUREMENT
  
  mock_reset();
  assert(set_mode(MODE_ANALOG_TUNING, true, &new_modes) == 0);
  assert(set_mode(MODE_1PPS_PHASE_MEASUREMENT, true, &new_modes) == 0);
  assert(set_mode(MODE_ULTRA_LOW_POWER, true, &new_modes) == 0);
  assert(set_mode(MODE_CHECKSUM_REQUIRED, true, &new_modes) == 0);
  assert(query_modes(&new_modes) == 0);
  assert(new_modes == all_modes);
  printf("✅ UC6: test_boundary_conditions: PASS\n");
}

void test_all_mode_operations() {
  printf("\n====================[ Mode Controller TEST START ]====================\n");
  test_set_mode_success();
  test_set_mode_errors();
  test_query_modes_success();
  test_query_modes_errors();
  test_mutex_modes();
  test_boundary_conditions();
  printf("====================[ Mode Controller TEST PASSED ]====================\n");
}