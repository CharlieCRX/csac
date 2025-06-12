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

void test_all_mode_operations() {
  printf("\n====================[ Mode Controller TEST START ]====================\n");
  test_set_mode_success();
  printf("====================[ Mode Controller TEST PASSED ]====================\n");
}