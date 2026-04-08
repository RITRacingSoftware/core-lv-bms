#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

extern const uint8_t num_cells_per_chip[NUM_CHIPS];
extern const uint8_t num_therms_per_chip[NUM_CHIPS];

bool LVBMS_init();
bool LVBMS_1kHz();
bool LVBMS_1Hz();