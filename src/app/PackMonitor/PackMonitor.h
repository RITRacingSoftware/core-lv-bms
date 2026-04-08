#pragma once

#include "config.h"
#include <stdbool.h>

extern float max_temp;
extern float min_temp;
extern float avg_temp;
extern float sum_temp; 

extern float max_cell_volt; 
extern float min_cell_volt;
extern float sum_cell_volt;
extern float avg_cell_volt;
extern float cell_volt_diff;

extern float chip_volt;

extern float cell_volt_arr[NUM_CELLS];
extern float chip_volt_arr[NUM_CHIPS];
extern float cell_temp_arr[NUM_THERMS];


void PackMonitor_init();
bool PackMonitor_task_update();