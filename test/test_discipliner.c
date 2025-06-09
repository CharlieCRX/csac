#include <stdio.h>
#include <assert.h>
#include <discipliner.h>
#include <mock_ruclock.h>

void test_enable_discipline_should_send_correct_command() {
  assert(discipliner_is_ready_to_execute());
}

void test_enable_discipline_failed() {
  mock_set_telemetry_response("1,0x0000,2203CS77980,0x0000,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.10");
  assert(!discipliner_is_ready_to_execute());
}

void test_all_discipline_tests() {
  test_enable_discipline_should_send_correct_command();
  test_enable_discipline_failed();
  printf("TEST ALL SUCCESSFULLY!\n");
}