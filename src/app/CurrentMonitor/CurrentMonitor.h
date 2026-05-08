#pragma once

#include <stdbool.h>

// Current sensor pin
#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_15

#define CS_REF_PORT GPIOB
#define CS_REF_PIN  GPIO_PIN_14

void CurrentMonitor_init();
bool CurrentMonitor_task_update();
