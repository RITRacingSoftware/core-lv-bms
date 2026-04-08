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

// STM enable pin
#define STM_ENA_PORT GPIOB
#define STM_ENA_PIN GPIO_PIN_1

// Charger connected
#define CHG_IN_PORT GPIOB
#define CHG_IN_PIN GPIO_PIN_10

// Charging enable
#define CHG_ENA_PORT GPIOA
#define CHG_ENA_PIN GPIO_PIN_7

// LV enable pin
#define LV_ENA_PORT GPIOA
#define LV_ENA_PIN GPIO_PIN_6

void GPIO_init();
void GPIO_Task_Update();