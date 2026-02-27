#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

extern const uint8_t num_cells_per_chip[NUM_CHIPS];
extern const uint8_t num_therms_per_chip[NUM_THERMS];

bool LVBMS_init();
void LVBMS_Task_Update();