#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ruclock.h>
#include <csac_interface.h>
#include <uart.h>
#include <csac_macros.h>
#include <csac_utils.h>
#include <discipliner.h>
#include <telemetry_query.h>
#include <csac_enums.h>

DisciplineStartResult ruclock_discipliner_start_training(uint8_t ns_threshold, uint16_t time_constant)
{
  if (!discipliner_is_CSAC_status_ready()) {
    ERR_LOG("铷钟状态不稳定\n");
    return (DisciplineStartResult){
      .success = false,
      .code = DISCIPLINE_START_ERR_CSAC_NOT_READY,
      .message = "铷钟状态不稳定，无法开始驯服"
    };
  }

  if (!discipliner_is_1PPS_EXT_ready()) {
    ERR_LOG("1PPS 输入信号未准备就绪\n");
    return (DisciplineStartResult){
      .success = false,
      .code = DISCIPLINE_START_ERR_1PPS_NOT_READY,
      .message = "1PPS输入信号未准备就绪"
    };
  }

  if (!discipliner_set_phase_threshold(ns_threshold)) {
    ERR_LOG("设置相位阈值失败：PhaseThreshold=%d ns\n", ns_threshold);
    return (DisciplineStartResult){
      .success = false,
      .code = DISCIPLINE_START_ERR_SET_PHASE_FAIL,
      .message = "设置相位阈值失败"
    };
  }

  if (!discipliner_set_time_constant(time_constant)) {
    ERR_LOG("设置时间常数失败：TimeConstant=%d s\n", time_constant);
    return (DisciplineStartResult){
      .success = false,
      .code = DISCIPLINE_START_ERR_SET_TIME_CONSTANT_FAIL,
      .message = "设置时间常数失败"
    };
  }

  if (!discipliner_enable(true)) {
    ERR_LOG("训练启动失败\n");
    return (DisciplineStartResult){
      .success = false,
      .code = DISCIPLINE_START_ERR_ENABLE_FAIL,
      .message = "训练启动失败"
    };
  }

  DEBUG_LOG("训练开始：PhaseThreshold=%d ns, TimeConstant=%d s\n", ns_threshold, time_constant);
  return (DisciplineStartResult){
    .success = true,
    .code = DISCIPLINE_START_OK,
    .message = "驯服已启动"
  };
}

void discipliner_stop_training(const char *reason)
{
  discipliner_enable(false);
  DEBUG_LOG("训练终止：%s\n", reason);
}

bool ruclock_discipliner_update_training_status() {
  T_CSAC_telemetry telemetry;
  get_telemetry_data(&telemetry);
  if (telemetry.disOK == 0) {
    DEBUG_LOG("训练中：DiscOK=%d, Phase=%d ns\n", telemetry.disOK, telemetry.phase);
    return false;
  }

  if (telemetry.disOK == 1) {
    discipliner_stop_training("驯服成功！");
    return true;
  }

  else if (telemetry.disOK == 2) {
    discipliner_stop_training("驯服失败：信号中断");
    return true; // 返回 true 表示训练已完成（失败）
  } 
  
  else {
    discipliner_stop_training("驯服错误：并未设置驯服模式");
    return true; // 返回 true 表示训练已完成（失败）
  }
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
