#include <stdio.h>
#include <getopt.h>
#include "ruclock.h"
#include "metadata.h"

static int parse_args(int argc, char *argv[]);
void perform_read_operation(const char *command);
void get_telemetry_headers_for_test();
void get_telemetry_data_for_test();
void test_for_all_command();

// 定义命令枚举
typedef enum {
  OPT_READ,
  OPT_GET_TELEMETRY_HEADERS,
  OPT_GET_TELEMETRY_DATA,
  OPT_TEST
} CommandOption;

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  return 0;
}



/** helper function */
static int parse_args(int argc, char *argv[]) {
  const struct option long_options[] = {
    {"read",                      required_argument, 0, OPT_READ},
    {"get-telemetry-headers",     no_argument,       0, OPT_GET_TELEMETRY_HEADERS},
    {"get-telemetry-data",        no_argument,       0, OPT_GET_TELEMETRY_DATA},
    {"test",                      no_argument,       0, OPT_TEST},
    {0, 0, 0, 0},
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
    switch (opt) {
      case OPT_READ:   	
          perform_read_operation(optarg);       break;
	    case OPT_GET_TELEMETRY_HEADERS:
          get_telemetry_headers_for_test();     break;
      case OPT_GET_TELEMETRY_DATA:
		      get_telemetry_data_for_test();        break;
      case OPT_TEST:    
          test_for_all_command();               break;
	    default:
          printf("Usage: %s [OPTION...] [args]\n\n", argv[0]);
          printf("\t--read  [port]            read for test\n");
          printf("\t--help                    help message\n");
          printf("\t--get-telemetry-headers   return telemetry headers\n");
          printf("\t--get-telemetry-data      return telemetry data\n");
          printf("\n");
          exit(0);
    }
  }
  return 0;
}

void perform_read_operation(const char *command)
{
  char response[MAX_RESPONSE_LENGTH] = {0};
  get_current_value(command, response);
}


void get_telemetry_data_for_test()
{
  T_ruclock_telemetry telemetry;
  get_telemetry_data(&telemetry);
}

void get_telemetry_headers_for_test()
{
  char telemetry_name[MAX_RESPONSE_LENGTH];
  get_telemetry_headers(telemetry_name);
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