#include <stdio.h>
#include <assert.h>
#include <discipliner.h>
#include <mock_csac_interfac.h>
void test_status_failed_to_execute() {
  // GPS 同步成功
  mock_set_1PPS_sync_response("S");

  // status = 1的不稳定状态
  mock_set_telemetry_response("1,0x0000,2203CS77980,0x0000,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.10");
  assert(!discipliner_is_ready_to_execute());

  // status = 5的不稳定状态
  mock_set_telemetry_response("5,0x0000,2203CS77980,0x0000,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.10");
  assert(!discipliner_is_ready_to_execute());

  // status = 0的稳定状态
  mock_set_telemetry_response("0,0x0000,2203CS77980,0x0000,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.10");
  assert(discipliner_is_ready_to_execute());

  printf("✅ UC1: CSAC lock check: PASS\n");
}

void test_1PPS_sync_failed_to_execute() {
  mock_set_1PPS_sync_response("E");
  assert(!discipliner_is_ready_to_execute());

  mock_set_1PPS_sync_response("S");
  assert(discipliner_is_ready_to_execute());

  printf("✅ UC2: GPS 1PPS Sync: PASS\n");
}

void test_set_and_query_constant_time() {
  assert(discipliner_set_time_constant(100));
  assert(discipliner_get_time_constant() == 100);
  assert(discipliner_set_time_constant(10000));
  assert(discipliner_get_time_constant() == 10000);
  assert(!discipliner_set_time_constant(1));
  assert(!discipliner_set_time_constant(9));
  assert(!discipliner_set_time_constant(-1));
  assert(!discipliner_set_time_constant(10001));

  assert(discipliner_set_time_constant(199));
  assert(discipliner_get_time_constant() == 199);
  printf("✅ UC3: Set constant time: PASS\n");
}

void test_set_and_query_phase_threshold() {
  assert(discipliner_set_phase_threshold(1));
  assert(discipliner_get_phase_threshold() == 1);
  assert(discipliner_set_phase_threshold(255));
  assert(discipliner_get_phase_threshold() == 255);
  assert(discipliner_set_phase_threshold(199));
  assert(discipliner_get_phase_threshold() == 199);

  assert(!discipliner_set_phase_threshold(0));
  assert(discipliner_set_phase_threshold(-1));
  assert(discipliner_get_phase_threshold() == 255);

  printf("✅ UC4: Set phase threshold: PASS\n");
}

void test_all_discipline_tests() {
  printf("\n🧪====================[ discipliner TEST START ]====================🧪\n");
  test_status_failed_to_execute();
  test_1PPS_sync_failed_to_execute();
  test_set_and_query_constant_time();
  test_set_and_query_phase_threshold();
  printf("✅====================[ discipliner TEST PASSED ]===================✅\n");
}