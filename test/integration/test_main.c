#include <stdio.h>
extern void test_all_discipline_tests();
extern void test_ruclock_utils();
int main() {
  test_all_discipline_tests();
  test_ruclock_utils();
  return 0;
}