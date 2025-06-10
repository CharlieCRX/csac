#include <string.h>
#include <stdlib.h>
#include "discipliner.h"
#include "mock_ruclock.h"
#include "ruclock_macros.h"

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

bool discipliner_set_time_constant(uint16_t sec)
{
  if (sec < 10 || sec > 10000) {
    return false;
  }

  char command[COMMAND_LENGTH];
  char response[RESPONSE_LENGTH];
  snprintf(command, sizeof(command), "!%c%d" CRLF, 'D', sec);

  int len = WRITE_READ_RUCLOCK(command, response);  

  if(len < 0) {
    ERR_LOG("[IO ERROR]:discipliner_set_time_constant is failed!\n");
    return false;
  }
  // 输出响应
  return sec == atoi(response);
}

uint16_t discipliner_get_time_constant()
{
  char response[RESPONSE_LENGTH];
  int len = WRITE_READ_RUCLOCK((const char *)QUERY_1PPS_TIME_CONST, response);
  if (len < 0) {
    ERR_LOG("[IO ERROR]:query_1pps_disciplining_time_constant is failed!\n");
    return false;
  }
  return atoi(response);
}

bool discipliner_set_phase_threshold(uint8_t ns)
{
  if (ns < 1) {
    return false;
  }

  char command[COMMAND_LENGTH];
  char response[RESPONSE_LENGTH];
  snprintf(command, sizeof(command), "!%c%d" CRLF, 'm', ns);

  int len = WRITE_READ_RUCLOCK(command, response);  

  if(len < 0) {
    ERR_LOG("[IO ERROR]:discipliner_set_phase_threshold is failed!\n");
    return false;
  }
  // 输出响应
  return ns == atoi(response);
}

uint8_t discipliner_get_phase_threshold()
{
  char response[RESPONSE_LENGTH];
  int len = WRITE_READ_RUCLOCK((const char *)QUERY_PHASE_THRESHOLD, response);
  if (len < 0) {
    ERR_LOG("[IO ERROR]:query_1pps_disciplining_time_constant is failed!\n");
    return false;
  }
  return atoi(response);
}

// [START] - Helper function definition
bool is_CSAC_status_ready()
{
  T_ruclock_telemetry telemetry;
  // 查询铷钟状态
  get_telemetry_data(&telemetry);

  // 检查Status是否为0的稳定状态
  return telemetry.status == Locked;
}

bool is_1PPS_EXT_ready()
{
  char response[RESPONSE_LENGTH];
  int len = WRITE_READ_RUCLOCK((const char *)SYNC_1PPS, response);

  if (len < 0) {
    return false;
  }
  char sync_success[] = "S";

  if (strcmp(response, sync_success) == 0) {
    return true;
  }

  return false;
}
// [END]   - Helper function definition