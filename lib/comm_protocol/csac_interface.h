#ifndef CSAC_INTERFACE_H
#define CSAC_INTERFACE_H
#include <stdint.h>

/**
 * @brief 向 CSAC 发送命令并读取响应
 *
 * 自动格式化命令（加上 前缀! 和 \r\n），并自动去除响应末尾 \n\n。
 *
 * @param command  命令主体（如 "m?", "M=0x0010"）
 * @param response 存放去除末尾后的响应数据
 * @return int     响应数据长度（不含 \n\n），失败返回负值
 */
int csac_send_command(const char *command, char *response);

#endif // CSAC_INTERFACE_H