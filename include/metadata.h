#ifndef _METADATA_H_
#define _METADATA_H_
#include "enums.h" // 仅依赖枚举定义

/* 模式位元数据接口 */
typedef struct {
    Telemetry_Modes_type mode;
    char code;
    const char* description;
    uint8_t is_enable;
} ModeMetadata;

const ModeMetadata* metadata_get_modes(size_t* count);
const ModeMetadata* metadata_find_mode(Telemetry_Modes_type target);

/* 状态元数据接口 */
typedef struct {
    E_telemetry_status status;
    const char* name;
    const char* description;
    uint8_t priority;
} StatusMetadata;

const StatusMetadata* metadata_get_statuses(size_t* count);
const StatusMetadata* metadata_find_status(E_telemetry_status target);


/* 测试函数 */
void test_print_all_metadata();
#endif