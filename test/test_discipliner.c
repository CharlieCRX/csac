#include <stdio.h>
#include <assert.h>
#include <discipliner.h>
void test_enable_discipline_should_send_correct_command() {
  assert(discipliner_is_ready_to_execute());
}

void test_all_discipline_tests() {
  test_enable_discipline_should_send_correct_command();
}