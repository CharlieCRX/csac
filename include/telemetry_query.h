#ifndef TELEMETRY_QUERY_H
#define TELEMETRY_QUERY_H
#include <csac_enums.h>
/**
 * @brief 获取 CSAC 铯原子钟的遥测数据
 *
 * 该函数向设备发送 TELEMETRY_DATA 命令，从响应中解析各项遥测参数，
 * 并填充到传入的 telemetry 结构体中。返回值用于判断通讯是否成功以及数据是否完整。
 *
 * 处理流程包括：
 * 1. 通过 csac_send_command 发送遥测请求并接收数据；
 * 2. 使用逗号分隔符将响应字符串切分为多个字段；
 * 3. 检查字段数量是否符合协议规定（EXPECTED_FIELDS）；
 * 4. 将字段逐个转换并赋值到结构体 telemetry 中；
 * 5. 对部分复杂字段（如 aTune、phase、disOK）调用自定义转换函数处理；
 * 6. 对字符串字段（如 SN、fwVer）做安全拷贝，避免缓冲区溢出。
 *
 * 该函数适用于周期性拉取遥测状态并展示于前端或用于日志记录与状态诊断。
 *
 * @param telemetry 指向 T_CSAC_telemetry 结构体的指针，用于接收解析后的数据
 *
 * @return int
 * - 返回 0：成功获取并解析遥测数据；
 * - 返回 -1：命令发送失败或响应接收失败；
 * - 返回 -2：响应字段数量不符合协议预期（数据损坏或设备异常）。
 */
int get_telemetry_data(T_CSAC_telemetry *telemetry);

#endif // TELEMETRY_QUERY_H