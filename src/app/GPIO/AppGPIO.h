#pragma once

#include <stdbool.h>

#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_15

#define LED1_PORT GPIOC
#define LED1_PIN GPIO_PIN_1

#define LED2_PORT GPIOC
#define LED2_PIN GPIO_PIN_2

bool GPIO_init();