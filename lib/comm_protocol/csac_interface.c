#include "csac_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <uart.h>
#include <csac_macros.h>
#include <csac_utils.h>

int csac_send_command(const char *command, char *response)
{
  int recv_n;
  int len = 0, iRet = 0;
  time_t start_time = time(NULL);
  char recv_buf[MAX_RESPONSE_LENGTH] = {0};
  char format_command[MAX_RESPONSE_LENGTH] = {0};
  // 串口初始化
  uart_exit(UART2);
  iRet = uart_init(UART2);
  if(iRet!=0) {
    return iRet;
  }
  // 格式化命令
  format_ruclock_command(command, format_command, sizeof(format_command));

  // 发送格式化后的命令
  uart_send(UART2, (const unsigned char *)format_command, strlen(format_command));
  DEBUG_LOG("format_command: %s", format_command);

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

  // 清洁响应数据
  len = sanitize_ruclock_response(recv_buf, len, response);
  DEBUG_LOG("RESPONSE: len = %d, response = {%s}\n", len, response);

  return len;
}