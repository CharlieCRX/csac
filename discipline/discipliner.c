#include "discipliner.h"
#include "mock_ruclock.h"

// Helper function
bool is_CSAC_status_ready();
bool is_1PPS_EXT_ready();



// Discilipine Fuction
bool discipliner_is_ready_to_execute()
{
  bool ret = true;
  ret &= is_CSAC_status_ready();
  ret &= is_1PPS_EXT_ready();
  return ret;
}

bool discipliner_enable(bool on)
{
  return false;
}


// [START] - Helper function definition
bool is_CSAC_status_ready()
{
  return false;
}

bool is_1PPS_EXT_ready()
{
  return false;
}
// [END]   - Helper function definition