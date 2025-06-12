#include <string.h>
#include <stdlib.h>
#include "discipliner.h"
#include "telemetry_query.h"
#ifdef UNIT_TEST
#include "mock_csac_interface.h"
#else
#include <csac_interface.h>
#endif
#include "csac_macros.h"
#include "mode_controller.h"

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
  uint16_t new_modes;
  return set_mode(MODE_DISCIPLINE, on, &new_modes);
}

bool discipliner_is_enable()
{
  uint16_t modes;
  return (query_modes(&modes) == 0) && (modes & MODE_DISCIPLINE) == MODE_DISCIPLINE;
}

bool discipliner_set_time_constant(uint16_t sec)
{
  if (sec < 10 || sec > 10000) {
    return false;
  }

  char command[COMMAND_LENGTH];
  char response[RESPONSE_LENGTH];
  snprintf(command, sizeof(command), "%c%d", 'D', sec);

  int len = csac_send_command(command, response);

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
  int len = csac_send_command((const char *)DISC_QUERY_TIME_CONST, response);
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
  snprintf(command, sizeof(command), "%c%d", 'm', ns);

  int len = csac_send_command(command, response);  

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
  int len = csac_send_command((const char *)DISC_QUERY_PHASE_THRESHOLD, response);
  if (len < 0) {
    ERR_LOG("[IO ERROR]:query_1pps_disciplining_time_constant is failed!\n");
    return false;
  }
  return atoi(response);
}

// [START] - Helper function definition
bool is_CSAC_status_ready()
{
  T_CSAC_telemetry telemetry;
  // 查询铷钟状态
  get_telemetry_data(&telemetry);

  // 检查Status是否为0的稳定状态
  return telemetry.status == Locked;
}

bool is_1PPS_EXT_ready()
{
  char response[RESPONSE_LENGTH];
  int len = csac_send_command((const char *)SYNC_1PPS_MANUAL, response);

  if (len < 0) {
    return false;
  }
  char sync_success[] = "S";

  if (strcmp(response, sync_success) == 0) {
    return true;
  }

  return false;
}