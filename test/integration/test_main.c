#include <stdio.h>
extern void test_all_discipline_tests();
extern void test_ruclock_utils();
extern void test_all_telemetry_tests();
int main() {
  test_all_discipline_tests();
  test_ruclock_utils();
  test_all_telemetry_tests();
  return 0;
}