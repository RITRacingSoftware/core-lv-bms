#pragma once

#include <stdbool.h>

#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_15

//heartbeat
#define HEARTBEAT_PORT GPIOB
#define HEARTBEAT_PIN GPIO_PIN_15

// LED 1
#define LED1_PORT GPIOC
#define LED1_PIN GPIO_PIN_1

// LED 2
#define LED2_PORT GPIOC
#define LED2_PIN GPIO_PIN_2

void GPIO_init();
void GPIO_Task_Update();