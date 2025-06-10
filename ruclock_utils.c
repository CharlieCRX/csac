#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ruclock_utils.h"

int format_ruclock_command(const char *raw_cmd, char *full_cmd, size_t bufsize)
{
    size_t needed = strlen(raw_cmd) + 1 + 2 + 1; // '!' + cmd + "\r\n" + '\0'
    if (bufsize < needed) {
        return -1;
    }
    snprintf(full_cmd, bufsize, "!%s\r\n", raw_cmd);
    return 0;
}

/**
 * @brief 判断字符串是否以 "\n\n" 结尾
 *
 * 内部函数，不导出。用于辅助识别铷钟响应的结束符。
 *
 * @param buf 输入字符串缓冲区
 * @param len 输入字符串长度
 * @return 1 表示以 "\n\n" 结尾，0 表示不是
 */
static int ends_with_crlf(const char *buf, int len)
{
    return (len >= 2 && buf[len - 2] == '\n' && buf[len - 1] == '\n');
}

int sanitize_ruclock_response(const char *raw, int raw_len, char *cleaned)
{
    int clean_len = raw_len;

    if (ends_with_crlf(raw, raw_len)) {
        clean_len -= 2;
    }

    memcpy(cleaned, raw, clean_len);
    cleaned[clean_len] = '\0';
    return clean_len;
}
