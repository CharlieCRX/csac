#ifndef RUCLOCK_UTILS_H
#define RUCLOCK_UTILS_H

#include <stddef.h>


/**
 * @brief 将原始命令格式化为符合铷钟要求的完整指令
 *
 * 铷钟要求的命令格式为以 '!' 开头，以 "\r\n" 结尾的字符串。
 * 例如输入 "m10"，会被格式化为 "!m10\r\n"。
 *
 * @param raw_cmd 原始命令字符串（不包含 '!' 和 "\r\n"）
 * @param full_cmd 输出缓冲区，用于存储格式化后的完整命令
 * @param bufsize full_cmd 缓冲区的大小
 * @return 0 表示成功，-1 表示缓冲区不足
 */
int format_ruclock_command(const char *raw_cmd, char *full_cmd, size_t bufsize);


/**
 * @brief 判断字符串是否以 "\n\n" 结尾（铷钟响应结束标志）
 *
 * @param buf 输入字符串缓冲区
 * @param len 输入字符串长度
 * @return 1 表示以 "\n\n" 结尾，0 表示不是
 */
int ends_with_crlf(const char *buf, int len);



/**
 * @brief 清洗铷钟响应内容，去除末尾的双换行符 "\n\n"
 *
 * 铷钟响应格式通常以 "\n\n" 结尾，该函数会自动将其去除，并在清洗后的字符串末尾添加 '\0'。
 * 如果响应不以 "\n\n" 结尾，则原样复制。
 *
 * @param raw 原始响应字符串
 * @param len 原始响应的字节长度
 * @param cleaned 输出缓冲区，用于存储去除后缀的响应，需至少和 len 一样大
 * @return 去除末尾后的字符串长度（不含 '\0'）
 */
int sanitize_ruclock_response(const char *raw, int len, char *cleaned);
#endif
