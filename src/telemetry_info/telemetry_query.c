#include <telemetry_query.h>
#ifdef UNIT_TEST
#include "mock_csac_interfac.h"
#else
#include <csac_interface.h>
#endif

int get_telemetry_data(T_CSAC_telemetry *telemetry) {
  return 0;
}