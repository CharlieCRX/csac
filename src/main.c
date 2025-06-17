#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <csac_interface.h>
#include <csac_macros.h>
#include <telemetry_query.h>
#include <metadata.h>
#include <ruclock.h>

static int parse_args(int argc, char *argv[]);
void perform_read_operation(const char *command);
void get_telemetry_data_for_test();
void test_for_all_command();
void test_start_training(const char *args);
void test_update_training_status();

// 定义命令枚举
typedef enum {
  OPT_READ,
  OPT_GET_TELEMETRY_DATA,
  OPT_TEST,
  OPT_START_TRAINING,
  OPT_UPDATE_TRAINING,
} CommandOption;

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  return 0;
}



/** helper function */
static int parse_args(int argc, char *argv[]) {
  const struct option long_options[] = {
    {"read",                      required_argument, 0, OPT_READ},
    {"get-telemetry-data",        no_argument,       0, OPT_GET_TELEMETRY_DATA},
    {"test",                      no_argument,       0, OPT_TEST},
    {"start-training",            required_argument, 0, OPT_START_TRAINING},     // 新增
    {"update-training",           no_argument,       0, OPT_UPDATE_TRAINING},    // 新增
    {0, 0, 0, 0},
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
    switch (opt) {
      case OPT_READ:     
          perform_read_operation(optarg);       break;
      case OPT_GET_TELEMETRY_DATA:
          get_telemetry_data_for_test();        break;
      case OPT_TEST:    
          test_for_all_command();               break;
      case OPT_START_TRAINING:
          test_start_training(optarg);          break;
      case OPT_UPDATE_TRAINING:
          test_update_training_status();        break;
      default:
          printf("Usage: %s [OPTION...] [args]\n\n", argv[0]);
          printf("\t--read [command]            执行读取命令\n");
          printf("\t--get-telemetry-data        获取遥测数据\n");
          printf("\t--test                      运行所有命令测试\n");
          printf("\t--start-training <ns>,<sec> 启动训练测试 (例如: 50,300)\n"); // 新增
          printf("\t--update-training           更新训练状态测试\n");              // 新增
          printf("\n");
          exit(0);
    }
  }
  return 0;
}

void perform_read_operation(const char *command)
{
  char response[MAX_RESPONSE_LENGTH] = {0};
  csac_send_command(command, response);
}


void get_telemetry_data_for_test()
{
  T_CSAC_telemetry telemetry;
  get_telemetry_data(&telemetry);
}


void test_for_all_command() {
  // int freq;
  // int ret = get_current_frequency(&freq);
  // printf("get_current_frequency: ret = %d, freq = %d\n", ret, freq);

  // int freq = -123000;
  // int updated_freq;
  // int ret = set_absolute_frequency(freq, &updated_freq);

  test_print_all_metadata();
}


// 新增：测试启动训练的函数
void test_start_training(const char *args) {
  uint8_t ns_threshold;
  uint16_t time_constant;
  
  // 解析参数
  if (sscanf(args, "%hhu,%hu", &ns_threshold, &time_constant) != 2) {
      ERR_LOG("错误: 无效的参数格式。请使用: --start-training <ns>,<sec>\n");
      return;
  }
  
  DEBUG_LOG("启动训练测试: 阈值=%dns, 时间常数=%d秒\n", ns_threshold, time_constant);
  
  // 执行训练启动
  DisciplineStartResult result = ruclock_discipliner_start_training(ns_threshold, time_constant);
  
  // 输出结果
  if (result.code == DISCIPLINE_START_OK) {
    DEBUG_LOG("训练启动成功！\n");
  } else {
    ERR_LOG("训练启动失败！\n");
  }
}

// 新增：测试更新训练状态的函数
void test_update_training_status() {
  DEBUG_LOG("更新训练状态测试...\n");
  
  // 执行状态更新
  bool completed = ruclock_discipliner_update_training_status();
  
  // 输出结果
  if (completed) {
    DEBUG_LOG("训练已完成！\n");
  } else {
    ERR_LOG("训练仍在进行中...\n");
  }
}