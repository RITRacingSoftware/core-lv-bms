#pragma once

#include <stdbool.h>

// Current sensor pin
#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_15

void CurrentMonitor_init();
bool CurrentMonitor_task_update();
