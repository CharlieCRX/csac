#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "metadata.h"

/**************** 模式元数据表 ****************/
static const ModeMetadata MODE_METADATA[] = {
  {Enable_Analog,             'A', "Enable Analog tuning", 1},
  {Enable_Phase_Measure,      'M', "1PPS phase measurement", 1},
  {Enable_Sync,               'S', "1PPS auto-sync", 1},
  {Enable_Discipline,         'D', "Discipline mode", 1},
  {Enable_U_LOW_POWER,        'U', "Ultra-Low Power mode", 1},
  {Enable_Checksum,           'C', "Require checksum", 1},
  
  {DisEnable_Analog,          'a', "Disable Analog tuning", 0},
  {DisEnable_Phase_Measure,   'm', "Disable phase measurement", 0},
  {DisEnable_Sync,            's', "Disable auto-sync", 0},
  {DisEnable_Discipline,      'd', "Disable discipline", 0},
  {DisEnable_U_LOW_POWER,     'u', "Disable Ultra-Low Power", 0},
  {DisEnable_Checksum,        'c', "Disable checksum", 0},
  
  {Report_current_settings,   '?', "Query settings", 0}
};

/**************** 状态元数据表 ****************/
static const StatusMetadata STATUS_METADATA[] = {
  // 格式：{枚举值, 状态名称, 描述, 优先级}
  {Asleep, "Asleep", "ULP mode only", 1},
  {Warmup, "Warmup", "Initial warm-up", 2},
  {HeaterEquilibration, "HeaterEquilibration", "Heater equilibration", 3},
  {MicrowaveAcquisition, "MicrowaveAcquisition", "Microwave power acquisition", 4},
  {LaserCurrentAcquisition, "LaserCurrentAcquisition", "Laser current acquisition", 5},
  {LaserPowerAcquisition, "LaserPowerAcquisition", "Laser power acquisition", 6},
  {MicrowaveFrequencyAcquisition, "MicrowaveFrequencyAcquisition", 
      "Microwave frequency acquisition", 7},
  {MicrowaveFrequencyStabilization, "MicrowaveFrequencyStabilization", 
      "Microwave frequency stabilization", 8},
  {MicrowaveFrequencySteering, "MicrowaveFrequencySteering", 
      "Microwave frequency steering", 9},
  {Locked, "Locked", "Locked state", 10}
};

/**************** 通用访问接口 ****************/
const ModeMetadata* metadata_get_modes(size_t* count) {
    *count = sizeof(MODE_METADATA)/sizeof(ModeMetadata);
    return MODE_METADATA;
}

const ModeMetadata* metadata_find_mode(Telemetry_Modes_type target) {
    const size_t count = sizeof(MODE_METADATA)/sizeof(ModeMetadata);
    for(size_t i=0; i<count; i++) {
        if(MODE_METADATA[i].mode == target) return &MODE_METADATA[i];
    }
    return NULL;
}

static void print_mode_datas() {
  size_t mode_count;
  const ModeMetadata *mode_metadata = metadata_get_modes(&mode_count);
  printf("Mode Metadata:\n");
  for (size_t i = 0; i < mode_count; i++) {
    printf("Mode: %d, Code: %c, Description: %s, Is Enable: %d\n",
    mode_metadata[i].mode, mode_metadata[i].code, mode_metadata[i].description, mode_metadata[i].is_enable);
  }
}

const StatusMetadata* metadata_get_statuses(size_t* count) {
    *count = sizeof(STATUS_METADATA)/sizeof(StatusMetadata);
    return STATUS_METADATA;
}

const StatusMetadata* metadata_find_status(E_telemetry_status target) {
    const size_t count = sizeof(STATUS_METADATA)/sizeof(StatusMetadata);
    for(size_t i=0; i<count; i++) {
        if(STATUS_METADATA[i].status == target) return &STATUS_METADATA[i];
    }
    return NULL;
}

static void print_status_datas() {
  size_t status_count;
  const StatusMetadata* status_metadata = metadata_get_statuses(&status_count);
  printf("\nStatus Metadata:\n");
  for (size_t i = 0; i < status_count; i++) {
    printf("Status: %d, Name: %s, Description: %s, Priority: %d\n",
       status_metadata[i].status, status_metadata[i].name, status_metadata[i].description, status_metadata[i].priority);
  }
}
void test_print_all_metadata() {
  print_mode_datas();
  print_status_datas();
}