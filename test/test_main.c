#include <stdio.h>
#include "mock_ruclock.h"
int main() {
  T_ruclock_telemetry telemetry;
  get_telemetry_data(&telemetry);
  return 0;
}