#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "ruclock.h"
#include "mock_csac_interface.h"
#include "csac_enums.h"

void test_start_training_success() {
  mock_set_telemetry_response("0,0x0000,2203CS77980,0x0001,3452,1.05,1.492,10.69,1.339,27.54,624,2.34,123,1,259922,259824,1.09");

  DisciplineStartResult result = ruclock_discipliner_start_training(100, 10);

  assert(result.success == true);
  assert(result.code == DISCIPLINE_START_OK);
  printf("✅ UC1: test_start_training_success: PASS\n");
}



void test_start_training_fail_on_device_not_ready() {
    // 模拟设备未准备好 status = 5
    mock_set_telemetry_response("5,0x0000,2203CS77980,0x0001,3452,1.05,1.492,10.69,1.339,27.54,624,2.34,123,1,259922,259824,1.09");
    
    // 调用函数
    DisciplineStartResult result = ruclock_discipliner_start_training(100, 10);
    
    // 验证结果
    assert(result.success == false);
    assert(result.code ==  DISCIPLINE_START_ERR_CSAC_NOT_READY);
    printf("✅ UC2: test_start_training_fail_on_device_not_ready: PASS\n");
}


void test_start_training_fail_on_sync_failure() {
    // 模拟设备同步失败状态
    mock_set_telemetry_response("0,0x0000,2203CS77980,0x0001,3452,1.05,1.492,10.69,1.339,27.54,624,2.34,123,1,259922,259824,1.09");
    mock_set_1PPS_sync_response("E");
    
    // 调用函数
    DisciplineStartResult result = ruclock_discipliner_start_training(100, 10);
    
    // 验证结果
    assert(result.success == false);
    assert(result.code ==  DISCIPLINE_START_ERR_1PPS_NOT_READY);
    printf("✅ UC3: test_start_training_fail_on_sync_failure: PASS\n");
}

void test_all_ruclock_functions() {
  printf("\n====================[ RUCLOCK TEST START ]====================\n");
  test_start_training_success();
  test_start_training_fail_on_device_not_ready();
  test_start_training_fail_on_sync_failure();
  printf("====================[ RUCLOCK TEST PASSED ]====================\n");
}