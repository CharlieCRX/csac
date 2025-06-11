#ifndef RUCLOCK_H
#define RUCLOCK_H
#include <stdint.h>
#include <pthread.h>
#include "enums.h"

/* 频率调整*/
int get_current_frequency(int *freq);
int set_absolute_frequency(int abs_freq, int *now_freq);
int adjust_relative_frequency(int offset_freq, int *now_freq);
int latch_current_frequency();

/* 操作模式*/
int set_operating_modes(Telemetry_Modes_type type);

#endif