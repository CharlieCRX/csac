#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <csac_utils.h>

void test_format_ruclock_command() {
    char buffer[64];

    // 普通命令
    int ret = format_ruclock_command("M=0x0010", buffer, sizeof(buffer));
    assert(ret == 0);
    assert(strcmp(buffer, "!M=0x0010\r\n") == 0);

    // 问号命令
    ret = format_ruclock_command("m?", buffer, sizeof(buffer));
    assert(ret == 0);
    assert(strcmp(buffer, "!m?\r\n") == 0);

    // 边界命令（非常短）
    ret = format_ruclock_command("A", buffer, sizeof(buffer));
    assert(ret == 0);
    assert(strcmp(buffer, "!A\r\n") == 0);

    // 缓冲区不足
    char small_buf[6];
    ret = format_ruclock_command("M=0x0010", small_buf, sizeof(small_buf));
    assert(ret == -1);

    printf("✅ UC1: Format command: PASS\n");
}

void test_sanitize_ruclock_response() {
    char raw1[] = "OK\n\n";
    char cleaned[64];
    int len = sanitize_ruclock_response(raw1, strlen(raw1), cleaned);
    assert(strcmp(cleaned, "OK") == 0);
    assert(len == 2);

    char raw2[] = "VALUE=123\n\n";
    len = sanitize_ruclock_response(raw2, strlen(raw2), cleaned);
    assert(strcmp(cleaned, "VALUE=123") == 0);
    assert(len == 9);

    char raw3[] = "NO_CRLF";
    len = sanitize_ruclock_response(raw3, strlen(raw3), cleaned);
    assert(strcmp(cleaned, "NO_CRLF") == 0);
    assert(len == 7);

    char raw4[] = "TRAILING\\n\n";  // 注意：这是两个字符 \ 和 n，不是真正的换行
    len = sanitize_ruclock_response(raw4, strlen(raw4), cleaned);
    assert(strcmp(cleaned, "TRAILING\\n\n") == 0);
    assert(len == strlen(raw4));
    printf("✅ UC2: Sanitize response: PASS\n");
}



void test_convert_aTune() {
  assert(convert_aTune("1.234") == 1.234);
  assert(convert_aTune("3.456") == 3.456);
  assert(convert_aTune("---") < 0);
  assert(convert_aTune(NULL) < 0);
}

void test_convert_phase() {
  assert(convert_phase("123") == 123);
  assert(convert_phase("-456") == -456);
  assert(convert_phase("---") == INT16_MIN);
  assert(convert_phase("NEEDREFPPS") == INT16_MIN);
}

void test_convert_disOK() {
  assert(convert_disOK("1") == 1);
  assert(convert_disOK("0") == 0);
  assert(convert_disOK("---") == -1);
  assert(convert_disOK(NULL) == -1);
}

void test_telemetry_utils() {
  test_convert_aTune();
  test_convert_phase();
  test_convert_disOK();
  printf("✅ UC3: Convert aTune + phase + disOK: PASS\n");
}

void test_ruclock_utils() {
  printf("\n🧪====================[ ruclock_utils TEST START ]====================🧪\n");
  test_format_ruclock_command();
  test_sanitize_ruclock_response();
  test_telemetry_utils();
  printf("✅====================[ ruclock_utils TEST PASSED ]===================✅\n");
}