#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ruclock.h>
#include <csac_interface.h>
#include <uart.h>
#include <csac_macros.h>
#include <csac_utils.h>
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
  recv_n = csac_send_command(command_buf, recv_buf);
  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 从响应中提取频率值
  if(sscanf(recv_buf, "steer = %d", updated_freq) != 1) {
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
  
  recv_n = csac_send_command(TELEMETRY_GET_FREQ, recv_buf);  

  if(recv_n <= 0){
    printf("recv is empty!\n");
    return -1;
  }

  // 从响应中提取频率值
  if(sscanf(recv_buf, "steer = %d", freq) != 1) {
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
  
  recv_n = csac_send_command(TELEMETRY_LATCH_FREQ, recv_buf);  

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

  snprintf(command, sizeof(command), "%c%c", 'M', type);

  recv_n = csac_send_command(command, response);  

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
  recv_n = csac_send_command(command, response);
  if (recv_n <= 0) {
      printf("Error: No response received from the unit.\n");
      return -1;
  }

  // 输出响应
  printf("Response: %s\n", response);

  return 0;
}