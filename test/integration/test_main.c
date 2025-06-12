#include <stdio.h>
extern void test_all_mock_command();
extern void test_all_discipline_tests();
extern void test_ruclock_utils();
extern void test_all_telemetry_tests();
extern void test_all_mode_operations();
int main() {
  test_all_mock_command();
  test_all_discipline_tests();
  test_ruclock_utils();
  test_all_telemetry_tests();
  test_all_mode_operations();
  return 0;
}