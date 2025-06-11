#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <telemetry_query.h>
#include "csac_utils.h"
#include "csac_macros.h"
#ifdef UNIT_TEST
#include "mock_csac_interfac.h"
#else
#include <csac_interface.h>
#endif

#define EXPECTED_FIELDS 17
int get_telemetry_data(T_CSAC_telemetry *telemetry) {
  int resp_size;
  char resp_buf[MAX_REBUFF];
  char *fields[MAX_FIELDS]; // 存储分割后的字段
  int num_fields = 0;       // 记录字段数量
   
  // 1. 发送命令并接收遥测字符串
  resp_size = csac_send_command(TELEMETRY_DATA, resp_buf);
  if(resp_size < 0) {
    return -1;
  }

  // 2. 分割字符串为字段数组
  char *token = strtok(resp_buf, ",");
  while (token != NULL && num_fields < MAX_FIELDS) {
      fields[num_fields++] = token;
      token = strtok(NULL, ",");
  }

  if (num_fields != EXPECTED_FIELDS) {
    return -2; // 字段数量不符
  }

  // 3. 字段转换与赋值（按 Telemetry 表中顺序）
  telemetry->status = (E_telemetry_status)atoi(fields[0]);
  telemetry->alarms = (E_telemetry_alarms)strtol(fields[1], NULL, 16); // 0x1234
  strncpy(telemetry->SN, fields[2], sizeof(telemetry->SN)-1);
  telemetry->SN[sizeof(telemetry->SN)-1] = '\0'; // 保证结尾
  telemetry->modes = (E_telemetry_operating_modes)strtol(fields[3], NULL, 16);
  telemetry->Contrast = (uint32_t)strtoul(fields[4], NULL, 10);
  telemetry->laserI = strtod(fields[5], NULL);
  telemetry->tcxo = strtod(fields[6], NULL);
  telemetry->heatP = strtod(fields[7], NULL);
  telemetry->sigV = strtod(fields[8], NULL);
  telemetry->temp = strtod(fields[9], NULL);
  telemetry->steer = (int32_t)strtol(fields[10], NULL, 10);

  // 4. 特殊字段处理
  telemetry->aTune = convert_aTune(fields[11]);
  telemetry->phase = convert_phase(fields[12]);
  telemetry->disOK = convert_disOK(fields[13]);

  
  telemetry->TOD = (uint32_t)strtoul(fields[14], NULL, 10);
  telemetry->LTime = (uint32_t)strtoul(fields[15], NULL, 10);
  strncpy(telemetry->fwVer, fields[16], sizeof(telemetry->fwVer)-1);
  telemetry->fwVer[sizeof(telemetry->fwVer)-1] = '\0';

  return 0;
}