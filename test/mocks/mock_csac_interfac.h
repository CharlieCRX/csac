#ifndef MOCK_RUCLOCK_H
#define MOCK_RUCLOCK_H
#include <stdint.h>
#include <stdio.h>
#include "enums.h"

int csac_send_command(const char *command, char *response);
int get_telemetry_data(T_ruclock_telemetry *telemetry);


// moc
void mock_set_telemetry_response(const char *resp);
void mock_set_1PPS_sync_response(const char *resp);
#endif