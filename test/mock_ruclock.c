#include "mock_ruclock.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
// 定义字段名数组（按你提供的顺序）
const char *field_keys[] = {
  "Status", "Alarm", "SN", "Mode", "Contrast",
  "LaserI", "TCXO", "HeatP", "Sig", "Temp",
  "Steer", "ATune", "Phase", "DiscOK",
  "TOD", "LTime", "Ver"
};


// 保存测试设置的模拟响应
static char mock_telemetry_response[MAX_RESPONSE_SIZE] = "0,0x0000,2203CS77980,0x0000,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.10";
static char mock_1PPS_sync_response[MAX_RESPONSE_SIZE] = "S";

int WRITE_READ_RUCLOCK(const char *command, char *response)
{
    // 模拟返回数据（可按不同命令做匹配）
    if (strcmp(command, TELEMETRY_DATA) == 0) {
      strcpy(response, mock_telemetry_response);
      return strlen(mock_telemetry_response);  // 返回 response 长度;
    }

    // 1PPS同步
    else if (strcmp(command, SYNC_1PPS) == 0) {
      strcpy(response, mock_1PPS_sync_response);
      return strlen(mock_1PPS_sync_response);
    }
    return -1;
}



void print_table(const char *keys[], char *values[], int count) {
  printf("+------------+------------------+\n");
  printf("| %-10s | %-16s |\n", "Key", "Value");
  printf("+------------+------------------+\n");

  for (int i = 0; i < count; i++) {
      printf("| %-10s | %-16s |\n", keys[i], values[i]);
  }

  printf("+------------+------------------+\n");
}

int get_telemetry_para(T_ruclock_telemetry *telemetry_param)
{
  int recv_n;
  char recv_buf[MAX_REBUFF];
  char *fields[MAX_FIELDS]; // 存储分割后的字段
  int num_fields = 0; // 记录字段数量
   

  recv_n = WRITE_READ_RUCLOCK(TELEMETRY_DATA, recv_buf);
  if(recv_n < 0)
  {
    return -1;
  }

  // 使用 strtok 函数分割字符串
  char *token = strtok(recv_buf, ",");
  while (token != NULL && num_fields < MAX_FIELDS) {
      fields[num_fields++] = token;
      token = strtok(NULL, ",");
  }
  // 打印 key-value 格式字段
  // print_table(field_keys, fields, num_fields);
  
  telemetry_param->status = atoi(fields[0]); // 转换为整数
  telemetry_param->alarms = strtol(fields[1], NULL, 16); // 16进制字符串转换为整数
  strcpy(telemetry_param->SN, fields[2]);
  telemetry_param->modes = strtol(fields[3], NULL, 16);
  telemetry_param->Contrast = atoi(fields[4]); // 字符串转换为整数
  telemetry_param->LaserI = atof(fields[5]); // 字符串转换为浮点数
  telemetry_param->TCXO = atof(fields[6]); // 字符串转换为浮点数
  telemetry_param->HeatP = atof(fields[7]); // 字符串转换为浮点数
  telemetry_param->Signal_level = atof(fields[8]); // 字符串转换为浮点数
  telemetry_param->Temp = atof(fields[9]); // 字符串转换为浮点数
  telemetry_param->Steer = atoi(fields[10]); // 字符串转换为整数
  if (strcmp(fields[11], "---") == 0) {
      telemetry_param->ATune = -1; // 如果字符串是 "---"，则设置为-1
  } else {
      telemetry_param->ATune = atof(fields[11]); // 字符串转换为浮点数
  }
  telemetry_param->Phase = atoi(fields[12]); // 字符串转换为整数
  strcpy(telemetry_param->DiscOK, fields[13]);
  telemetry_param->TOD = strtoul(fields[14], NULL, 10); // 字符串转换为无符号长整数
  telemetry_param->LTime = strtoul(fields[15], NULL, 10); // 字符串转换为无符号长整数
  strcpy(telemetry_param->FWver, fields[16]);

  return 0;
}

int get_telemetry_data(T_ruclock_telemetry *telemetry)
{
  
  int iRet = 0;
  iRet = get_telemetry_para(telemetry);
  if(iRet < 0){
    printf("get ruclock failed!\n");
    return -1;
  }
  return 0;
}


void mock_set_telemetry_response(const char *resp) {
  strncpy(mock_telemetry_response, resp, MAX_RESPONSE_SIZE - 1);
  mock_telemetry_response[MAX_RESPONSE_SIZE - 1] = '\0';
}


void mock_set_1PPS_sync_response(const char *resp) {
  strncpy(mock_1PPS_sync_response, resp, MAX_RESPONSE_SIZE - 1);
  mock_1PPS_sync_response[MAX_RESPONSE_SIZE - 1] = '\0';
}