#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "ruclock.h"
#include "uart.h"
#include "ruclock_macros.h"
#include "ruclock_utils.h"

// 定义字段名数组（按你提供的顺序）
const char *field_keys[] = {
  "Status", "Alarm", "SN", "Mode", "Contrast",
  "LaserI", "TCXO", "HeatP", "Sig", "Temp",
  "Steer", "ATune", "Phase", "DiscOK",
  "TOD", "LTime", "Ver"
};

int ruclock_init()
{
  return 0;
}

//通用的读写
int WRITE_READ_RUCLOCK(const char *command, char *response)
{
  int recv_n;
  int len = 0, iRet = 0;
  time_t start_time = time(NULL);
  char recv_buf[MAX_RESPONSE_LENGTH] = {0};
  // 串口初始化
  uart_exit(UART2);
  iRet = uart_init(UART2);
  if(iRet!=0) {
    return iRet;
  }
  // 格式化命令
  format_ruclock_command(command, recv_buf, sizeof(recv_buf));

  // 发送命令
  DEBUG_LOG("command: %s", command);
  uart_send(UART2, (const unsigned char *)command, strlen(command));

  // 获取响应
  do {
    // 检查是否超时
    if (time(NULL) - start_time > TIMEOUT_SECONDS) {
      ERR_LOG("Timeout occurred while waiting for response!\n");
      return -1;
    }

    recv_n = uart_recv(UART2, (unsigned char *)(recv_buf + len), sizeof(recv_buf) - len);
    // 检查响应的合法性
    if (ends_with_crlf(recv_buf, len)) {
      break;
    }
    if(recv_n > 0) len += recv_n;
  } while(1);

  len = sanitize_ruclock_response(recv_buf, len, response);
  DEBUG_LOG("RESPONSE: len = %d, response = {%s}\n", len, response);

  return len;
}

int get_current_value(const char *command, char *response)
{
  int recv_n;
  char recv_buf[MAX_RESPONSE_LENGTH];
  char final_command[MAX_COMMAND_LENGTH];

  // 构造查询命令
  snprintf(final_command, sizeof(final_command), "!%s\r\n", command);
  // 发送命令并接收响应
  recv_n = WRITE_READ_RUCLOCK(final_command, recv_buf);  

  // 检查是否接收到数据
  if (recv_n <= 0) {
      printf("recv is empty!\n");
      return -1;
  }

  // 输出响应
  strncpy(response, recv_buf, recv_n);
  return 0;
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
  print_table(field_keys, fields, num_fields);
  
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

int get_telemetry_headers(char *telemetry_name)
{
  int recv_n;
  char recv_buf[MAX_RESPONSE_LENGTH];
  
  recv_n = WRITE_READ_RUCLOCK(TELEMETRY_HEAD, recv_buf);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  strncpy(telemetry_name, recv_buf, recv_n);

  return 0;
}

// 绝对值调整 CSAC 频率
//flag表示是绝对值写入还是相对值写入，绝对值写入就是‘A’,相对值写入就是‘D’
int adjust_frequency(char flag, int freq, int *updated_freq) {
  int recv_n;
  char recv_buf[RESPONSE_LENGTH];

  // 判断flag是否是合法的值
  if (flag != 'A' && flag != 'D') {
    printf("Error: Invalid flag.\n");
    return -1;
  }

  // 构建命令字符串
  char command_buf[COMMAND_LENGTH];
  snprintf(command_buf, sizeof(command_buf), "!%c%c%+d\r\n", 'F', flag, freq);

  // 发送命令并接收响应，!FY(freq)\r\n,Y是A/D,freq是当前写入的修正值，单位是10^-15
  recv_n = WRITE_READ_RUCLOCK(command_buf, recv_buf);
  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 从响应中提取频率值
  if(sscanf(recv_buf, "Steer = %d", updated_freq) != 1) {
    printf("Error: Failed to extract frequency value from response.\n");
    return -1;
  }
  // 输出响应
  ERR_LOG("Frequency adjust response: %d\n", *updated_freq);

  return 0;
}

// 获取 CSAC 频率，获取的是当前的修正值，单位是10^-12
int get_current_frequency(int *freq) {
  int recv_n;
  char recv_buf[RESPONSE_LENGTH];
  
  recv_n = WRITE_READ_RUCLOCK(TELEMETRY_GET_FREQ, recv_buf);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 从响应中提取频率值
  if(sscanf(recv_buf, "Steer = %d", freq) != 1) {
    printf("Error: Failed to extract frequency value from response.\n");
    return -1;
  }
  ERR_LOG("get_current_frequency: updated freq = {%d}\n", *freq);

  return 0;
}

int set_absolute_frequency(int abs_freq, int *now_freq){
  return adjust_frequency('A', abs_freq, now_freq);
}
int adjust_relative_frequency(int offset_freq, int *now_freq){
  return adjust_frequency('D', offset_freq, now_freq);
}

// 锁定当前的转向值
/*!*Once a Latch command is issued, the previously latched value cannot be recovered. 
There is no command to restore “factory defaults”.*!*/
int latch_current_frequency(void) {
  int recv_n;
  char recv_buf[RESPONSE_LENGTH];
  
  recv_n = WRITE_READ_RUCLOCK(TELEMETRY_LATCH_FREQ, recv_buf);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 输出响应
  printf("Frequency adjust response: %s\n", recv_buf);
  return 0;
}

// 设置操作模式
// 返回值：设置成功返回1， 设置失败返回0， 命令错误返回-1
int set_operating_modes(Telemetry_Modes_type type)
{
  char command[COMMAND_LENGTH];  // 命令字符串长度
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  int recv_n;

  snprintf(command, sizeof(command), "!%c%c\r\n", 'M', type);

  recv_n = WRITE_READ_RUCLOCK(command, response);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 从响应中提取返回值
  int ret = 0;
  if (sscanf(response, "0x000%d", &ret) != 1) {  // ?
    printf("Error: Failed to extract frequency value from response.\n");
    return -1;
  }

  // 输出响应
  printf("return value = %d\n", ret);
  printf("Unit response: %s\n", response);

  return ret;
}

/**
 @description:设置 1PPS 校准电缆长度补偿
 @param:输入的补偿值以 100 ps 为单位，最大绝对值为 1000（对应 100 ns）
 @return:返回设置后的补偿值; -1调用接口失败；
*/
int set_1pps_cable_length_compensation(int compensation_in_100ps) {
  char command[COMMAND_LENGTH];
  char response[RESPONSE_LENGTH];
  snprintf(command, sizeof(command), "!DC%d\r\n", compensation_in_100ps);
  int len = WRITE_READ_RUCLOCK((const char *)command, response);
  if (len < 0) {
    ERR_LOG("set_1pps_cable_length_compensation is failed!\n");
    return -1;
  }
  DEBUG_LOG("Unit response: %s\n", response);
  return atoi(response);
}

/**
 @description:查询当前1pps的校准电缆长度补偿
 @return:返回1pps的电缆补偿值； -1调用接口失败；
*/
int query_1pps_cable_length_compensation(){
  char response[RESPONSE_LENGTH];
  int len = WRITE_READ_RUCLOCK((const char *)DISC_QUERY_CABLE_LEN, response);
  if (len < 0) {
    ERR_LOG("query_1pps_cable_length_compensation is failed!\n");
    return -1;
  }
  DEBUG_LOG("Unit response: %s\n", response);
  return atoi(response);
}

/**
 @description:将当前的电缆长度补偿值锁定为上电默认值
 @return:1设置成功，0设置失败
*/
int latch_1pps_compensation_to_powerup_default(){
  char response[RESPONSE_LENGTH];
  char valid_response[] = "Phase comp latched";
  int len = WRITE_READ_RUCLOCK((const char *)DISC_LATCH_CABLE_LEN, response);
  if (len < 0) {
    ERR_LOG("query_1pps_cable_length_compensation is failed!\n");
    return -1;
  }
  if (strcmp(valid_response, response) == 0) {
    return 1;
  }
  return 0;
}

//Set 1PPS Disciplining Cable Length Compensation
//电缆长度补偿以以100 ps为单位的有符号整数表示，最大值为±1000（100 ns）
int set_pps_cable_compensation(int compensate)
{
  char command[COMMAND_LENGTH];  // 命令字符串长度
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  int recv_n;

  snprintf(command, sizeof(command), "!%s%d\r\n", "DC", compensate);

  recv_n = WRITE_READ_RUCLOCK(command, response);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }
  // 输出响应
    printf("Unit response: %s\n", response);

    return 0;
}

//查询当前的补偿值

//存储当前的补偿值
int store_pps_cable_compensation(int compensate)
{
  int recv_n;
  char recv_buf[RESPONSE_LENGTH];
  
  recv_n = WRITE_READ_RUCLOCK(TELEMETRY_LATCH_FREQ, recv_buf);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 输出响应
  printf("Frequency adjust response: %s\n", recv_buf);
  return 0;
}

//Set Ultra-Low Power Mode Parameters
int set_power_parameter(int32_t sleep_time, int32_t wake_time)
{
  char command[COMMAND_LENGTH];  // 命令字符串长度
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  int recv_n;

  snprintf(command, sizeof(command), "!%c%d,%d\r\n", 'U', sleep_time, wake_time);

  recv_n = WRITE_READ_RUCLOCK(command, response);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }
  // 输出响应
    printf("Unit response: %s\n", response);

    return 0;
}

//
int get_power_parameter(int32_t *sleep_time, int32_t *wake_time)
{
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  char *fields[2]; // 存储分割后的字段
  int num_fields = 0; // 记录字段数量
  int recv_n;

  recv_n = get_current_value("U", response);  
  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  char *token = strtok(response, ",");
    while (token != NULL && num_fields < 2) {
        fields[num_fields++] = token;
        token = strtok(NULL, ",");
  }
  *sleep_time = strtol(fields[0], NULL, 10);
  *wake_time = strtol(fields[1], NULL, 10);
  printf("sleep time = %d, wake_time = %d\n", *sleep_time, *wake_time);
  return 0;
  
}

//设置 Time-of-Day
//flag表示是绝对值写入还是相对值写入，绝对值写入就是‘A’,相对值写入就是‘D’
int set_timeofday(char flag, uint32_t time)
{
  char command[COMMAND_LENGTH];  // 命令字符串长度
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  int recv_n;

  // 判断flag是否是合法的值
  if (flag != 'A' && flag != 'D') {
    printf("Error: Invalid flag.\n");
    return -1;
  }

  snprintf(command, sizeof(command), "!%c%c%d\r\n", 'T', flag, time);

  recv_n = WRITE_READ_RUCLOCK(command, response);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }
  // 输出响应
    printf("Unit response: %s\n", response);

    return 0;
}

int get_timeofday(uint32_t *time)
{
  char command[] = "!T?\r\n";
  char response[RESPONSE_LENGTH]; // 定义响应字符串的长度
  int recv_n;

  // 向设备发送命令并接收响应
  recv_n = WRITE_READ_RUCLOCK(command, response);

  if (recv_n <= 0) {
      printf("Error: No response received\n");
      return -1;
  }

  // 解析响应中的时间数据
    char *endptr;
    *time = (uint32_t)strtol(response, &endptr, 10);
    if (endptr == response || *endptr != '\0') {
        printf("Error: Failed to parse time from response\n");
        return -1;
    }

  return 0;
}

//设置1PPs相位阈值（相对值设置）默认20
int set_1PPs_Threshold(int new_threshold)
{
  char command[COMMAND_LENGTH];  // 命令字符串长度
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  int recv_n;

  snprintf(command, sizeof(command), "!%c%d\r\n", 'm', new_threshold);

  recv_n = WRITE_READ_RUCLOCK(command, response);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }
  // 输出响应
    printf("Unit response: %s\n", response);

    return 0;
}

int get_1PPs_Threshold(int *current_threshold)
{
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  int recv_n;

  recv_n = get_current_value("m", response);  
  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 输出响应
    printf("Unit response: %s\n", response);

  *current_threshold = atoi(response);
  return 0;
}

int set_1pps_pulse_width(int multiple) {
  char command[COMMAND_LENGTH];
  char response[RESPONSE_LENGTH];
  int recv_n;

  // 检查倍数是否在允许的范围内
  if (multiple < 1 || multiple > 4) {
      printf("Error: Pulse width multiple must be between 1 and 4.\n");
      return -1;
  }

  // 构建命令字符串
  snprintf(command, sizeof(command), "!>%d\r\n", multiple);

  // 发送命令并接收响应
  recv_n = WRITE_READ_RUCLOCK(command, response);
  if (recv_n <= 0) {
      printf("Error: No response received from the unit.\n");
      return -1;
  }

  // 输出响应
  printf("Response: %s\n", response);

  return 0;
}

int get_1pps_pulse_width(int *multiple)
{
  char response[RESPONSE_LENGTH]; // 响应字符串长度
  int recv_n;
  char width_str[10];

  recv_n = get_current_value(">", response);  
  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }
  // 输出响应
  printf("Unit response: %s\n", response);

  // 分析响应，查找关键字
  char *keyword = "PPS Pulse Width = ";
  char *start_pos = strstr(response, keyword);
  if (start_pos == NULL)
  {
      printf("Invalid response format!\n");
      return -1;
  }
  start_pos += strlen(keyword);
  char *unit_pos = strchr(start_pos, ' ');
  if (unit_pos == NULL)
  {
      printf("Invalid response format!\n");
      return -1;
  }
  strncpy(width_str, start_pos, unit_pos - start_pos);
  width_str[unit_pos - start_pos] = '\0';
  *multiple = atoi(response);
  return 0;
}

//延迟命令，延迟单位是秒
int deferred_command(int delay_t, char order)
{
  char command[COMMAND_LENGTH];
  char response[RESPONSE_LENGTH];
  int recv_n;

  // 检查order是否是合法的Telemetry_COMMAND_type枚举值
  switch(order) {
    case Telemetry_Headers:break; 
    case Telemetry_Data:break; 
    case Freqyency_Adjustment:break; 
    case Operating_Modes:break; 
    case Discipline_Time:break; 
    case U_L_power_param:break; 
    case Time_day:break; 
    case Discipline_Check:break; 
    case Pulse_Multiple:break; 
    case Deferred_Command:break; 
    case Help:break;  
    default:
      printf("Error: Invalid order.\n");
      return -1; // 如果不是合法的枚举值，报错并返回
  }
  // 构建命令字符串
  snprintf(command, sizeof(command), "!@%d%c\r\n", delay_t, order);

  // 发送命令并接收响应
  recv_n = WRITE_READ_RUCLOCK(command, response);
  if (recv_n <= 0) {
      printf("Error: No response received from the unit.\n");
      return -1;
  }

  // 输出响应
  printf("Response: %s\n", response);

  return 0;
}



// 其他命令接口根据文档3.3进行定义


// 截取字符串末尾两个字符的函数
char* get_last_two_chars(const char* str, int len) {
  if (len < 2) {
      return NULL;
  }
  // 为存储结果分配内存，包含字符串结束符
  char* result = (char*)malloc(3 * sizeof(char));
  if (result == NULL) {
      return NULL;
  }
  // 复制末尾两个字符到结果字符串
  result[0] = str[len - 2];
  result[1] = str[len - 1];
  result[2] = '\0';
  return result;
}


// 比较字符串末尾两个字符和 CRLF 的函数
int is_last_two_chars_crlf(const char* str, int len) {
  char* last_two = get_last_two_chars(str, len);
  if (last_two == NULL) {
      return 0;
  }
  int is_equal = strcmp(last_two, CRLF) == 0;
  free(last_two); // 释放动态分配的内存
  return is_equal;
}

// 删除末尾的 CRLF 符号，并添加终止符
int remove_end_crlf_in_string(const char *src, int len, char *dest) {
  memcpy(dest, src, len);
  dest[len - 2] = 0;
  return len -2;
}