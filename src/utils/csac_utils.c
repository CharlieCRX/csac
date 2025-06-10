#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <csac_utils.h>

int format_ruclock_command(const char *raw_cmd, char *full_cmd, size_t bufsize)
{
    size_t needed = strlen(raw_cmd) + 1 + 2 + 1; // '!' + cmd + "\r\n" + '\0'
    if (bufsize < needed) {
        return -1;
    }
    snprintf(full_cmd, bufsize, "!%s\r\n", raw_cmd);
    return 0;
}

int ends_with_crlf(const char *buf, int len)
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