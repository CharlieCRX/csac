#ifndef MOCK_CSAC_INTERFACE_H
#define MOCK_CSAC_INTERFACE_H
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

int csac_send_command(const char *command, char *response);
int get_telemetry_data(T_CSAC_telemetry *telemetry);


// moc
void mock_set_telemetry_response(const char *resp);
void mock_set_1PPS_sync_response(const char *resp);
void mock_set_mode_response(const char *resp);
void mock_set_comm_failure(const bool failure);
void mock_set_operation_failure_mask(uint16_t mask);
void mock_set_parse_failure(const bool failure);
uint16_t mock_get_current_modes(void);
void mock_reset(void);
#endif