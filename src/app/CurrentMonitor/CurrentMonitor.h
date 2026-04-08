#pragma once

#include <stdbool.h>

// Current sensor pin
#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_15

bool CurrentMonitor_task_update();